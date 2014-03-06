/* developers-tagger.cc
 * author: Johan Carlberger
 * last change: 990909
 * comments: DevelopersTagger class
 */

#include <math.h>
#include <time.h>
#include "developers-tagger.h"
#include "sentence.h"
#include "morf.h"
//#include "newword.h"

#include "settings_manip.h"

bool xSetWordProbs = false;
bool xSetMorfProbs = false;

void DevelopersTagger::TestInflections() {
  Message(MSG_STATUS, "testing inflections...");
  Words().TestInflections();
  Message(MSG_COUNTS, "during inflection testing");
}

void DevelopersTagger::GenerateExtraWordTags(bool selectUnknownLemmas) {
  Message(MSG_STATUS, "generating extra word tags...");
  Words().GenerateInflections(true);
  Words().GenerateExtraWordTags();
  SelectMoreWordForms(selectUnknownLemmas);
  Message(MSG_COUNTS, "during extra word tags generating");
}

void DevelopersTagger::SelectMoreWordForms(bool selectUnknownLemmas) {
  Message(MSG_STATUS, "selecting more word forms...");
  char word[MAX_WORD_LENGTH];
  char base[MAX_WORD_LENGTH];
  Tag *tag = new Tag();
  Tag *jj = Tags().FindTag("jj.pos.utr.sin.ind.nom");
  Tag *nnutr = Tags().FindTag("nn.utr.sin.ind.nom");
  Tag *nnneu = Tags().FindTag("nn.neu.sin.ind.nom");
  Tag *vb = Tags().FindTag("vb.inf.akt");
  //  NewWordLexicon news;
  while (std::cin >> word >> tag->string >> base) {
    const Word *w = Words().Find(word);
    const Tag *t = Tags().Find(*tag);
    ensure(t);
    //const Word *b = Words().Find(base);
    const WordTag *wt = NULL;
    if (w) {
      wt = w->GetWordTag(t);
      if (!wt)
	for (const WordTag *wt2 = w; wt2; wt2 = wt2->Next())
	  if (wt2->GetTag()->OriginalTag() == t) {
	    wt = wt2;
	    break;
	  }
    }
    if (w && !wt)
      std::cout << word << tab << tag << tab << base << std::endl;
    else if (selectUnknownLemmas)
      if (!w) {
	if (t == jj || t == nnutr || t == nnneu || t == vb)
	  // { 
	  std::cout << word << tab << tag << tab << base << std::endl;
	//	  if (!news.Find(word))
	//	    news.AddWord(word);
	//	} else if (news.Find(word))
	//	  std::cout << word << tab << tag << tab << base << std::endl;
      }
  }
  Message(MSG_COUNTS, "during selecting");
}

void DevelopersTagger::GuessRulesForWords() {
  Message(MSG_STATUS, "guessing rules for words...");
  xCompoundRequirePrefix = false;
  // guess rules for words of increasing length to make sure rule for compound-suffixes
  // is selected before the are used by even longer compounds.
  for (uint len = 1; len<MAX_WORD_LENGTH; len++)
    for (int i=0; i<Words().Cw(); i++) {
      Word* w = &Words()[i];
      if (w->StringLen() == len) {
	for (WordTag *wt = w; wt; wt = wt->Next()) {
	  const Tag *t = wt->GetTag();
	  if (t->IsRuleBase() && wt->NInflectRules() == 0 &&
	      (t->IsVerb() || t->IsAdjective() || t->IsNoun())) {
	    NewWord nw(w->String());
	    nw.tagIndex = t->Index();
	    Words().CompoundAnalyze(&nw);
	    Words().GuessWordTagRule(&nw);
	    if (nw.NInflectRules() > 0) {
	      std::cout << nw << tab << Words().Inflects().Rule(nw.InflectRule(0)).Name() << std::endl;
	      wt->inflectRule = nw.InflectRule(0);
	    }
	  }
	}
      }
    }
  Message(MSG_COUNTS, "during guessing");
}

void DevelopersTagger::PrintStatistics() {
  std::cout<<"words: ";
  Words().PrintStatistics();
  std::cout<<"morfs: ";
  Morfs().PrintStatistics();
  std::cout<<"tags: ";
  Tags().PrintStatistics();
}  

#define rfp(a) std::cout<<tab<<#a<<": "<<a<<std::endl;

void DevelopersTagger::PrintParameters() const {
  if (!xPrintParameters || xRandomize)
    return;
  Message(MSG_STATUS);
  std::cout<<"tagging parameters:"<<std::endl;
  rfp(xTaggingEquation);
  if (xNewParameter > 0)
    rfp(xNewParameter);
  rfp(xLambda19);
  rfp(xLambdaUni);
  rfp(xLambdaBi);
  if (xTagTrigramsUsed) {
    rfp(xLambdaTri);
    rfp(xLambdaTriExp);
  }
  rfp(xEpsilonTri);
  rfp(xLambdaExtra);
  rfp(xAlphaExtra);
  rfp(xEpsilonExtra);
  rfp(xNWordVersions);
  rfp(xNNewWordVersions);
  std::cout<<tab<<"xAlphaLastChars: ";
  for (int i=xMinLastChars; i<=xMaxLastChars; i++)
    std::cout<<xAlphaLastChar[i]<<"  ";
  std::cout<<std::endl;
  if (xMorfCommonSuffix)
    rfp(xAlphaSuffix);
  rfp(xAlphaMember);
  rfp(xAlphaCapital);
  rfp(xAlphaNonCapital);
  rfp(xAlphaUnknownCapital);
  rfp(xAlphaUnknownNonCapital);
  std::cout<<tab<<noOrNuff(xMorfCapital)<<"capital sensitivity used"<<std::endl;
  std::cout<<tab<<noOrNuff(xMorfNonCapital)<<"non-capital sensitivity used"<<std::endl;
  std::cout<<tab<<"last "<<xMinLastChars<<'-'<<xMaxLastChars<<" character"
	   <<optS(xMaxLastChars>1)<<" checked"<<std::endl;
  std::cout<<tab<<noOrNuff(xMorfCommonSuffix)<<"common suffix check used"<<std::endl;
  std::cout<<tab<<noOrNuff(xAmbiguousNewWords)<<"new words tagged ambiguously"<<std::endl;
}

int DevelopersTagger::CountCorrectTaggings() const {
  int OK = 0;
  for (const Sentence *s=GetText().FirstSentence(); s; s=s->Next())
    for (int i=2; i<s->NWords()+2; i++) {
      const Tag *correctTag = GetCorrectTag(s->GetWordToken(i)); // jonas,check for "missing" tags should not be necessary?
      if (correctTag->IsCorrectGuess(s->GetWordToken(i)->SelectedTag()))
	OK++;
    }
  return OK;
}

void DevelopersTagger::ResetWords() {
  if (xOptimize) {
    for (Sentence *s=GetText().FirstSentence(); s; s=s->Next())
      for (int i=2; i<s->NWords()+2; i++) {
	s->GetWordToken(i)->SetSelectedTag(NULL);
	Word *w = s->GetWord(i);
	if (!w->IsNewWord()) {
	  if (w->HasExtraWordTag())
	    TagUnknownWord(w, true, false);
	  w->ComputeLexProbs();
	}
      }
  } else
    Message(MSG_STATUS, "reseting words...");
  //jonas  NewWords().Reset(); // this frees memory that should not be freed. Is this necessary?
}

void DevelopersTagger::SetProbsAndTagText() {
  static float oldLambda19 = -1;
  static float oldLambdaExtra = -1;
  static float oldAlphaExtra = -1;
  static float oldEpsilonExtra = -1;
  Tags().ComputeProbs();
  if ((xTaggingEquation == 21 && xLambda19 != oldLambda19) ||
      xLambdaExtra != oldLambdaExtra ||
      xAlphaExtra != oldAlphaExtra ||
      xEpsilonExtra != oldEpsilonExtra) {
    oldLambda19 = xLambda19;
    oldLambdaExtra = xLambdaExtra;
    oldAlphaExtra = xAlphaExtra;
    oldEpsilonExtra = xEpsilonExtra;
    xComputeLexProbs = true;
  } else
    xComputeLexProbs = false;
  SetMorfProbs();
  ResetWords();
  TagText();
}

int DevelopersTagger::TagAndCount(float *p, float x) {
  *p = x;
  std::cout.precision(5);
  if (xVerbose)
    std::cout<<*p<<tab;
  SetProbsAndTagText();
  int result =  CountCorrectTaggings();
  if (xVerbose)
    std::cout<<result<<std::endl;
  return result;
}

int DevelopersTagger::TagAndCount(int *p, int x) {
  *p = x;
  std::cout.precision(5);
  if (xVerbose)
    std::cout<<*p<<tab;
  SetProbsAndTagText();
  int result =  CountCorrectTaggings();
  if (xVerbose)
    std::cout<<result<<std::endl;
  return result;
}

void DevelopersTagger::OptimizeParameterLinear(float *p, float min, float max) {
  int maxSaved = 0;
  float oldP = *p;
  float best = *p;
  ensure (xScope > 1);
  float scope = xScope;
  if (min == -1)
    min = *p / scope;
  if (max == -1)
    max = *p * scope;
  if (min <= 0.0)
    min = (float) 0.0001;
  for (bool redo=true; redo;) {
    redo = false;
    float inc = (oldP - min) / (nLinearOptSteps/2.0-0.4);
    for (float a = min; a<=max; a += inc) {
      int correct = TagAndCount(p, a);
      if (correct > maxSaved) {
	maxSaved = correct;
	best = *p;
	if (correct > bestResult) {
	  bestResult = correct;
	  improvement = true;
	  redo = true;
	  oldP = *p;
	}
      }
      if (a > oldP)
	inc = (max-oldP) / (nLinearOptSteps/2.0-0.5);
    }
    if (xVerbose && redo) {
      std::cout<<"narrowing..."<<std::endl;
    } // jonas
    if(redo) { // jonas
      scope = 1 + (scope-1)/3;
      min = best / scope;
      max = best * scope;
    }
  }
  *p = oldP;
}

void DevelopersTagger::OptimizeParameterGolden(float *p, float a, float b) {
  float oldP = *p;
  if (*p == 0)
    *p = 1;
  if (a == -1)
    a = *p*0.01;
  if (b == -1)
    b = *p*1.3;
  const float r = (pow(5, 0.5)-1)/2;
  const float q = 1 - r;
  float dx = b-a;
  float x1 = a + q*dx;
  float x2 = a + r*dx;
  int F1 = TagAndCount(p, x1);
  int F2 = TagAndCount(p, x2);
  while ((b-a)/a > 0.003)
    if (F1 > F2) {
      dx = x2-a;
      b = x2;
      x2 = x1;
      F2 = F1;
      x1 = a + q*dx;
      F1 = TagAndCount(p, x1);
    } else {
      dx = b - x1;
      a = x1;
      x1 = x2;
      F1 = F2;
      x2 = a+r*dx;
      F2 = TagAndCount(p, x2);
    }
  if (F1 > bestResult) {
    std::cerr<<"improvement "<<x1<<' '<<F1<<std::endl;
    bestResult = F1;
    *p = x1;
    improvement = true;
  } else
    *p = oldP;
}

void DevelopersTagger::OptimizeParameter(char *name, float *p, float a, float b) {
  Message(MSG_STATUS);
  float r = 1/xScope;
  if (r > 1)
    r = 0.8;
  if (xRandomize) {
    std::cerr<<name<<' '<<*p;
    *p *= r + float(rand())/RAND_MAX*2*(1-r);
    std::cerr<<" -> "<<*p<<std::endl;
    return;
  }
  std::cout<<name<<" ("<< *p <<") "<<bestResult<<"..."<<std::endl;
  if (xGoldenRatio) 
    OptimizeParameterGolden(p, a, b);
  else
    OptimizeParameterLinear(p, a, b);
}

void DevelopersTagger::OptimizeParameter(char *name, bool *p) {
  OptimizeParameter(name, (int*)p, 0, 1);
}

// jonas
void DevelopersTagger::OptimizeParameter(char *name, int *p) {
  OptimizeParameter(name, p, 0, 1);
}

void DevelopersTagger::OptimizeParameter(char *name, int *p, int a, int b) {
  if (xRandomize)
    return;
  std::cout<<"optimizing "<<name<<" ("<<*p<<") "<<bestResult<<"..."<<std::endl;
  int bestP = *p;
  for (int i=a; i<=b; i++) {
    int n = TagAndCount(p, i);
    if (n > bestResult) {
      bestP = i;
      bestResult = n;
    }
  }
  *p = bestP;
}

#define ref(a) #a, &a

// jonas
#define LOOPS_UNTIL_PRINT 10

void DevelopersTagger::Optimize(char *opt) {
  char opt2[] = "123bzgeflmpcavsu";
  if (!strcmp(opt, "") || opt[0] == '^') {
    const char *nots = NULL;
    if (*opt == '^')
      nots = opt+1;
    if (xOptimizeImportantParameters)
      strcpy(opt2, "123ezpaflsv");
    if (nots)
      for (; *nots; nots++)
	for (char *p=opt2; *p; p++)
	  if (*p == *nots)
	    *p = '-';
    opt = opt2;
  }
  xCountPunctuationMarksAsWords = true;
  xOptimize = false;
  ReadTaggedText();
  SetProbsAndTagText();
  int bestResultEver = bestResult = EvaluateTagging();
  xOptimize = true;
  bool randomize2 = false;
  Message(MSG_STATUS, "optimizing...");
  srand(clock());
  nLinearOptSteps = 15;
  float xScopeStart = xScope;
  do {
    improvement = false;
    for (int k=0; opt[k]; k++) {
      switch(opt[k]) {
      case '1':
	OptimizeParameter(ref(xLambdaUni));
	break;
      case '2':
	OptimizeParameter(ref(xLambdaBi));
	break;
      case '3':
	if (xTagTrigramsUsed) {
	  OptimizeParameter(ref(xLambdaTri));
	  OptimizeParameter(ref(xLambdaTriExp));
	}
	break;
      case 'a':
	OptimizeParameter(ref(xAmbiguousNewWords));
	break;
      case 'c':
	OptimizeParameter(ref(xMorfCapital));
	OptimizeParameter(ref(xMorfNonCapital));
	break;
      case 'e':
	//	OptimizeParameter(ref(xEpsilonExtra));
	OptimizeParameter(ref(xLambdaExtra));
	OptimizeParameter(ref(xAlphaExtra));
	break;
      case 'f':
	OptimizeParameter(ref(xEpsilonTri));
	break;
      case 'm':
	OptimizeParameter(ref(xMaxLastChars), MIN_LAST_CHARS, MAX_LAST_CHARS);
	OptimizeParameter(ref(xMinLastChars), MIN_LAST_CHARS, xMaxLastChars);
	break;
      case 'l':
	{
	  char name[] = "xAlphaLastChar[0]";
	  for (int i=xMaxLastChars; i>=xMinLastChars; i--) {
	    name[15] = '0' + i;
	    OptimizeParameter(name, &(xAlphaLastChar[i]));
	  }
	  break;
	}
      case 'p':
	OptimizeParameter(ref(xAlphaCapital));
	OptimizeParameter(ref(xAlphaNonCapital));
	OptimizeParameter(ref(xAlphaUnknownCapital));
	OptimizeParameter(ref(xAlphaUnknownNonCapital));
	break;
      case 's':
	//	OptimizeParameter(ref(xMorfCommonSuffix));
	if (xMorfCommonSuffix && !xAnalyzeNewWords)
	  OptimizeParameter(ref(xAlphaSuffix));
	
	for (xCompoundMinLength=2; xCompoundMinLength<8; xCompoundMinLength++) {
	  for (xCompoundPrefixMinLength=1;
	       xCompoundPrefixMinLength<xCompoundMinLength; xCompoundPrefixMinLength++) {
	    xCompoundSuffixMinLength = xCompoundMinLength - xCompoundPrefixMinLength;
	    SetProbsAndTagText();
	    int result = CountCorrectTaggings();
	    std::cout << xCompoundMinLength << ' ' << xCompoundPrefixMinLength << ' ' << xCompoundSuffixMinLength << ": " << result << std::endl;
	    if (result > bestResult)
	      std::cout << "WOW!" << std::endl;
	  }
	}
	xCompoundMinLength = 4;
	xCompoundPrefixMinLength = 1;
	xCompoundSuffixMinLength = 3;
	
	break;
      case 'u':
	// OptimizeParameter(ref(xAlphaMember));
	break;
      case 'v':
	OptimizeParameter(ref(xNWordVersions), 1, MAX_WORD_VERSIONS);
	OptimizeParameter(ref(xNNewWordVersions), 1, MAX_WORD_VERSIONS);
	break;
      case 'x':
	if (xNewParameter != 0.0f)
	  OptimizeParameter(ref(xNewParameter));
	if (xNewParameter2 != 0.0f)
	  OptimizeParameter(ref(xNewParameter2));
	break;
      case 'z':
	if (xTaggingEquation == 21)
	  OptimizeParameter(ref(xLambda19));
	break;
      case '-':
	break;
      default:
	std::cerr<<"unknown opt: "<<opt[k]<<std::endl;
      }
    }
    if (xRandomize) {
      xRandomize = false;
      randomize2 = true;
      SetProbsAndTagText();
      bestResult = EvaluateTagging();
    } else {
      if (improvement)
	xScope = 1 + ((xScope-1)/2);
      else if (!xRandomize) {
	SetProbsAndTagText();
	if (!randomize2)
	  xOptimize = false;
	int result = EvaluateTagging();
	if (result != bestResult) {
	  Message(MSG_WARNING, "something is wrong", int2str(result));
	  Message(MSG_CONTINUE, "is not", int2str(bestResult));
	} else if (bestResult > bestResultEver) {
	  WriteSettings(LexiconDir(), "settings", bestResult);
	  bestResultEver = bestResult;
	}
	xOptimize = true;
	if (randomize2) {
	  ReadSettings(LexiconDir(), "settings");
	  xRandomize = true;
	  xScope = xScopeStart;
	}
      }
    }
    if (randomize2) {
      uint optLength = strlen(opt);
      for (int j=0; opt[j]; j++) {
	int m = rand() % optLength;
	Swap(opt[j], opt[m]);
      }
    }
  } while (improvement || randomize2);
  PrintParameters();
}

bool DevelopersTagger::LexiconCheck() {
  std::ifstream in;
  int crap;
  char string[MAX_WORD_LENGTH];
  FixIfstream(in, LexiconDir(), "words/cw");
  int n=0;
  for (int i=0; i<Words().Cw(); i++) {
    in >> crap >> string;
    if (!Words().Find(string)) {
      std::cout << string << " not found " << std::endl;
      n++;
    }
  }
  if (n)
    std::cerr<<"ERROR, "<<n<<" out of "<<Words().Cw()
	     <<" words not found"<<std::endl;
  else
    std::cerr<<"all words retrieved in hash table"<<std::endl;
  return !n;
}

void DevelopersTagger::ExtractTesaurusWordTags(const char *file) {
  Message(MSG_STATUS, "extracting tesaurus words...");
  xCompoundRequirePrefix = false;
  std::ifstream in;
  FixIfstream(in, file);
  char string[1000];
  while(in.getline(string, 1000)) {
    char *s = strchr(string+1, '\t');
    if (s)
      s++;
    else
      s = string;
    if (!*s || strchr(s, ' '))
      continue;
    Word *w = FindMainOrNewWordAndAddIfNotPresent(s);
    if (w->IsNewWord()) {
      Words().AnalyzeNewWord((NewWord*)w, true);
      //for (int i=0; i<w->NLemmas(); i++)
      //	for (int j=0; j<w->Lemma(i)->NInflectRules(); j++)
      //	  std::cout << w->Lemma(i)->String() << ' ' << Words().Inflects().Rule(w->Lemma(i)->InflectRule(j)).Name() << std::endl;
    }
    for (const WordTag *wt=w; wt; wt=wt->Next()) {
      for (int k=0; k<wt->NLemmas(); k++)
	for (int j=0; j<wt->Lemma(k)->NInflectRules(); j++)
	  for (int i=0; i<Tags().Ct(); i++)
	    if (Tags()[i].IsContent() || Tags()[i].OriginalTag()->IsContent()) {
	      WordTag *wt2 = wt->GetForm(&Tags()[i], k, j);
	      if (wt2)
		std::cout << wt2->String() << tab << wt2->GetTag() << tab << wt->Lemma(k)->String() << std::endl;
	    }
    }
  }
  Message(MSG_STATUS, "tesaurus words extracted");
}

void DevelopersTagger::LoadTesaurus(const char *wtlFile, const char *tesFile) {
  Message(MSG_STATUS, "loading tesaurus words...");
  nTesaurusWords = 26431;
  tesaurusWords.Init("tesaurusWords", nTesaurusWords, 
		     CompareTesaurusWords, KeyTesaurusWord,
		     NULL, NULL, NULL); // jonas, NULL NULL NULL OK?
  std::ifstream in;
  FixIfstream(in, wtlFile);
  char wordString[MAX_WORD_LENGTH], lemmaString[MAX_WORD_LENGTH];
  Tag tag;
  int n = 0;
  while(in >> wordString >> tag.string >> lemmaString) {
    Word *w = FindMainWord(wordString);
    if (!w)
      Message(MSG_ERROR, wordString, "in", wtlFile, "is an unknown word");
    w->tesaurus = 1;
    const Tag *t = Tags().Find(tag);
    if (!t)
      Message(MSG_ERROR, tag.String(), "in", wtlFile, "is an unknown tag");
    WordTag *wt = w->GetWordTag(t);
    ensure(wt);
    tesaurusWords[n].wordTag = wt;
    n++;
  }
  ensure(n == nTesaurusWords);
  tesaurusWords.Hashify();
  Message(MSG_STATUS, "tesaurus wtl-words done");

  FixIfstream(in, tesFile);
  char string[1000];
  TesaurusWord *headTW = NULL;
  while(in.getline(string, 1000)) {
    bool head = false;
    bool use = false;
    char *s = strchr(string+1, '\t');
    if (s) {
      s++;
      if (!strncmp(string+1, "USE", 3))
	use = true;
      else if (!strncmp(string+1, "UF", 2))
	;
    } else {
      head = true;
      s = string;
    }
    if (!*s || strchr(s, ' ')) {
      if (head)
	headTW = NULL;
      continue;
    }
    //    std::cout << '[' << s << ']' << std::endl;
    Word *w = FindMainWord(s);
    if (!w) {
      Message(MSG_MINOR_WARNING, "no word", s);
      if (head)
	headTW = NULL;
      continue;
    }
    TesaurusWord *tw = tesaurusWords.Find(TesaurusWord(w));
    if (head)
      headTW = tw;
    if (!tw) {
      Message(MSG_MINOR_WARNING, "no tw for word", w->String());
      continue;
    }
    if (use && headTW) {
      headTW->use = tw;
      //      std::cout << headTW << " use " << tw << std::endl;
    }
    
    for (const WordTag *wt=w; wt; wt=wt->Next()) {
      for (int k=0; k<wt->NLemmas(); k++)
	for (int j=0; j<wt->Lemma(k)->NInflectRules(); j++)
	  for (int i=0; i<Tags().Ct(); i++)
	    if (Tags()[i].IsContent() || Tags()[i].OriginalTag()->IsContent()) {
	      WordTag *wt2 = wt->GetForm(&Tags()[i], k, j);
	      if (wt2) {
		TesaurusWord *tw2 = tesaurusWords.Find(TesaurusWord(wt2));
		if (!tw2)
		  Message(MSG_MINOR_WARNING, "no tw for word-tag", wt2->String());
		else
		  tw2->wantedForm = tw;
		//		std::cout << w << ' ' << wt2 << std::endl;
	      }
	    }
    }
  }
}

#include "heap.h"
typedef const TesaurusWord* TWPointer;
int totTwDoc;
const float TesaurusWordFreq(TWPointer tw) {
  return 100.0 * (float(tw->docFreq) / totTwDoc +
		  float(tw->docFreq) / tw->freq +
		  float(tw->docFreq) / tw->nDocs);
}

int TesaurusWord::totalNDocs;

void TesaurusWord::Print(std::ostream &os) const {
  os << TesaurusWordFreq(this) 
     << tab << 100.0 * docFreq / freq
     << tab << 100.0 * nDocs / totalNDocs
     << tab << wordTag->String();
}

void DevelopersTagger::ExtractIndexWords(char **files, int nFiles) {
  Message(MSG_STATUS, "extracting index words from", int2str(nFiles), "files...");
  std::ifstream in;
  Token token;
  
  TesaurusWord::totalNDocs = nFiles;
  std::cout.setf(std::ios::fixed);
  std::cout.precision(0);
  for (int n = 0; n<nFiles; n++) {
    //    Message(MSG_STATUS, "counting tesaurus-words in", files[n], "...");
    FixIfstream(in, files[n]);
    SetStream(&in); // jonas SetStream(&in, false);
    while((token = tokenizer.Parse()) != TOKEN_END) { 
      const char *string = tokenizer.TokenString();
      if (token == TOKEN_WORD || token == TOKEN_SIMPLE_WORD) {
	Word *w = FindMainWord(string);
	//  for (const Sentence *s = GetText().FirstSentence(); s; s = s->Next())
	//  for (int i=2; i<s->NWords()+2; i++)
	//      if (s->GetWord(i)->IsTesaurus()) {
	//	TesaurusWord *tw = tesaurusWords.Find(s->GetWord(i));
	if (w && w->IsTesaurus()) {
	  TesaurusWord *tw = tesaurusWords.Find(w);
	  if (!tw) {
	    Message(MSG_MINOR_WARNING, "no tesaurus-word for text-word", w->String());
	    continue;
	  }
	  TesaurusWord *index = tw->IndexWord();
	  ensure(index);
	  index->freq++;
	  if (index->crap != n) {
	    index->nDocs++;
	    index->crap = n;
	  }
	}
      }
    }
  }
  
  TesaurusWord *possible[1000], *best[10], **selected;

  for (int n = 0; n<nFiles; n++) {
    std::cout << std::endl << "file " << n+1 <<" of " << nFiles << ": " << files[n] << std::endl;

    //    Message(MSG_STATUS, "extracting index words from", files[n]);
    int nPoss = 0;
    totTwDoc = 0;
    FixIfstream(in, files[n]);
    SetStream(&in);    //jonas    SetStream(&in, false);
    while((token = tokenizer.Parse()) != TOKEN_END) {
      const char *string = tokenizer.TokenString();
      if (token == TOKEN_WORD || token == TOKEN_SIMPLE_WORD) {
	Word *w = FindMainWord(string);
	if (w && w->IsTesaurus()) {
	  TesaurusWord *tw = tesaurusWords.Find(w);
	  if (!tw) {
	    Message(MSG_MINOR_WARNING, "no tesaurus-word for text-word", w->String());
	    continue;
	  }
	  TesaurusWord *index = tw->IndexWord();
	  ensure(index);
	  if (index->docFreq == 0) 
	    possible[nPoss++] = index;
	  index->docFreq++;
	  totTwDoc++;
	}
      }
    }
    int nFound = 0;
    if (nPoss <= 5) {
      selected = possible;
      nFound = nPoss;
    } else {
      SelectBest((TWPointer*) possible, nPoss, (TWPointer*) best, 5, TesaurusWordFreq);
      selected = best;
      nFound = 5;
    }
    for (int i=0; i<nFound; i++)
      for (int j=i+1; j<nFound; j++)
	if (TesaurusWordFreq(selected[i]) < TesaurusWordFreq(selected[j]))
	  Swap(selected[i], selected[j]);
    for (int i=0; i<nFound; i++) {
      std::cout << ' ' << selected[i] << std::endl;
    }
    for (int i=0; i<nPoss; i++) {
      possible[i]->docFreq = 0;
    }
  }
}


/* jonas: new stab at doing a ReadTaggedText() */
static const int TOKEN_BUF_CHUNK = 16000; // jonas, ugly
inline int min(int a, int b) {
  return (a < b) ? a : b;
}
#include <sstream>
void DevelopersTagger::ReadTaggedText() {
  Message(MSG_STATUS, "reading tagged text...");
  Reset();
  Timer timer;
  if (xTakeTime) 
    timer.Start();

  if (tokensBufSize == 0) {
    if(input_size > 0) { // jonas, this part is new
      tokensBufSize = input_size / 16; // guess words approx 4 chars, plus tags approx 9, + some whitespace, not so good for other tag-sets, like bnc
      std::cerr << "allocated space for " << tokensBufSize << " tokens " << std::endl;
      theTokens = new WordToken[tokensBufSize];
    } else { // jonas, this is the old stuff
      theTokens = new WordToken[TOKEN_BUF_CHUNK]; // new OK
      tokensBufSize = TOKEN_BUF_CHUNK;
    }
    if(correctTagsSize <= 0) {
      correctTags = new Tag*[tokensBufSize];
      correctTagsSize = tokensBufSize;
    }
  }
  theTokens[0].SetWord(specialWord[TOKEN_DELIMITER_PERIOD], "$.", TOKEN_END);
  nTokens = 1;
  std::string temp_string;  //jonas  char string[MAX_WORD_LENGTH];

  Tag tag;
  while(*inputStream) {
    /* read word + tag, word might contain whitespace, tag should not */
    std::string line;
    std::getline(*inputStream, line);
    if(line.size() <= 0)
      continue;
    int last_not_space = line.find_last_not_of(" \t");
    int last_space = line.find_last_of(" \t", last_not_space);
    //    if(last_space > static_cast<int>(MAX_WORD_LENGTH))
    //  std::cerr << "on line "<< nTokens << " word is too long ("<<last_not_space<<") " << line <<std::endl;
    if(last_not_space - last_space > MAX_TAG_STRING)
      std::cerr << "on line "<< nTokens << " tag is too long ("<<last_space<<", " << last_not_space << " ) " << line <<std::endl;
    //jonas   strncpy(string, line.c_str(), last_space);    
    //jonas   string[last_space] = 0;
    temp_string = line.substr(0,last_space);
    strcpy(tag.string, line.substr(last_space+1, min(MAX_TAG_STRING,last_not_space-last_space)).c_str());

    if (nTokens >= tokensBufSize) {
      tokensBufSize *= 2;      //jonas      tokensBufSize += TOKEN_BUF_CHUNK;
      WordToken *tok = new WordToken[tokensBufSize]; // new OK
      memcpy(tok, theTokens, nTokens * sizeof(WordToken));
      delete [] theTokens;	// jbfix: delete p --> delete [] p
      theTokens = tok;
      std::cerr << "REallocated space for " << tokensBufSize << " tokens !!!!" << std::endl;
    }
    if (nTokens >= correctTagsSize) {
      Tag** temp = new Tag*[tokensBufSize];
      memcpy(temp, correctTags, nTokens *sizeof(Tag*));
      delete [] correctTags;
      correctTags = temp;
      correctTagsSize = tokensBufSize;
    }
    Tag *correctTag = Tags().Find(tag); 
    if (!correctTag) {
      //jonas      Message(MSG_MINOR_WARNING, "unknown tag for", string);
      Message(MSG_WARNING, "unknown tag for", temp_string.c_str(), tag.string);
      correctTag = Tags().DummyTag(); // jonas
      ensure(correctTag);
    }

    //jonas    Message(MSG_VERBOSE, string);
    Message(MSG_VERBOSE, temp_string.c_str());
    //jonas    if (xIgnoreCitation && !strcmp(string, "\"")) // this isn't done for untagged text...
    if (xIgnoreCitation && !strcmp(temp_string.c_str(), "\"")) // this isn't done for untagged text...
      continue;

    correctTags[nTokens] = correctTag;
    WordToken &t = theTokens[nTokens++];
    if(nTokens % 100000 == 1)
      std::cerr << nTokens << " tokens read" << std::endl;
    if (correctTag->IsSentenceDelimiter()) {
      t.SetSelectedTag(correctTag, false);
      //jonas      Word *w = Words().Find(string);
      Word *w = Words().Find(temp_string.c_str());
      ensure(w);
      //jonas      t.SetWord(w, string, TOKEN_PUNCTUATION);
      t.SetWord(w, temp_string.c_str(), TOKEN_PUNCTUATION);
    } else {
      //     std::istringstream jobbigt(string, strlen(string));
      //jonas      std::string temp_jobbigt = string;
      std::string temp_jobbigt = temp_string;
      std::istringstream jobbigt(temp_jobbigt);
      tokenizer.SetStream(&jobbigt);
      Token token = tokenizer.Parse();
      //jonas      Word *w = FindMainOrNewWordAndAddIfNotPresent(string);
      Word *w = FindMainOrNewWordAndAddIfNotPresent(temp_string.c_str());
      ensure(w);
      //jonas      t.SetWord(w, string, token);
      t.SetWord(w, temp_string.c_str(), token);
      if (w->IsNewWord()) {
	NewWord *nw = (NewWord*) w;
	if (nw->Freq() == 0)
	  nw->isAlwaysCapped = (t.IsFirstCapped() || t.IsAllCapped()); // jonas, correct ?
	else
	  nw->isAlwaysCapped &= (t.IsFirstCapped() || t.IsAllCapped()); // jonas, correct ?
	nw->freq++;	
	//jonas	nw->isForeign = IsForeign(*string);//jonas
	nw->isForeign = IsForeign(temp_string[0]);//jonas
      }
      w->textFreq++;
    }
  }
  theTokens[nTokens].SetWord(specialWord[TOKEN_DELIMITER_PERIOD], "$.", TOKEN_END);
  theTokens[nTokens+1].SetWord(specialWord[TOKEN_DELIMITER_PERIOD], "$.", TOKEN_END);
  //  for(int i = 0; i < nTokens; ++i)  std::cout<< theTokens[i] << std::endl;  // jonas, debug
  if (xTakeTime) 
    tokenizeTime = timer.Restart();
  BuildSentences(theTokens);
  if (xTakeTime) 
    sentenceTime = timer.Restart();
  Message(MSG_STATUS, "text read");
}

// jonas, copy pasted code from tagger.cpp
// (actually, move-pasted code)
int DevelopersTagger::EvaluateTagging() {
  Message(MSG_STATUS, "evaluating tagging...");
  static uchar **faults = NULL;
  if (xCountFaults) {
    if (!faults) {
      faults = new uchar*[Tags().Ct()]; // new OK ?jonas
      for (int i=0; i<Tags().Ct(); i++)
	faults[i] = new uchar[Tags().Ct()]; // new OK ?jonas
    }
    for (int i=0; i<Tags().Ct(); i++)
      for (int j=0; j<Tags().Ct(); j++)
	faults[i][j] = 0;
  }
  int sentencesOK = 0, OK = 0, knownOK = 0, punctsOK = 0, unknownOK = 0;
  int unknownTags = 0, hard = 0, extra = 0, extraWordTag = 0, extraOK = 0;
  int extraWordTagOK = 0, compoundsOK = 0, compounds = 0, wordClassesOK = 0;
  int puncts = 0; // jonas, count punctuations now, should be handled somewhere else?
  theText.CountContents();
  for (const Sentence *s=theText.FirstSentence(); s; s=s->Next()) {
    bool sentenceOK = true;
    for (int i=2; i<s->NWords()+2; i++) {
      const Word *w = s->GetWord(i);
      const NewWord *nw = w->IsNewWord() ? (NewWord*) w : NULL;
      const Tag *correct = GetCorrectTag(s->GetWordToken(i));
      const Tag *selected = s->GetWordToken(i)->SelectedTag();
      ensure(selected);
      const WordTag *wt = w->GetWordTag(selected);
      if (xCountPunctuationMarksAsWords || !correct->IsPunctuationOrEnder()) {
	if (correct->IsCorrectGuess(selected)) {
	  OK++;
	  wordClassesOK++;
	  if (w->IsNewWord()) {
	    unknownOK++;
	    if (nw->IsCompound())
	      compoundsOK++;
	  } else 
	    knownOK++;
	  if (correct->IsPunctuationOrEnder())
	    punctsOK++;
	  if (w->IsExtra())
	    extraOK++;
	  if (wt && wt->IsExtraWordTag())
	    extraWordTagOK++;
	} else { // faulty tagging
	  //	  std::cout << "fault:\t" << selected->string << "!="<<correct->string << ", " << wt->GetWord()<< std::endl; // jonas, debug
	  if (selected->WordClass() == correct->WordClass()) {
	    //	    std::cerr << selected << " != " << correct << ", but " << selected->WordClass() << " == " << correct->WordClass() << std::endl; // jonas, debug
	    wordClassesOK++;
	  }
	  if (xCountFaults)
	    faults[selected->Index()][correct->Index()]++;
	  sentenceOK = false;
	  if (correct == Tags().DummyTag())
	    unknownTags++;	  
	  else if (!w->IsNewWord() && !w->GetWordTag(correct))
	    hard++;
	}
	if(correct->IsPunctuationOrEnder())
	  puncts++;
	if (w->IsExtra())
	  extra++;
	if (wt && wt->IsExtraWordTag())
	  extraWordTag++;
	if (nw && nw->IsCompound())
	  compounds++;
      }
    }
    if (sentenceOK)
      sentencesOK++;
  }
  if (xOptimize || xRepeatTest) {
    return OK;
  }

  int n;
  if (xCountPunctuationMarksAsWords) {
    std::cout << "punctuation marks count as words"<<std::endl;
    n = theText.NWordTokens(); // jonas, this actually includes punctuation
  } else {
    std::cout << "punctuation marks does NOT count as words"<<std::endl;
    n = theText.NWordTokens() - puncts; // jonas
  }
  std::cout << "silly correct tags" << dont(!xAcceptAnyTagWhenCorrectIsSilly) 
	    << "count as correct guess" << std::endl;
  int errors = n - OK, unknown = theText.NNewWords();
  std::cout<<"tagging performance:"<< std::endl
	   << tab << theText << std::endl
	   << tab << float(n) / theText.NSentences()
	   <<" words per sentence"<< std::endl;
  if (xTaggedText) {
    if(n) {
      if(errors) {
	std::cout<<tab<<100.0*unknown/n<<" % unknown words ("<<unknown<<')'<<std::endl
		 <<tab<<100.0*hard/n<<" % words hard to tag ("<<hard<<')'<<std::endl
		 <<tab<<100.0*(unknown-unknownOK)/errors<<" % of errors were unknown words ("
		 <<unknown-unknownOK<<')'<<std::endl
		 <<tab<<100.0*(puncts-punctsOK)/errors<<" % of errors were punctuations ("
		 <<puncts-punctsOK<<')'<<std::endl;
	if (unknownTags)
	  std::cout<<tab<<100.0*unknownTags/errors
		   <<" % of errors were unknown tags! ("
		   <<unknownTags<<')'<<std::endl;
	std::cout<<tab<<100.0*hard/errors<<" % of errors were hard words ("<<hard<<')'<<std::endl
		 <<tab<<100.0*(n-unknown-hard-knownOK-(puncts-punctsOK)-unknownTags)/errors
		 <<" % of errors were non-chosen known word-tags ("
		 <<(n-unknown-hard-knownOK-(puncts-punctsOK)-unknownTags)<<')'<<std::endl;
      } else {
	std::cout<<tab<<100.0*unknown/n<<" % unknown words ("<<unknown<<')'<<std::endl
		 <<tab<<100.0*hard/n<<" % words hard to tag ("<<hard<<')'<<std::endl;
      }
    }
    if (extraWordTag && errors)
      std::cout<<tab<<100.0*(extraWordTag-extraWordTagOK)/errors
	       <<" % of errors were ill-chosen extra word-tags ("
	       <<extraWordTag-extraWordTagOK<<')'<<std::endl;
    if(theText.NSentences())
      std::cout<<tab<<100.0*sentencesOK/theText.NSentences()
	       <<" % correctly tagged sentences ("
	       <<sentencesOK<<')'<<std::endl;
    if(unknown) 
      std::cout	<<tab<<100.0*unknownOK/unknown
		<<" % correctly tagged unknown words"<<std::endl;
    if(compounds) 
      std::cout <<tab<<100.0*compoundsOK/compounds
		<<" % correctly tagged compound words"<<std::endl;
    if(n-unknown) 
      std::cout <<tab<<100.0*(knownOK)/(n-unknown)
		<<" % correctly tagged known words"<<std::endl;
    if(puncts) 
      std::cout <<tab<<100.0*punctsOK/puncts
		<<" % correctly tagged punctuation marks"<<std::endl;    
    if (extraWordTag)
      std::cout<<tab<<100.0*extraOK/extra
	       <<" % correctly tagged extra words"<<std::endl
	       <<tab<<100.0*extraWordTagOK/extraWordTag
	       <<" % correctly tagged extra word-tags"<<std::endl;
    std::cout<<tab<<100.0*wordClassesOK/n<<" % word class tagging accuracy"<<std::endl;
    std::cout<<tab<<100.0*OK/n<<" % correctly tagged words ("<<errors<<'/'<<OK<<'/'<<n<<')'<<std::endl;
  }
  if (xCountFaults)
    for (int i=0; i<Tags().Ct(); i++)
      for (int j=0; j<Tags().Ct(); j++)
	if (faults[i][j])
	  std::cout<<(uint)faults[i][j]<<tab<<Tags()[i]<<" correct: "<<Tags()[j]<<std::endl;
  return OK;
}

// jonas
void DevelopersTagger::DontTagText() {
  for (Sentence *s=theText.FirstSentence(); s; s = s->Next())
    for (int i = 2; i <= s->NTokens() - 3; i++) {
      WordToken *t = s->GetWordToken(i);
      t->SetSelectedTag(GetCorrectTag(s->GetWordToken(i)));
    }
}
