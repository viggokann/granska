/* main.cc
 * author: Viggo Kann (Johan Carlberger)
 * last Viggo change: 1999-02-15
 * last Johan change: 2002-02-26
 * last Johnny change: ja
 */

#include "scrutinizer.h"
#include "matchingset.h"
#include "ruleset.h"
	
bool xAnalyze = false;
bool xReadTaggedText = false; // jonas, intended for use only for evaluation study 030120 - 030228

// jonas, we do not want -w to take the rest of the command line
bool xDoWebScrut = false;
char **WebArgv = 0;


#ifdef PROBCHECK  // jb: remove these after batch test
#include "prob.h"
#include "output.h"
#include "config.h"

#endif // PROBCHECK

#ifdef TRANSITIVITY
#include "trans.h"
#endif // TRANSITIVITY


static void PrintMemoryUsage() {
#ifdef COUNT_OBJECTS
  PriObj(Scrutinizer);
  PriObj(RuleSet);
  PriObj(HashArray<Word>);
  PriObj(HashArray<StyleWord>);
  PriObj(HashArray<Tag>);
  PriObj(HashArray<TagTrigram>);
  PriObj(HashTable<NewWord>);
  PriObj(HashTable<WordRuleTerms>);
  PriObj(TagLexicon);
  PriObj(WordLexicon);
  PriObj(MorfLexicon);
  PriObj(NewWordLexicon);
  PriObj(StringBuf);
  PriObj(Text);
  PriObj(Sentence);
  PriObj(DynamicSentence);
  PriObj(WordToken);
  PriObj(Word);
  PriObj(WordTag);
  PriObj(StyleWord);
  PriObj(Tag);
  PriObj(TagTrigram);
  PriObj(ChangeableTag);
  PriObj(NewWord);
  PriObj(Matching);
  PriObj(ElementMatching);
  PriObj(GramError);
  PriObj(CorrThing);
  PriObj(RuleTermList);
  PriObj(IdEntry);
  PriObj(Element);
  PriObj(Expr);
  std::cout << "memory usage of counted objects = " <<
    (NBytes(Scrutinizer) + NBytes(RuleSet) + 
     NBytes(HashArray<Word>) + NBytes(HashArray<StyleWord>) +
     NBytes(HashArray<Tag>) +
     NBytes(HashArray<TagTrigram>) + NBytes(HashTable<NewWord>) + NBytes(HashTable<WordRuleTerms>) +
     NBytes(TagLexicon) + NBytes(WordLexicon) + NBytes(MorfLexicon) +
     NBytes(NewWordLexicon) +
     NBytes(StringBuf) + NBytes(Text) + NBytes(Sentence) +
     NBytes(DynamicSentence) +
     NBytes(WordToken) +
     NBytes(Word) + NBytes(WordTag) + + NBytes(StyleWord) + NBytes(Tag) + NBytes(TagTrigram) +
     NBytes(ChangeableTag) + NBytes(NewWord) +
     NBytes(Matching) + NBytes(ElementMatching) + NBytes(GramError) + NBytes(CorrThing) +
     NBytes(RuleTermList) + NBytes(IdEntry) + NBytes(Element) + NBytes(Expr)
     ) /
    (1024.0*1024.0) << " Mb" << std::endl;
#endif
}

static void RepeatTest(Scrutinizer *s, const char *ruleFile, const char *textFile) {
  //  s->Load(NULL, ruleFile);
  s->ReadTextFromFile(textFile);
  int n;
  s->Scrutinize(&n);
  PrintMemoryUsage();
  char *p1 = new char[1];
  char *p2 = new char[28987];
  static char *first = p2;
  std::cout << "last pointers = " << (void*)p1 << ' ' << (void*)p2 << std::endl;
  std::cout << ((p2>first) ? (p2-first) : (first-p2)) << " bytes appear wasted" << std::endl;
  delete p1; delete p2;
}

static void PrintUsage(char *progName) {
    // jbfix: getenv() returns 0 if env.var. unavailable
    // this caused operator<<(0) to abort
    char *stava_lex = getenv("STAVA_LEXICON");
    char *tagger_lex = getenv("TAGGER_LEXICON");
    char *scrut_rule = getenv("SCRUTINIZER_RULE_FILE");
    char *scrut_test = getenv("SCRUTINIZER_TEST_TEXT");
    if(!stava_lex) stava_lex = "<NULL>";
    if(!tagger_lex) tagger_lex = "<NULL>";
    if(!scrut_rule)	scrut_rule = "<NULL>";
    if(!scrut_test) scrut_test = "<NULL>";
    
    std::cerr
      << "usage:" << std::endl
      << progName << tab << "[-d|i|h|m|e|p|r|R|t|x] [-agjnopqsuvyW]" << std::endl
      << tab << tab << "[-r ruleFile] [textFile]" << std::endl
      << tab << tab << "OR [-w args]" << std::endl
      <<"mutually exclusive running modes:" << std::endl
      <<tab<<"d:"<<dont(xCheckAccept)<<"check accept and detect texts of rules"<<std::endl
      <<tab<<"e:"<<dont(xPrintGramErrors)<<"print gram-errors"<<std::endl
      <<tab<<"k:"<<dont(xPrintOptimization)<<"print optimization info"<<std::endl
      <<tab<<"z:"<<dont(xAnalyze)<<"analyze sentences"<<std::endl
      <<tab<<"h:"<<" print usage"<<std::endl
      <<tab<<"m:"<<dont(xPrintMatchings)<<"print matchings"<<std::endl
      <<tab<<"p:"<<dont(xPrintRulesOnly)<<"print rules"<<std::endl
      <<tab<<"R:"<<" print rules triggered during scrutinization"<<std::endl
      <<tab<<"q:"<<dont(xPrintRuleHeadersOnly)<<"print rule headers only"<<std::endl
      <<tab<<"w: run web mode"<<std::endl //johan020226 
#ifdef TIMER
      <<tab<<"t:"<<dont(xTakeTime)<<"take time"<<std::endl
#endif
      <<"in some modes effective options:" << std::endl
      <<tab<<"Q:"<<dont(xReadTaggedText)<<"read text that is already tagged"<<std::endl// jonas, intended for use only for evaluation study 030120 - 030228
      <<tab<<"a:"<<dont(xPrintSelectedTag)<<"print tags selected by tagger"<<std::endl
      <<tab<<"b:"<<dont(xAcceptNonImprovingCorrections)<<"accept non-improving corrections"<<std::endl
      <<tab<<"c:"<<dont(xAcceptAllWordsInCompounds)<<"accept all words in compounds"<<std::endl
      <<tab<<"F:"<<dont(xTryLatestFeature)<<"try latest new feature"<<std::endl
      <<tab<<"g:"<<dont(xSuggestionSameAsOriginalMeansFalseAlarm)<<"interpret non-changed suggestion as false alarm"<<std::endl
      <<tab<<"j:"<<dont(xPrintWordInfo)<<"print word info"<<std::endl
      <<tab<<"n:"<<dont(xPrintMatchingHelpRules)<<"print matching help rules"<<std::endl
      <<tab<<"N:"<<dont(xNewlineMeansNewSentence)<<"interpret newline as end of sentence"<<std::endl
      <<tab<<"f:"<<dont(xAcceptRepeatedSuggestions)<<"accept repeated suggestions"<<std::endl
      <<tab<<"u:"<<dont(xPrintMatchingAcceptingRules)<<"print matching accepting rules"<<std::endl
      <<tab<<"o:"<<dont(xOptimizeMatchings)<<"optimize rule matching"<<std::endl
      <<tab<<"s:"<<dont(xPrintAllSentences)<<"print all sentences"<<std::endl
      <<tab<<"v:"<<dont(xVerbose)<<"verbose"<<std::endl
      <<tab<<"W:"<<dont(xWarnAll)<<"warn for all suspicious things"<<std::endl
      <<tab<<"Z:"<<dont(xVerbose)<<"do repeat test"<<std::endl
      <<"The program looks for the Stava lexicon in the path given by the environment"<<std::endl
      <<"variable STAVA_LEXICON = "<<stava_lex<<". If this is null, "<<std::endl
      <<"the program uses the path \"lexicons/stava\" instead."<<std::endl
      <<"The program looks for the Tagger lexicon in the path given by the environment"<<std::endl
      <<"variable TAGGER_LEXICON = "<<tagger_lex<<". If this is null, "<<std::endl
      <<"the program uses the path \"lexicons\" instead."<<std::endl
      <<"If no rule file is given, the program uses the file given by"<<std::endl
      <<"the environment variable SCRUTINIZER_RULE_FILE = "<<scrut_rule<<std::endl
      <<"If no text file is given, the program uses the file given by"<<std::endl
      <<"the rule file name with extension '.testfil' if existing, else"<<std::endl
      <<"the environment variable SCRUTINIZER_TEST_TEXT = "<<scrut_test<<std::endl;
}

static int run(const char *ruleFile, const char *textFile) {
  Scrutinizer scrutinizer;

  scrutinizer.Load(NULL, ruleFile);

#ifdef PROBCHECK    // jb: added prob_check support 2001-04-24
  Prob::load(scrutinizer.Tags());
#endif // PROBCHECK

#ifdef TRANSITIVITY // jb: added transitivity support 2001-06-18
  Trans::load();
#endif

  if (xRepeatTest)
    for (int i=0; ;i++) {
      std::cout << std::endl << "test " << i << std::endl;
      RepeatTest(&scrutinizer, ruleFile, textFile); 
    }
  if (xPrintRulesOnly || xPrintRuleHeadersOnly) {
    std::cout << scrutinizer.GetRuleSet() << std::endl;
    return 0;
  }
  if (xCheckAccept) {
    scrutinizer.CheckAcceptAndDetect();
    return 0;
  }
  int n;
  if (xTryLatestFeature) {
    char *str1 = "En hus.";
    char *str2 = "Iledning.";
    scrutinizer.ReadTextFromString(str1);
    scrutinizer.Scrutinize(&n);
    scrutinizer.PrintResult();
    scrutinizer.ReadTextFromString(str2);
    scrutinizer.Scrutinize(&n);
    scrutinizer.PrintResult();
    scrutinizer.ReadTextFromString(str1);
    scrutinizer.Scrutinize(&n);
    scrutinizer.PrintResult();
    return 0;
  }
  scrutinizer.ReadTextFromFile(textFile);
  Message(MSG_COUNTS, "during text reading");
  if (xAnalyze) {
    scrutinizer.Analyze();
    return 0;
  }
  else
  {
#ifdef USE_AS_CLIENT
      std::cerr << "pipe name: " << config().pipe_name << std::endl;
      Prob::Client c(config().pipe_name, scrutinizer, config);
      c.run();
#else // USE_AS_SERVER
      scrutinizer.Scrutinize(&n);
#endif // USE_AS_SERVER
  }
  Message(MSG_COUNTS, "during scrutinizing");
  scrutinizer.PrintResult();
 
  return 0;
}

#ifdef WEB_SCRUTINIZER
extern int WebScrutinize(Scrutinizer*, int, char**); //johan020226

static int WebScrut(int argc, char **argv) {
  SetMessageStream(MSG_STATUS, NULL);
  Scrutinizer scrutinizer;
  return WebScrutinize(&scrutinizer, argc, argv);
}
#endif // WEB_SCRUTINIZER

int main(int argc, char **argv) {
#ifdef PROBCHECK
    using Prob::config;
#endif // PROBCHECK
  try {
      xPrintOneWordPerLine = false;
      xPrintSelectedTag = xPrintWordInfo = false;
      xPrintAllWords = true;
      char *ruleFile = NULL;
      char *textFile = NULL;
      int c;
  #ifdef DEVELOPER_OUTPUT
      for(c = 0; c < argc; c++)
	  std::cout << "argv[" << c << "]: " << argv[c] << std::endl;
  #endif // DEVELOPER_OUTPUT
      for(c = 1; c < argc; c++) {
	if (argv[c][0] == '-')
	  for (int i=1; argv[c][i]; i++)
	    switch(argv[c][i]) {
	    case 'a': neg(xPrintSelectedTag); break;
	    case 'b': neg(xAcceptNonImprovingCorrections); break;
	    case 'c': neg(xAcceptAllWordsInCompounds); break;
	    case 'd': neg(xCheckAccept); break;
	    case 'e': neg(xPrintGramErrors); break;
	    case 'f': neg(xAcceptRepeatedSuggestions); break;
	    case 'F': neg(xTryLatestFeature); break;
	    case 'g': neg(xSuggestionSameAsOriginalMeansFalseAlarm); break;
	    case 'j': neg(xPrintWordInfo); break;
	    case 'k': neg(xPrintOptimization); break;
	    case 'm': neg(xPrintMatchings); break;
	    case 'N': neg(xNewlineMeansNewSentence); break;
	    case 'n': neg(xPrintMatchingHelpRules); break;
	    case 'u': neg(xPrintMatchingAcceptingRules); break;
	    case 'o': neg(xOptimizeMatchings); break;
	    case 'p': neg(xPrintRulesOnly); break;
	    case 'Q': neg(xReadTaggedText); neg(xNoCollocations); break;// jonas, intended for use only for evaluation study 030120 - 030228
	    case 'q': neg(xPrintRuleHeadersOnly); break;
	    case 'r': ruleFile = argv[++c]; goto plupp;
	    case 'R': neg(xPrintRuleCount); break; 
	    case 's': neg(xPrintAllSentences); break;
  #ifdef TIMER
	    case 't': neg(xTakeTime); break;
  #endif // TIMER
	    case 'v': neg(xVerbose); break;
  #ifdef WEB_SCRUTINIZER
	    case 'w': 
	      xDoWebScrut = true;
	      WebArgv = argv + c + 1; // where web-arguments start
	      c += 4; // skip the 4 arguments for -w
	      goto plupp;
	      /* 
	      // jonas, we do not want -w to take the rest of the command line
	      return WebScrut(argc-c-1, argv+c+1); //johan020216
	      */
  #endif // WEB_SCRUTINIZER
	    case 'z': neg(xAnalyze); break;
	    case 'W': neg(xWarnAll); break;
	    case 'Z': neg(xRepeatTest); break;
  #ifdef PROBCHECK
	    case 'C':	// probchecker variable (c)onfigs
	    {
		switch(argv[c][i + 1])
		{
		case 'g':	    // function g
		    config().g_no = atoi(argv[++c]);
		    break;
		case 'G':	    // arg to function g
		    config().g_coeff = atof(argv[++c]);
		    break;
		case 'h':	    // function h
		    config().h_no = atoi(argv[++c]);
		    break;
		case 'H':	    // arg to function h
		    config().h_coeff = atof(argv[++c]);
		    break;
		case 'W':	    // specify (w)eights exactly
		{
		    for(int i = 0; i < Prob::BEST_COUNT; i++)
			config().weights[i] = atof(argv[++c]);
		    break;
		}
		case 'p':	    // the named (p)ipe to use
		    config().pipe_name = argv[++c];
		    break;
		case 'c':	    // (c)ontext type, 0 = none
		    config().use_context = atoi(argv[++c]);
		    break;
		case 'd':	    // clauses (d)elimits context
		    config().clause_delimits_context = !!atoi(argv[++c]);
		    break;
		case 'x':	    // use (x)ml output
		    config().xml_output = !!atoi(argv[++c]);
		    break;
		case 'm':	    // (m)odel
		    config().model[0] = atoi(argv[++c]);
		    break;
		case 'w':	    // (w)eight
		    config().thresh[0] = atof(argv[++c]);
		    break;
		case 'b':	    // output (B)IO format (deep analysis)
		    config().bio = !!atoi(argv[++c]);
		    break;
		case 'r':	    // (r)epresentative file to use
		    config().repr_file = argv[++c];
		    break;
		default:
		    std::cerr << "Unknown argument '-C" << argv[c][i + 1] << "' to probcheck config" << std::endl;
		    throw "Unknown argument to probcheck config";
		}
		goto plupp;
	    }
	    case 'J':	// must be given last!!! eats all remaining args
	    {
		config().decision = argv[++c];
		for(config().model_c = 0; c < argc - 1; config().model_c++)
		{
		    config().model[config().model_c] = atoi(argv[++c]);
		    config().thresh[config().model_c] = (float)atof(argv[++c]);
		}
		goto plupp;
	    }
	    case 'A': config().annot_file  = argv[++c]; goto plupp;
	    case 'O': Prob::output(argv[++c]); goto plupp;
  #endif // PROBCHECK
	    default: PrintUsage(argv[0]); return 1;
	    }
	else if (textFile) {
	  PrintUsage(argv[0]); return 1;
	} else
	  textFile = argv[c];
      plupp:
	;
      }

      if(xDoWebScrut) {
	// jonas, we do not want -w to take the rest of the command line
	return WebScrut(4, WebArgv);	      
      }

      
      if (!ruleFile)
	ruleFile = getenv("SCRUTINIZER_RULE_FILE");
      if (!xCheckAccept && !textFile) {
	std::ifstream in;
	char f[MAX_FILE_NAME_LENGTH];
	if(ruleFile) // jonas, if $SCRUTINIZER_RULE_FILE not set, ruleFile == 0
	  sprintf(f, "%s%s", ruleFile, ".testfil");
	else
	  sprintf(f, "%s", ".testfil"); // jonas, what to do here?
	if (FixIfstream(in, f))
	  textFile = f;
	else
	  textFile = getenv("SCRUTINIZER_TEST_TEXT");
      }
      bool res = !!run(ruleFile, textFile);
      if (xVerbose) PrintMemoryUsage();

#ifdef PROBCHECK
      Prob::unload();
#endif // PROBCHECK

      return res;
  }
  catch(const char *s)
  {
      Message(MSG_ERROR, s);
      return 1;
  }
  catch(...)
  {
      Message(MSG_ERROR, "an unknown (internal) error has occurred, bailing out");
      return 1;
  }
}





