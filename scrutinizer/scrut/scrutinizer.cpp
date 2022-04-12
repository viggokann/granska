/* scrutinizer.cc
 * author: Johan Carlberger
 * last change: 2002-04-09
 * comments: Scrutinizer class
 */

#include "scrutinizer.h"
#include "matchingset.h"
#include "message.h"
#include "ruleset.h"
#include "timer.h"
#include "misc/xmloutput.h"
#include <sstream>
#include <fstream>
#include <stdlib.h>

extern "C" {
#include "libstava.h"
#include "utf2latin1.h"
}

#ifdef PROBCHECK
#include "prob.h"
#include "output.h"
#include <sstream>
#endif // PROBCHECK

// these are used for the xml output
#include "defines.h"
#include "report.h"

#include <regex>

std::ofstream messageStream("messages");

static const int GE_BUF_SIZE = 200;

static Timer timer;
static Timer::type readTime, scrutTime;
//static Timer::type loadTime;  // jb: not used?


DefObj(Scrutinizer);

Scrutinizer::Scrutinizer(OutputMode mode) : ruleSet(NULL) {
  if (GramError::scrutinizer)
    Message(MSG_ERROR, "more than one scrutinizer instance is not recommended now");
  SetFormatSettings(mode);
  GramError::scrutinizer = this;
  GramError::matchingSet = &GetMatchingSet();
  gramErrorBufSize = 0;
  gramErrors = NULL;
  nGramErrors = 0;
  xAmbiguousNewWords = true; // för demon
  NewObj();
}

Scrutinizer::~Scrutinizer() {
  Message(MSG_STATUS, "deleting scrutinizer...");
  if (gramErrors) {
    for (int i=0; i<nGramErrors; i++)
      delete gramErrors[i];
    delete [] gramErrors; // jonas, delete -> delete []
  }
  GramError::Reset();
  GetMatchingSet().DeleteBuffers();
  DeleteElements();

  DelObj();
}

inline static void WriteInt(FILE *fp, int i) {
  fwrite(&i, sizeof(int), 1, fp);
}
inline static int ReadInt(FILE *fp) {
  int i;
  if (fread(&i, sizeof(int), 1, fp) <= 0) return 0;
  return i;
}

bool Scrutinizer::Load(const char *taggerLexDir, const char *ruleFile) {
  if (xTakeTime) timer.Start();
  Message(MSG_STATUS, "loading default scrutinizer...");
  if (!taggerLexDir)
    taggerLexDir = getenv("TAGGER_LEXICON");
  if (!taggerLexDir)
    taggerLexDir = TAGGERLEXICONFOLDER;
  Tagger::Load(taggerLexDir);
  if (!IsLoaded()) {
    Message(MSG_WARNING, "cannot load tagger from", taggerLexDir);
    return false;
  }
  const char *stavaDir = getenv("STAVA_LEXICON");
  if (!stavaDir) {
    stavaDir = STAVALEXICONFOLDER;
  }
  // Specialinställningar till Stava:
  xGenerateCompounds = 1;
  xAcceptCapitalWords = xAcceptSpellCapitalWords;
  //if (!StavaReadLexicon(stavaDir,1,1,1,1,1,1,(uchar*)"\t")) {
  if (!StavaReadLexicon(stavaDir,1,1,1,1,1,1,(const unsigned char *) ",")) {
    Message(MSG_WARNING, "cannot load Stava lexicons from", stavaDir);
    return false;
  }
  if (!ruleFile)
    ruleFile = getenv("SCRUTINIZER_RULE_FILE");
  if (!ruleFile)
    ruleFile = DEFAULTRULEFILE;
  haveRegexpRules = false;
  ruleSet = ReadRules(this, ruleFile);
  if (!ruleSet->IsFixed())
    return false;
  RuleTerm::nTags = Tags().Ct();
  RuleTerm::tagLexicon = &Tags();
#ifdef PROBCHECK
  Prob::report_granska_rules(ruleSet);
#endif // PROBCHECK
  if (xOptimizeMatchings) {
    char optFileName[1000];
    sprintf(optFileName, "%s.opt", ruleFile);
    FILE *fp = fopen(optFileName, "rb");
    const int magic = 6509869;
    bool ok = false;
    bool tried = false;
    
    if (fp) {
      if (!xPrintOptimization) {
	const int magic2 = ReadInt(fp);
	if (magic == magic2) {
	  tried = true;
	  Message(MSG_STATUS, "loading rule optimizations from", optFileName);
	  ok = RuleTerm::ReadMatchingOptimization(fp);
	  if (!ok)
	    Message(MSG_WARNING, "error in optimization file", optFileName);
	}
      }
      fclose(fp);
    }
    if (!ok) {
      GetRuleSet()->OptimizeMatchings();
      Message(MSG_STATUS, "saving rule optimizations to", optFileName);
      fp = fopen(optFileName, "wb");
      WriteInt(fp, magic);
      RuleTerm::SaveMatchingOptimization(fp);
      fclose(fp);


      if(tried) {
	// tried to load rule optimizations but failed, something is probably wrong with the stored optimization file
	Message(MSG_WARNING, "Rule optimization file could not be read properly. This can be caused by changing the tagger lexicon to another lexicon than the one used when creating the optimization file. If you get errors, try and running Granska again after deleting the rule '.opt' file: ", optFileName);
      }
      
    }
  }
  if (xTakeTime) loadTime = timer.Get();
  return true;
}

const Text *Scrutinizer::ReadTextFromString(const char *text) {
  // if (xVerbose) std::cout << "ReadTextFromString(\"" << text << "\")" << std::endl;
  std::string str(text);
  std::istringstream in(str);

  return ReadTextFromStream(&in);
}

const Text *Scrutinizer::ReadTextFromFile(const char *fileName) {
  std::ifstream in;
  if (!FixIfstream(in, fileName, NULL, true))
    return NULL;

  //return ReadTextFromStream(&in);
    
  // Handle UTF here?
  std::string filecontent;
  filecontent.assign( (std::istreambuf_iterator<char>(in) ),
		      (std::istreambuf_iterator<char>()    ) );

  if(looksLikeUTF(filecontent.c_str())) {
    char *tempBuf = utf2latin1Copy(filecontent.c_str());
    const Text *tempResult = ReadTextFromString(tempBuf);
    free(tempBuf);
    return tempResult;
  } else {
    in.clear();
    in.seekg(0, std::ios::beg);
    return ReadTextFromStream(&in);
  }
}

std::istream * Scrutinizer::CopyInputStream(std::istream *in) {
  copyOfInputString = "";
  char buffer[4096];
  while (in->read(buffer, sizeof(buffer))) {
    copyOfInputString.append(buffer, sizeof(buffer));
  }
  copyOfInputString.append(buffer, in->gcount());

  copyAsStream = std::istringstream(copyOfInputString);
  return &copyAsStream;
}

extern bool xReadTaggedText;// jonas, intended for use only for evaluation study 030120 - 030228
const Text *Scrutinizer::ReadTextFromStream(std::istream *in) {
  if (xTakeTime) timer.Start();

  theOriginalText.str("");
  theOriginalText.clear();
  
  if(haveRegexpRules) {
    std::istream *newStream = CopyInputStream(in);
    SetStream(newStream);
  } else {
    copyOfInputString = "";
    SetStream(in);
  }

  xTaggedText = false;
  while(nGramErrors > 0)
    delete gramErrors[--nGramErrors];
  if(xReadTaggedText) 
    ReadTaggedTextQnD();// jonas, intended for use only for evaluation study 030120 - 030228
  else
    ReadText();// jonas, this is the normal (not study 030120...) way to do things
  TagText();
  if (xTakeTime) readTime = timer.Get();
  return &theText;
}

void Scrutinizer::Scrutinize(AbstractSentence *s) {
  xCurrSentence = s;
  s->SetContentBits();
  if (xPrintMatchings) {
    std::cout << std::endl;
    if (GetMatchingSet().CheckMode()) std::cout << tab;
    std::cout << s << std::endl;
  }
  if (xOptimizeMatchings)
    RuleTerm::FindMatchingsOptimized(s);
  else
    ruleSet->FindMatchings(s);
  GetMatchingSet().TerminateSearch(s);

  xCurrSentence = NULL;
}

// Matches all the RegexpRules (rules that are on regular expression
// matched to the input text, not rules that have one or more tokens
// that are matched with regular expressions after tokenization (such
// rules are handled like normal rules).
//
// The RegexpRules require a copy of the original input text to match
// the expression to, which is normally not available (only the
// sequence of tokens is available). When ReadRules() detexts
// RegexpRules, a flag 'haveRegexpRules' is set to indicate that at
// least one RegexpRule is present, and a copy of the original text is
// stored in the variable 'copyOfInputString'.
//
// The suggestions etc. for the matching are stored in regexpMatches,
// a member of Scrutinizer.
//
// RegexpRules are only allowed to have correction suggestions on the
// form: corr("[string]"), a string that replaces the matched part of
// the text. This string can use variables to refer to parts of the
// match, for example corr("/\1 \2/"), where \1 and \2 refer to
// subgroups 1 and 2 in the matched text.
void Scrutinizer::RegexpRuleMatching() {
  // Loop through the rules, find the RegexpRules

  // Do the regexp search for each rule, apply the corrections
  
  regexpMatches.clear();
  
  const int nrules = ruleSet->NRules();
  if (nrules > 0) {
    const std::string &text = copyOfInputString; // The original input text
    
    for(int ri = 0; ri < nrules; ri++) { // for each rule ...
      Rule *r = ruleSet->GetRule(ri);
      if(strcmp(r->Type(), "RegExpRule") == 0) { // ... if it is a RegexpRule, see if it matches

	int posOffs = 0;
      
	// std::cout << "Rule no " << ri << " is a RegexpRule\n";
	RegExpRule *rr = static_cast<RegExpRule *>(r);
	std::regex ex(rr->GetRegexp());
	std::smatch m;

	std::string s = text;

	while(std::regex_search(s, m, ex)) {
	  // We have a matching section of text, output suggestions etc.

	  const char *ce = rr->GetCorr();
	  Expr *ie = rr->GetInfo();

	  if(ce != NULL) {
	    bool regexpReplace = false;
	    std::string repl = "";
	    
	    if(ce[0] == '/') {
	      int l = strlen(ce);
	      if(l > 0 && ce[l - 1] == '/') {
		// suggestion possibly contains references to subgroups in the match
		regexpReplace = true;

		std::vector<std::string> groups;
		bool haveGroups = false;

		for(int ii = 1; ii < l - 1; ii++) {
		  if(ce[ii] == '\\' && isdigit(ce[ii + 1])) {
		    unsigned int nn = 0, jj = 0;
		    for(jj = ii+1; isdigit(ce[jj]); jj++) {
		      nn = nn*10 + (ce[jj] - '0');
		    }
		    ii = jj - 1;

		    if(!haveGroups) {
		      haveGroups = true;

		      // if he suggestions refer to parts of the regexp, here are the subgroups
		      for(unsigned int i = 0; i < m.size(); i++) {
			groups.push_back(m[i]);
		      }
		    }

		    if(nn < groups.size()) {
		      repl += groups[nn];
		    } else {
		      repl += "[okänd regexpgrupp]";
		    }
		  } else {
		    repl += ce[ii];
		  } 
		}
	      }
	    }
	  
	    if(!regexpReplace) {
	      repl = ce;
	    }

	    RegexResult temp(posOffs + m.position(), posOffs + m.position() + m.length(), repl, ie->c.string, r->Name());
	    regexpMatches.push_back(temp);
	    /*
	    std::cout << "=====================================================\nRegexp Match: "
	      << rr->GetRegexp() << "\n"
	      << "from pos " << (posOffs + m.position()) << " to pos " << (posOffs + m.position() + m.length())
	      << " replace '" << m.str() << "' with '" << repl.c_str() << "'\n"
	      << " add information '" << ie->c.string << "'\n"
	      << "\n-------------------------------\n"
	      << copyOfInputString.substr(0, posOffs + m.position())
	      << repl.c_str()  
	      << copyOfInputString.substr(posOffs + m.position() + m.length(), std::string::npos)
	      << "\n----------------------------------------\n";
	    */	 
								 
	  }
	      
          // go to next match	      
          posOffs = posOffs + m.position() + m.length();
	  s = m.suffix();
	}
      }
    }
  }
}

GramError **Scrutinizer::Scrutinize(int *n) {
  if (nGramErrors > 0) {
    Message(MSG_WARNING, "same text scrutinized twice");
    *n = nGramErrors;
    return gramErrors;
  }   
#ifdef VERBOSE
  Message(MSG_STATUS, "scrutinizing text...");
#endif
  if (ruleSet->NRules() <= 0) {
    Message(MSG_WARNING, "there are no rules");
    *n = 0;
    return NULL;
  }
  for (int i=0; i<20; i++) xCase[i] = 0; // this line is the only one that uses xCase? // jonas
  if (xTakeTime) timer.Start();
  GetMatchingSet().Clear();
  RuleTerm::prepTime = 0;
  Expr::evalTime = 0;

  if(haveRegexpRules) {
    // regexp rules do not trigger during the normal search, because they have no ruleterms
    RegexpRuleMatching();
  }
  
  for (Sentence *s=theText.FirstSentence(); s; s=s->Next()) {
    Scrutinize(s);
    for (const GramError *g = s->gramError; g; g = g->Next()) {
      if (nGramErrors >= gramErrorBufSize) {
	gramErrorBufSize += GE_BUF_SIZE;
	GramError **ge = new GramError*[gramErrorBufSize]; // new OK
	if (nGramErrors > 0) {
	  memcpy(ge, gramErrors, nGramErrors*sizeof(GramError*));
	  delete [] gramErrors; // jonas delete -> delete [] 
	}
	gramErrors = ge;
      }
      gramErrors[nGramErrors++] = (GramError*) g;
    }
  }
  *n = nGramErrors;
  if (xTakeTime) scrutTime = timer.Get();
  //  if (xVerbose) {
  //    for (int i=0; i<20; i++) std::cout << xCase[i] << ' ';
  //    std::cout << std::endl; }
  return gramErrors;
}

void Scrutinizer::CheckAcceptAndDetect() {
  Message(MSG_STATUS, "checking accept and detect texts of rules...");
  SetMessageStream(MSG_STATUS, NULL);
  xPrintAllSentences = true;
  std::cout << std::endl << std::endl;
  int nWithout = 0, nMissed = 0, nFalse = 0;
  for (int i=0; i<ruleSet->NRules(); i++) {
    Rule *r = ruleSet->GetRule(i);
    const char *d = r->FirstRuleTerm()->GetDetect();
    const char *a = r->FirstRuleTerm()->GetAccept();
    if (d || a) {
      std::cout << "checking " << r->Header() << "..." << std::endl << std::endl;
      if (d) {
	std::cout << "detect: " << std::endl;
	/*
	Scrutinize(d);
	if (!FirstGramError()) {
	  Message(MSG_WARNING, r->Header(), "did not detect");
	  Message(MSG_CONTINUE, d);
	  nMissed++;
	}
	*/
      }
      if (a) {
	std::cout << "accept: " << std::endl;
	/*
	Scrutinize(a);
	if (FirstGramError()) {
	  Message(MSG_WARNING, r->Header(), "detected");
	  Message(MSG_CONTINUE, a);
	  nFalse++;
	}
	*/
      }
    } else
      nWithout++;
  }
  if (nWithout > 0)
    Message(MSG_WARNING, int2str(nWithout), "rules have neither detect or accept text");
  if (nMissed > 0)
    Message(MSG_WARNING, int2str(nMissed), "detect texts were not detected");
  if (nFalse > 0)
    Message(MSG_WARNING, int2str(nFalse), "accept texts were detected");
  Message(MSG_COUNTS, "during accept and detect check");
}

#ifdef PROBCHECK
void Scrutinizer::PrintRegexpAlarms() {
  Prob::Output &o = Prob::output();
  if(regexpMatches.size() > 0) {
    o.push("regularExpressions");
    for(unsigned int i = 0; i < regexpMatches.size(); i++) {
      RegexResult m = regexpMatches[i];
      o.push("regexMatch");
      o.attr("markBeg", m.startpos);
      o.attr("markEnd", m.endpos);
      o.add("rule", m.ruleName);
      o.add("info", m.info);
      o.add("mark", copyOfInputString.substr(m.startpos, m.endpos - m.startpos));
      o.add("sugg", m.replacement);
      o.pop();
    }
    o.pop();
  }
}
#else
void Scrutinizer::PrintRegexpAlarms(std::ostream &out) {
  if(regexpMatches.size() > 0) {
    out << "<regularExpressions>\n";
    for(unsigned int i = 0; i < regexpMatches.size(); i++) {
      RegexResult m = regexpMatches[i];
      out << "<regexMatch";
      out << " \"markBeg\"=" << m.startpos;
      out << " \"markEnd\"=" << m.endpos;
      out << ">\n";
      out << "<rule>" << m.ruleName << "</rule>\n";
      out << "<info>" << m.info << "</info>\n";
      out << "<mark>" << copyOfInputString.substr(m.startpos, m.endpos - m.startpos) << "</mark>\n";
      out << "<sugg>" << m.replacement << "</sugg>\n";
      out << "</regexMatch>\n";
    }
    out << "</regularExpressions>\n";
  }
}
#endif

void Scrutinizer::PrintResult(std::ostream &out) {
  if (xPrintMatchings || xPrintOptimization || !xPrintGramErrors) return;
  xPrintAllWords = true;
  xPrintOneWordPerLine = false;
  if (xTakeTime) {
    out.setf(std::ios::fixed); out.precision(4);
    theText.CountContents();
    if (xOptimizeMatchings) GetRuleSet()->PrintEvaluationTimes();
    out << int(loadTime * 1000.0 / Timer::clocks_per_sec())
	<< tab << "ms to load scrutinizer" << std::endl
	<< int((double)Timer::clocks_per_sec() * theText.NWordTokens()/readTime)
	<< tab << "word-tokens/s read and tagged (" << theText << ')' << std::endl
	<< int((double)Timer::clocks_per_sec() * theText.NWordTokens()/scrutTime)
	<< tab << "word-tokens/s scrutinized (" << nGramErrors << " gram-errors)" << std::endl
	<< int((double)Timer::clocks_per_sec() * theText.NWordTokens()/(RuleTerm::prepTime))
	<< tab << "word-tokens/s prepared for matching" << std::endl
	<< int((double)Timer::clocks_per_sec() * theText.NWordTokens()/(ruleSet->matchTime))
	<< tab << "word-tokens/s in TryMatching() including Eval()" << std::endl
	<< int((double)Timer::clocks_per_sec() * theText.NWordTokens()/(Expr::evalTime))
	<< tab << "word-tokens/s in Eval()" << std::endl
	<< int((double)Timer::clocks_per_sec() * theText.NWordTokens()/(scrutTime+readTime))
	<< tab << "word-tokens/s read, tagged and scrutinized" << std::endl
	<< int((double)Timer::clocks_per_sec() * theText.NWordTokens()/(scrutTime+readTime+loadTime))
	<< tab << "word-tokens/s read, tagged and scrutinized (incl. loading)" << std::endl;
    return;
  }
  if (xPrintHTML) {
    out << "<H4>Förklaringar till markeringarna</H4>"
	<< "<LI>Misstänkta områden i texten markeras med "
	<< xRed << "rött" << xNoColor << "</LI>" << xEndl
	<< "<LI>Ersättningsförslagen presenteras därefter och ändringar markeras med "
	<< xGreen << "grönt" << xNoColor << "</LI>" << xEndl
	<< "<LI>En kortfattad" << xBlue << " blå " << xNoColor
	<< "kommentar skrivs efter varje förslag</LI>" << xEndl
	<< "<LI>En länk till utförligare information om feltypen"
	<< " finns efter vissa kommentarer</LI>" << xEndl;
    if (!xSuggestionSameAsOriginalMeansFalseAlarm) 
      out << "<LI>Ett " << xRed << 'F' << xNoColor
	  << " efter förslaget betyder att inga ändringar gjorts, "
	  << "vilket indikerar ett möjligt falskt alarm</LI>" << xEndl;
    if (xAcceptNonImprovingCorrections)
      out << "<LI>Ett " << xRed << 'E' << xNoColor
	  << " efter förslaget betyder att rättelsen kan ge ett annat fel</LI>" << xEndl;
    out << "<LI>Om en mening innehåller flera misstänkta"
	<< " felområden presenteras samma mening flera gånger</LI>"
	<< xEndl << xEndl
      // << "Skicka gärna synpukter till <A HREF=\"mailto:knutsson@nada.kth.se\">&lt;knutsson@nada.kth.se&gt;</A>"
	<< "<HR WIDTH=\"100%\"></H1>";
  }

#ifdef PROBCHECK
  Prob::Output &o = Prob::output();

  if(xPrintRuleCount){
    o.push("rules");
    std::map<std::string, int>::iterator iter;
    for(iter = ruleCount.begin(); iter != ruleCount.end(); iter++){
      o.push("rule");
      o.add("name", iter->first);
      o.add("triggered", iter->second);
      o.pop();
    }
    o.pop(); //rules
  }

#ifndef DEVELOPER_OUTPUT
  // print all the actual sentences, before the <scrutinizer> part
  // some old scripts expect the output in this order
  for(const Sentence *s=theText.FirstSentence(); s; s=s->Next()) {
    const GramError *g = 0;
      
    bool found = false;
    for(g = s->GetGramError(); g; g = g->Next()) {
      g->Report();
      if(g->IsError())
	found = true;
    }

    // no error found, don't output sentence
    if(!found && !xPrintAllSentences)
      continue;

    if(found || xPrintAllSentences) {
      int offset = s->GetWordToken(0 + 2)->Offset();
      o.push("s");
      o.attr("ref", offset);

      // print tokens
      int nWords = 0;
      WordToken **tokens = 0;
	      
#ifdef NO_SENTENCE_DELIMITERS
      nWords = s->NWords();
      tokens = s->GetWordTokensAt(0 + 2);
#else // NO_SENTENCE_DELIMITERS
      nWords = s->NTokens();
      tokens = s->GetWordTokensAt(0);
#endif // NO_SENTENCE_DELIMITERS
		
      o.add("words", nWords);

      std::string clearText;
      for(int i = 0; i < nWords; i++)
	{
	  clearText += tokens[i]->RealString();
	  if(i < nWords - 1 && tokens[i]->HasTrailingSpace()) // jsh, fixed trailing spaces
	    clearText += " ";
	}
  
      o.add("text",  Misc::fixXML(clearText.c_str()));

      if(s->IsHeading())
	o.add("heading");
      if(s->EndsParagraph())
	o.add("paragraph");

      // get tokens and words
      o.push("contents");
      int i;
      for(i = 0; i < nWords; i++)
	{
	  o.add("w", Misc::fixXML(tokens[i]->RealString()));
	  o.attr("no", i);
	  // Is this used for anything?
	  // if(tokens[i]->Offset())
	  //   o.attr("ref", tokens[i]->Offset() - s->GetWordToken(0 + 2)->Offset());
#ifdef DEVELOPER_OUTPUT
	  o.attr("tag", tokens[i]->SelectedTag()->String());
#else  // DEVELOPER_OUTPUT
	  o.attr("tag", tokens[i]->SelectedTag()->Index());
#endif // DEVELOPER_OUTPUT
	  o.attr("lemma", Misc::fixXML(tokens[i]->LemmaString()));
	}
      o.pop();  // push("contents");

      o.pop();  // push("sentence");

    } // found or print all sentences
  }
#endif // DEVELOPER_OUTPUT


  // print error reports
    

  o.push("scrutinizer");

  for(const Sentence *s=theText.FirstSentence(); s; s=s->Next()) {
    const GramError *g = 0;

    // jb: this is used to determine whether there is an actual
    // error or just @recog rules. If error, we will print.
    bool found = false;
    for(g = s->GetGramError(); g; g = g->Next())
      {
	g->Report();
	if(g->IsError())
	  found = true;
      }

    // no error found, don't output sentence
    if(!found && !xPrintAllSentences)
      continue;

    int offset = s->GetWordToken(0 + 2)->Offset();
    o.push("s");
    o.attr("ref", offset);
#ifdef DEVELOPER_OUTPUT
    if(s->GetGramError() || xPrintAllSentences)
      {
	o.add("tokens", s->NTokens());

	//std::ostringstream ss;
	//ss << s;                    
	//o.add("text", ss.str().c_str());                    
	//std::string orgText = fixXML(s->getOriginalText());

	// The get and set of OriginalText does not work properly // Jonas
	// o.add("text", Misc::fixXML(s->getOriginalText())); //Oscar

	// This part ?? ------------------------------------------------------
	int nWords = 0;
	WordToken **tokens = 0;
#ifdef NO_SENTENCE_DELIMITERS
	nWords = s->NWords();
	tokens = s->GetWordTokensAt(0 + 2);
#else // NO_SENTENCE_DELIMITERS
	nWords = s->NTokens();
	tokens = s->GetWordTokensAt(0);
#endif // NO_SENTENCE_DELIMITERS
	std::string clearText;
	for(int i = 0; i < nWords; i++)
	  {
	    clearText += tokens[i]->RealString();
	    if(i < nWords - 1 && tokens[i]->HasTrailingSpace()) // jsh, fixed trailing spaces
	      clearText += " ";
	  }
  
	o.add("text",  Misc::fixXML(clearText.c_str()));
	// This part ^^ ?? ------------------------------------------------------

	
	if(!s->IsHeading())
	  o.add("heading");
	if(s->EndsParagraph())
	  o.add("paragraph");
	o.push("contents");
	for(int i = 0; i < s->NTokens(); i++)
	  {
	    const WordToken *token = s->GetWordToken(i);
	    const Tag *tag = token->SelectedTag();

	    //std::string xmlOkWord = fixXML(token->RealString());
	    //if(xmlOkWord.length()>0)
	    o.add("w", Misc::fixXML(token->RealString()));
	    //else
	    //    o.add("w", token->RealString());

	    o.attr("no", i);
	    if(token->Offset())
	      o.attr("ref", token->Offset() - offset);
	    o.attr("tag", tag->String());
                            
	    //std::string xmlOkLemma = fixXML(token->LemmaString());
	    //if(xmlOkLemma.length()>0)
	    //    o.attr("lemma", xmlOkLemma);
	    //else
	    o.attr("lemma", Misc::fixXML(token->LemmaString()));
	    //if(token->LemmaString()[0] != '"')
	    //    o.attr("lemma", token->LemmaString());
	    //else
	    //    o.attr("lemma", "'");
	  }
	o.pop();  // push("contents");
      }
#endif // DEVELOPER_OUTPUT
	    
    if(found)
      {
	o.push("gramerrors");
	for(g = s->GetGramError(); g; g = g->Next())
	  if(g->IsError())
	    g->Output();
	o.pop();  // push("gramerrors");
      }

    o.pop();  // push("sentence");
  }

  if(haveRegexpRules) {
    PrintRegexpAlarms();
  }
  
  o.pop();  // push("scrutinizer");

  Prob::print(this);
#endif
#ifndef PROBCHECK
  // Old style (not PROBCHECK)
  for(const Sentence *s=theText.FirstSentence(); s; s=s->Next())
    {
      const GramError *g = 0;

      // jb: this is used to determine whether there is an actual
      //     error or just @recog rules. If error, we will print.
      bool found = false;
      for(g = s->GetGramError(); g; g = g->Next())
	{
	  g->Report();
	  if(g->IsError())
	    found = true;
	}
      // no error found, don't output sentence
      if(!found && !xPrintAllSentences)
	continue;

      if(s->GetGramError() || xPrintAllSentences)
	{
	  out << s << ' ';
	  if(s->GetGramError())
	    {
	      if(!s->IsHeading()) out << xEndl; 
	    }
	  else if(s->EndsParagraph() && !s->IsHeading())
	    out << xEndl << xEndl;
	}
      for(g = s->GetGramError(); g; g = g->Next())
	if(g->IsError())
	  out << g << xEndl;
    }
  PrintRegexpAlarms(out);
#endif

}

// for use in granskaapi.
char* Scrutinizer::GetResult() {
  xPrintAllWords = true;
  xPrintOneWordPerLine = false;
    
#ifdef PROBCHECK
  Prob::Output &o = Prob::output();
  o.isLib();
  o.init();
  o.push("rules");
  //std::ostringstream out;
  std::map<std::string, int>::iterator iter;
  for(iter = ruleCount.begin(); iter != ruleCount.end(); iter++){
    o.push("triggeredrule");
    o.add("name", iter->first);
    o.add("triggered", iter->second);
    o.pop();
    //out << iter->first << "	" << iter->second << "\n";
  }
  o.pop(); //rules
  //o.add("rules", out.str());
  o.push("scrutinizer");
  for(const Sentence *s=theText.FirstSentence(); s; s=s->Next())
    {
      const GramError *g = 0;
      // jb: this is used to determine whether there is an actual
      // error or just @recog rules. If error, we will print.
      bool found = false;
      for(g = s->GetGramError(); g; g = g->Next())
	{
	  g->Report();
	  if(g->IsError())
	    found = true;
	}
      // no error found, don't output sentence
      if(!found && !xPrintAllSentences)
	continue;

      int offset = s->GetWordToken(0 + 2)->Offset();
      o.push("s");
      o.attr("ref", offset);
#ifdef DEVELOPER_OUTPUT
      if(s->GetGramError() || xPrintAllSentences)
	{
	  o.add("tokens", s->NTokens());

	  //std::ostringstream ss;
	  //ss << s;                    
	  //o.add("text", ss.str().c_str());                    
	  //std::string orgText = fixXML(s->getOriginalText());

	  //o.add("text", Misc::fixXML(s->getOriginalText())); //Oscar

#ifdef NO_SENTENCE_DELIMITERS
	  int nW = s->NWords();
	  WordToken **tokens = s->GetWordTokensAt(0 + 2);
#else // NO_SENTENCE_DELIMITERS
	  int nW = s->NTokens();
	  WordToken **tokens = s->GetWordTokensAt(0);
#endif // NO_SENTENCE_DELIMITERS

	  std::string clearText;
	  for(int i = 0; i < nW; i++)
	    {
	      clearText += tokens[i]->RealString();
	      if(i < nW - 1 && tokens[i]->HasTrailingSpace()) // jsh, fixed trailing spaces
		clearText += " ";
	    }
  
	  o.add("text",  Misc::fixXML(clearText.c_str()));
	  
	  /*
	    if(!s->IsHeading())
	    o.add("heading");
	    if(s->EndsParagraph())
	    o.add("paragraph");*/
	  o.push("contents");
	  for(int i = 0; i < s->NTokens(); i++)
	    {
	      const WordToken *token = s->GetWordToken(i);
	      const Tag *tag = token->SelectedTag();

	      //std::string xmlOkWord = fixXML(token->RealString());
	      //if(xmlOkWord.length()>0)
	      o.add("w", Misc::fixXML(token->RealString()));
	      //else
	      //    o.add("w", token->RealString());

	      o.attr("no", i);
	      if(token->Offset())
		o.attr("ref", token->Offset() - offset);
	      o.attr("tag", tag->String());
                            
	      //std::string xmlOkLemma = fixXML(token->LemmaString());
	      //if(xmlOkLemma.length()>0)
	      //    o.attr("lemma", xmlOkLemma);
	      //else
	      o.attr("lemma", Misc::fixXML(token->LemmaString()));
	      //if(token->LemmaString()[0] != '"')
	      //    o.attr("lemma", token->LemmaString());
	      //else
	      //    o.attr("lemma", "'");
	    }
	  o.pop();  // push("contents");
	}
#endif // DEVELOPER_OUTPUT
      if(found)
	{
	  o.push("gramerrors");
	  for(g = s->GetGramError(); g; g = g->Next())
	    if(g->IsError())
	      g->Output();
	  o.pop();  // push("gramerrors");
	}

      o.pop();  // push("sentence");
    }
  o.pop();  // push("scrutinizer");
  //std::ostringstream* os = o.getStream();
  //o.pop(); //root

  Prob::print(this);

  std::string str = o.getCharP(); // This closes the output, so we need to do all the printing before this / Jonas

  // theOriginalText.str(std::string());
  theOriginalText.str("");
  theOriginalText.clear();
  
  ruleCount.clear();
#else
  std::string str = ""; // Is this OK? At least this compiles... /Jonas
#endif
  char* ch = new char[str.size()+1];
  strcpy(ch, str.c_str());
  return ch;
}
// std::string Scrutinizer::fixXML(std::string word) {
//     //Oscar, fix of bad XML output
//     std::ostringstream xmlOkWord;
//     int len = word.length();
//     int prevOffset = -1;
//     int offset;
//     char c;    
//     for(offset = 0; offset < len; offset++) {        
//         c = word.at(offset);
//         if(c=='&'||c=='\''||c=='\"'||c=='<'||c=='>') {
//             if(prevOffset < offset-1)
//                 xmlOkWord << word.substr(prevOffset+1,offset-prevOffset-1);
//             switch(c) {
//             case '&': xmlOkWord << "&amp;"; break;
//             case '\'': xmlOkWord << "&apos;"; break;
//             case '\"': xmlOkWord << "&quot;"; break;
//             case '<': xmlOkWord << "&lt;"; break;
//             case '>': xmlOkWord << "&gt;"; break;            
//             }
//             prevOffset=offset;
//         }
//     }
//     if(prevOffset != 0 && prevOffset < --offset) {        
//         xmlOkWord << word.substr(prevOffset+1, offset-prevOffset);
//     }
//     std::string ret = xmlOkWord.str();
    
//     if(ret.length()==0) return word;
//     else return ret;    
// }

// std::string Scrutinizer::fixXML(const char* word) {
//     //Oscar, fix of bad XML output
//     //should check if the Token can have any bad XML first    
//     std::ostringstream xmlOkWord;
//     int prevOffset = -1;
//     int offset;
//     char c;
//     for(offset = 0; word[offset]; offset++) {
//         c=word[offset];
//         if(c=='&'||c=='\''||c=='\"'||c=='<'||c=='>') {
//             if(prevOffset < offset-1)
//                 xmlOkWord.write(word+prevOffset+1,offset-prevOffset-1);
//             switch(c) {
//             case '&': xmlOkWord << "&amp;"; break;
//             case '\'': xmlOkWord << "&apos;"; break;
//             case '\"': xmlOkWord << "&quot;"; break;
//             case '<': xmlOkWord << "&lt;"; break;
//             case '>': xmlOkWord << "&gt;"; break;
//             }
//             prevOffset=offset;
//         }
//     }    
//     if(prevOffset != -1 && prevOffset < --offset)
//         xmlOkWord.write(word+prevOffset+1,offset-prevOffset);
//     return xmlOkWord.str();        
// }
bool Scrutinizer::IsSoundSpellOK(const char *s, Token token) {
  bool b = (SoundWord((unsigned char *) s)) ? true : false;
  if (b) return true;
  return false;
}

bool Scrutinizer::IsSpellOK(const char *s, Token token) {
  //  if (xVerbose)
  //    std::cout << xCurrentRule << " spell-checking: " << s << ' ' << token << std::endl;
  switch(token) {
  case TOKEN_E_MAIL:
  case TOKEN_MATH:
  case TOKEN_CARDINAL:
    return true;
  case TOKEN_BAD_CARDINAL:
    // return false here gives a lot of false alarms
    // such as "Tabell 1.1"
    return true; // return false;
  case TOKEN_PARAGRAPH:
    if (*s++ != '§') {
      while(*s && !IsSpace(*s)) s++;
      if (!IsSpace(*s)) return false;
      if (*++s != '§') return false;
      if (!*++s) return true;
      return false;
    }
    if (!IsSpace(*s++)) return false;
    if (!IsDigit(*s)) return false;
    return true;
  case TOKEN_PERCENTAGE:
    for (; *s != '%' && *s != 'p'; s++);
    if (IsSpace(*(s-1)))
      return true;
    return false;
  case TOKEN_TIME:
    if (*s == 'k') {
      if (!strncmp(s, "klockan ", 8))
	s += 8;
      else if (!strncmp(s, "kl. ", 4))
	s += 4;
      else return false;
    }
    if (IsDigit(*s)) s++; else return false;
    if (IsDigit(*s)) s++;
    if (!*s || *s == '.')
      return true;
    return false;
  case TOKEN_YEAR:
    for (; *s; s++) {
      if (IsPunct(*s) && IsPunct(s[1]))
	return false;
      if (IsLetter(*s) && IsDigit(s[1]))
	return false;
    }
    return true;
  case TOKEN_DATE: {
    if (Lower(*s) == 'd') {
      s += 3;
      if (!IsSpace(*s++)) return false;
      if (IsSpace(*s)) return false;
    }
    char ss[100];
    for (int k=0; k<2; k++) {
      int i;
      for (i=0; IsDigit(*s); i++)
	ss[i] = *s++;
      ss[i] = '\0';
      int n = atoi(ss);
      if (n < 1 && n > 31) return false;
      if (*s != '-') break;
      s++;
    }
    if (!IsSpace(*s++)) return false;
    if (strncmp(s, "januari", 7) && strncmp(s, "februari", 8) && strncmp(s, "mars", 4) &&
	strncmp(s, "april", 5) && strncmp(s, "maj", 3) && strncmp(s, "juni", 4) &&
	strncmp(s, "juli", 4) && strncmp(s, "augusti", 7) && strncmp(s, "september", 8) && 
	strncmp(s, "oktober", 7) && strncmp(s, "november", 8) && strncmp(s, "december", 8))
      return false;
    while(IsLetter(*s)) s++;
    if (!*s) return true;
    if (!IsSpace(*s++)) return false;
    if (IsSpace(*s)) return false;
    return true;
  }
  case TOKEN_SPLIT_WORD: {
    for (int p = strlen(s)-2; p>0; p--)
      if (IsSpace(s[p]))
	return (StavaWord((unsigned char *) (s+p+1))) ? true : false;
    Message(MSG_WARNING, s, "not a split-word");
    return true;
  }
  case TOKEN_ABBREVIATION: {
    if (!strcmp(s, "m") || !strcmp(s, "m.")) {
      Word *w = FindMainWord("meter");
      if (w->TextFreq() > 0)
	return false;
      return true;
    }
    return false;
  }
  default: {
    bool b = (StavaWord((unsigned char *) s)) ? true : false;
    if (!b) return false;
    for (const char *t=s; *t; t++)
      if (IsDigit(*t) && IsLetter(t[1]) && IsLetter(t[2]))
	return false;
    return true;
  }
  }
}

char *Scrutinizer::SpellOK(const char *s, Token token) {
  //  if (xVerbose)
  //    std::cout << xCurrentRule << " spell-corr: " << s << ' ' << token << std::endl;
  static char string[MAX_WORD_LENGTH * MAX_SUGGESTIONS];
  switch(token) {
  case TOKEN_PARAGRAPH:
    // Jonas 2019, returning a string literal (const char *) is
    // actually not OK, since this string will be tokenized with
    // strtok later (which potentially modifies the string). In the
    // current code, strtok will replace \t with NULL, and since the
    // string literal here does not contain \t it will not be
    // modified, so for now it is safe to cast the const away (but
    // if other parts of the code are later modified, things may
    // break).
    return (char *) "§ 7";
  case TOKEN_SPLIT_WORD: {
    for (int p = strlen(s)-2; p>0; p--)
      if (IsSpace(s[p])) {
	char *r = (char*) StavaCorrectWord((unsigned char *) (s+p+1));
	if (r) {
	  strcpy(string, r);
	  free(r);
	  return string;
	}
      }
    return (char *) ""; // see "Jonas 2019" comment above
  }
  case TOKEN_ABBREVIATION: {
    if (!strcmp(s, "m") || !strcmp(s, "m."))
      return (char *) "meter"; // see "Jonas 2019" comment above
    int len = strlen(s);
    if (s[len-1] == '.') {
      strcpy(string, s);
      string[len-1] = '\0';
    } else {
      strcpy(string, s);
      strcat(string, ".");
    }
    if (FindMainWord(string))
      return string;
    return (char *)  ""; // see "Jonas 2019" comment above
  }
  case TOKEN_PERCENTAGE: {
    char *ss = string;
    for (; *s; s++) {
      if (*s == '%' || *s == 'p') 
	if (!IsSpace(*(ss-1)))
	  *ss++ = ' ';
      *ss++ = *s;
    }
    *ss = '\0';
    return string;
  }
  case TOKEN_TIME: {
    if (*s == 'k') {
      if (!strcmp(s, "klockan")) strcpy(string, "klockan ");
      else strcpy(string, "kl. ");
    } else *string = '\0';
    char *ss = string;
    for (; *ss; ss++);
    for (; *s; s++) if (IsDigit(*s)) *ss++ = *s;
    if (IsDigit(*(ss-3))) {
      *ss = *(ss-1); *(ss-1) = *(ss-2); *(ss-2) = '.'; ss++; }
    *ss = '\0';
    return string;
  }
  case TOKEN_YEAR: {
    char *ss = string;
    for (; *s; s++)
      if (IsPunct(*s) && IsPunct(s[1])) {
	*ss++ = '-'; s++;
      } else if (IsLetter(*s) && IsDigit(s[1])) {
	*ss++ = *s; *ss++ = ' ';
      } else
	*ss++ = *s;
    *ss = '\0';
    return string;
  }
  case TOKEN_DATE: {
    char *ss = string;
    *ss = '\0';
    if (!strncmp(s, "den", 3)) {
      strcpy(ss, "den ");
      s+=3;
      while(IsSpace(*s)) s++;
    }
    char tt[MAX_WORD_LENGTH], *t = tt;
    int i;
    const char *u = s;
    for (i=0; IsDigit(*s); i++)
      t[i] = *s++;
    t[i] = '\0';
    int n = atoi(t);
    if (n < 1)
      strcat(ss, "1");
    else if (n > 31)
      strcat(ss, "31");
    else if (n < 10)
      strncat(ss, u, 1);
    else
      strncat(ss, u, 2);
    strcat(ss, " ");
    if (!strncmp(s, ":e", 2))
      s+=2;
    while(IsSpace(*s)) s++;
    switch(Lower(*s)) {
    case 'j': if (s[1] == 'a') strcat(ss, "januari ");
      else if (!strncmp(s, "jun", 3)) strcat(ss, "juni ");
      else strcat(ss, "juli"); break;
    case 'f': strcat(ss, "februari"); break;
    case 'm': if (!strncmp(s, "mar", 3)) strcat(ss, "mars");
      else strcat(ss, "maj"); break;
    case 'a': if (s[1] == 'p') strcat(ss, "april");
      else strcat(ss, "augusti"); break;
    case 's': strcat(ss, "september"); break;
    case 'o': strcat(ss, "oktober"); break;
    case 'n': strcat(ss, "november"); break;
    default: strcat(ss, "december");
    }
    while(*s && !IsDigit(*s)) s++;
    if (IsDigit(*s)) {
      strcat(ss, " ");
      while(*ss) ss++;
      for (; *s; s++)
	if (IsDigit(*s))
	  *ss++ = *s;
      *ss = '\0';
    }
    return string;
  }

  case TOKEN_BAD_CARDINAL: {
    int a=0; const char *p = NULL, *t;
    for (t = s; *t; t++)
      if (IsDigit(*t)) {
	if (!p) {
	  if (*t != '0' || a) a++;
	}
      } else if (IsPunct(*t))
	if (!p) p = t;
    char *ss = string;
    if (*s == '-') {
      *ss++ = '-';
      s++;
    }
    if (a == 0) {
      *ss++ = '0';
      if (p) *ss++ = ',';
      if (p)  // buggfix 001125 johan
	for (t=p; *t; t++)
	  if (IsDigit(*t)) *ss++ = *t;
      *ss = '\0';
      return string;
    }
    bool b = true;
    for (;*s; s++)
      if (IsDigit(*s)) {
	if (a) {
	  if (b) b = false;
	  else if (a%3 == 0) *ss++ = ' ';
	  *ss++ = *s;
	  a--;
	} else *ss++ = *s; 
      } else if (IsPunct(*s) && p) {
	*ss++ = ',';
	p = NULL;
      }
    *ss = '\0';
    char *k;
    if ((k = strchr(string, ',')) != NULL && IsDigit(k[1]) &&
	IsDigit(k[2]) && IsDigit(k[3])) {
      strcpy(ss+1, string);
      *ss = '\t';
      *k = ' ';
    }
    return string;
  }
  default: {
    char *t = (char*) StavaCorrectWord((unsigned char *) s);
    if (t) {
      strcpy(string, t);
      free(t);
      if (*string) return string;
    }
    char *ss = string;
    for (const char *tt=s; *tt; tt++) {
      *ss++ = *tt;
      if (IsDigit(*tt) && IsLetter(tt[1]))
	*ss++ = '-';
    }
    *ss = '\0';
    if (FindMainWord(string))
      return string;
    ss = string;
    for (const char *tt=s; *tt; tt++) {
      *ss++ = *tt;
      if (IsDigit(*tt) && IsLetter(tt[1]))
	*ss++ = ' ';
    }
    *ss = '\0';
    if(strcmp(s, string) == 0) {
      // if string == s, return something else or we get "false alarm" // jonas
      *ss++ = ' '; // this should lead to a NON_IMPROVING_SUGGESTION, which is fine
      *ss++ = ' '; // but it is still an ugly hack
      *ss++ = ' '; 
      *ss = '\0';
    }
    return string;
  }
  }
}

void Scrutinizer::Analyze() {
  for (int i=0; i<ruleSet->NRules(); i++)
    {
      Rule *r = ruleSet->GetRule(i);
      if (!r->IsHelpRule() && strcmp(r->CategoryName(), "analyze"))
	ruleSet->InActivate(r);
    }
  xPrintMatchings = true;
  int n;
  Scrutinize(&n);
  PrintResult();
}
