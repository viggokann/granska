/* main.cc
 * author: Johan Carlberger
 * last change: 991215
 * comments: main for DevopersTagger only
 */

#include "settings.h"
#include "file.h"
#include "letter.h"
#include "developers-tagger.h"
#include "timer.h"
#include "settings_manip.h"
#include <fstream>

bool xGenerateInflections = false;
bool xGuessRulesForWords = false;
bool xGenerateExtraWordTags = false;
bool xTestInflections = false;
bool xListNewWords = false;
bool xPrint = false;
bool xSelectUnknownLemmas = false;
bool xExtractTesaurus = false;
bool xExtractKeyWords = false;

bool xReadSettings = false; // jonas
bool xNoTagging = false; // jonas


int PrintUsage(char *progName) {
  std::cerr<<"usage:"<<std::endl
      <<progName<<tab<<"[-s settingsFile] [-ABCDEFGIJMORSUWXacdeghikmnprtuvxyz]"<<std::endl
      <<"[-l lexiconDir] [-f optimization-scope] "<<std::endl
      <<tab<<"[-qTaggingEquationNumber] [-jMinNumberCharsAnalyzed] [-yMaxDito]"<<std::endl
      <<tab<<"[-o123aclxz] [-s settingsFile] [testFile]"<<std::endl
      <<"input:"<<std::endl
      <<tab<<"i:"<<dont(xIgnoreCitation)<<"ignore citation marks"<<std::endl
	   <<tab<<"K:"<<dont(xNoCollocations)<<"skip detection of collocations"<<std::endl
      <<"output:"<<std::endl
      <<tab<<"A:"<<dont(xPrintAllWords)<<"print all words"<<std::endl
      <<tab<<"B:"<<dont(xPrintLemma)<<"print lemma"<<std::endl
      <<tab<<"C:"<<dont(xPrintCorrectTag)<<"print correct tag"<<std::endl
      <<tab<<"D:"<<dont(xTestInflections)<<"test inflections"<<std::endl
      <<tab<<"E:"<<dont(xGenerateExtraWordTags)<<"generate extra word tags"<<std::endl
      <<tab<<"F:"<<dont(xCountFaults)<<"count faults"<<std::endl
      <<tab<<"G:"<<dont(xGuessRulesForWords)<<"guess rules for words"<<std::endl
      <<tab<<"H:"<<dont(xSelectUnknownLemmas)<<"select unknown lemmas"<<std::endl
      <<tab<<"I:"<<dont(xGenerateInflections)<<"generate inflections"<<std::endl
      <<tab<<"J:"<<dont(xPrintWordInfo)<<"print word info"<<std::endl
      <<tab<<"L:"<<dont(xListMultipleLemmas)<<"list multiple lemmas"<<std::endl
      <<tab<<"M:"<<dont(xPrintAllWordTags)<<"print all word-tags of words"<<std::endl
      <<tab<<"N:"<<dont(xListNewWords)<<"list all new words"<<std::endl
      <<tab<<"S:"<<dont(xPrintSelectedTag)<<"print selected tag"<<std::endl
      <<tab<<"U:"<<dont(xPrintUnknownLemmas)<<"print unknown lemmas"<<std::endl
      <<"tagging mode:"<<std::endl
	   <<tab<<"b:"<<dont(xNoTagging)<<"skip tagging (useful for pre-tagged text)"<<std::endl
      <<tab<<"c:"<<dont(xMorfCapital)<<"use initial capital morphology analysis"<<std::endl
      <<tab<<"d:"<<dont(xTagTrigramsUsed)<<"use tag trigrams"<<std::endl
      <<tab<<"n:"<<dont(xMorfNonCapital)<<"use initial non-capital morphology analysis"<<std::endl
      <<tab<<"k:"<<dont(xMorfCommonSuffix)<<"use common suffix morphology analysis"<<std::endl
      <<tab<<"a:"<<dont(xAmbiguousNewWords)<<"tag new words ambiguously"<<std::endl
      <<tab<<"u:"<<dont(xAnalyzeNewWords)<<"analyze new words"<<std::endl
      <<tab<<"P:"<<dont(xCompoundRequirePrefix)<<"require prefix when analyzing compunds"<<std::endl
      <<tab<<"w:"<<dont(xNewWordsMemberTaggingOnly)<<"tag new words by tag member statistics only"<<std::endl
    //jonas      <<tab<<"X:"<<dont(xSimplifyText)<<"simplify text"<<std::endl
      <<"statistics:"<<std::endl
      <<tab<<"p:"<<dont(xCountPunctuationMarksAsWords)<<"count punctuation marks as words"<<std::endl
      <<tab<<"e:"<<dont(xEvaluateTagging)<<"evaluate tagging"<<std::endl
      <<"optimization:"<<std::endl
      <<tab<<"g:"<<dont(xGoldenRatio)<<"use golden ratio"<<std::endl
      <<tab<<"o:"<<dont(xOptimize)<<"optimize"<<std::endl
      <<tab<<"O:"<<dont(xOptimizeImportantParameters)<<"optimize important parameters only"<<std::endl
      <<tab<<"r:"<<dont(xRandomize)<<"randomize"<<std::endl
      <<"diagnostics:"<<std::endl
      <<tab<<"h:"<<dont(xCheckLexicons)<<"check lexicons"<<std::endl
      <<tab<<"m:"<<dont(xTestFeatures)<<"test features"<<std::endl
      <<tab<<"R:"<<dont(xRepeatTest)<<"do repeat test"<<std::endl
      <<tab<<"t:"<<dont(xTakeTime)<<"take time"<<std::endl
      <<tab<<"v:"<<dont(xVerbose)<<"verbose"<<std::endl
      <<tab<<"x:"<<dont(xCheckLetters)<<"check letters"<<std::endl      
      <<tab<<"z:"<<dont(xReadSettings)<<"read tagger parameters from 'settings'-file"<<std::endl // jonas
      <<tab<<"W:"<<dont(xWarnAll)<<"warn for all suspicious things"<<std::endl
	   <<"if no lexicon directory is given, the program uses the path"<<std::endl;
  const char * temp = getenv("DEVELOPERS_TAGGER_LEXICON");
  if(!temp)
    temp = "";  // seg fault on 'std::cerr << (char*) 0 << std::endl;'
  std::cerr <<"in environment variable DEVELOPERS_TAGGER_LEXICON = "<< temp <<std::endl;
  temp = getenv("DEVELOPERS_TAGGER_OPT_TEXT");
  if (!temp) 
    temp = "";
  std::cerr <<"if no test text is given, the program uses the path"<<std::endl
	    <<"in environment variable DEVELOPERS_TAGGER_OPT_TEXT = "<<temp<<std::endl;
  return 0;
}


/*
  RepeatTest
  test if tagging the same text repeatedly
  gives the same result every time, warn
  otherwise.
*/
void RepeatTest(DevelopersTagger &tagger) {
  Message(MSG_STATUS, "performing repeat test...");
  const int N_TESTS = 10;
  xPrintParameters = false;
  int nCorrect[N_TESTS];
  int j;
  for (j=0; j<N_TESTS; j++) {
    tagger.TagText();
    if (xPrint)
      tagger.GetText().Print();
    nCorrect[j] = tagger.EvaluateTagging();
    tagger.ResetWords();
    if (j>0 && nCorrect[j] != nCorrect[j-1]) {
      Message(MSG_WARNING, "not same result when same text tagged repeatedly");
      break;
    }
  }
  if (j == N_TESTS)
    std::cerr<<"repeat test OK"<<std::endl;
  else {
    for (int i=0; i<=j; i++)
      std::cerr<<nCorrect[i]<<' ';
    std::cerr<<std::endl;
  }
}

int main(int argc, char **argv) {
  const char *lexiconDir = getenv("DEVELOPERS_TAGGER_LEXICON");
  char *opt = NULL;
  int i;
  for (i=1; i<argc && argv[i][0] == '-'; i++)
    if (argv[i][1] == 'l') {
      if (++i < argc)
	lexiconDir = argv[i];
      else
	return PrintUsage(argv[0]);
    } else if (argv[i][1] == 's') {
      if (++i < argc) {
	xSettingsFile = argv[i];
	ReadSettings(lexiconDir, xSettingsFile);
      } else
	return PrintUsage(argv[0]);
    } else if (argv[i][1] == 'o') {
      xOptimize = true;
      opt = argv[i]+2; 
    } else if (argv[i][1] == 'f') {
      if (++i < argc)
	xScope = atof(argv[i]);
      else
	return PrintUsage(argv[0]);
    }
    else for (int j=1; argv[i][j]; j++)
      switch(argv[i][j]) {
      case 'Q': neg(xExtractTesaurus); break;
      case 'Z': neg(xExtractKeyWords); break;
      case 'A': neg(xPrintAllWords); break;
      case 'B': neg(xPrintLemma); break;
      case 'C': neg(xPrintCorrectTag); break;
      case 'D': neg(xTestInflections); break;
      case 'F': neg(xCountFaults); break;
      case 'G': neg(xGuessRulesForWords); break;
      case 'H': neg(xSelectUnknownLemmas); break;
      case 'I': neg(xGenerateInflections); break;
      case 'J': neg(xPrintWordInfo); break;
      case 'K': neg(xNoCollocations); break;
      case 'L': neg(xListMultipleLemmas); break;
      case 'M': neg(xPrintAllWordTags); break;
      case 'P': neg(xCompoundRequirePrefix); break;
      case 'N': neg(xListNewWords); break;
      case 'S': neg(xPrintSelectedTag); break;
      case 'U': neg(xPrintUnknownLemmas); break;
	//jonas      case 'a': neg(xAmbiguousNewWords); break;
      case 'b': neg(xNoTagging); break; // jonas
      case 'c': neg(xMorfCapital); break;
      case 'd': neg(xTagTrigramsUsed); break;
      case 'e': neg(xEvaluateTagging); break;
      case 'f': std::cerr << "this shouldn't happen" << std::endl; break;
      case 'g': neg(xGoldenRatio); break;
      case 'h': neg(xCheckLexicons); break;
      case 'i': neg(xIgnoreCitation); break;
      case 'j': xMinLastChars = atoi(argv[i]+j+1); argv[i][j+1]=0; break;
      case 'k': neg(xMorfCommonSuffix); break;
      case 'l': std::cerr << "this shouldn't happen" << std::endl; break;
      case 'm': neg(xTestFeatures); break;
      case 'n': neg(xMorfNonCapital); break;
      case 'O': neg(xOptimizeImportantParameters); break;
      case 'o': std::cerr << "this shouldn't happen" << std::endl; break;
      case 'p': neg(xCountPunctuationMarksAsWords); break;
      case 'q': xTaggingEquation = atoi(argv[i]+j+1); argv[i][j+1]=0; break;
      case 'r': neg(xRandomize); break;
      case 's': std::cerr << "this shouldn't happen" << std::endl; break;
      case 't': neg(xTakeTime); break;
      case 'u': neg(xAnalyzeNewWords); break;
      case 'v': neg(xVerbose); break;
      case 'w': neg(xNewWordsMemberTaggingOnly); break;
      case 'x': neg(xCheckLetters); break;
      case 'y': xMaxLastChars = atoi(argv[i]+j+1); argv[i][j+1]=0; break;
      case 'z': neg(xReadSettings); break; // jonas
      case 'E': neg(xGenerateExtraWordTags); break;
      case 'W': neg(xWarnAll); break;
      case 'R': neg(xRepeatTest); break;
	//jonas      case 'X': neg(xSimplifyText); break;
      default: return PrintUsage(argv[0]);
      }
  
  if (i == argc) {
    argv[i] = getenv("DEVELOPERS_TAGGER_OPT_TEXT");
    if (!argv[i])
      return PrintUsage(argv[0]);
    argc++;
  }
  if (!lexiconDir)
    return PrintUsage(argv[0]);
  xPrint = xPrintCorrectTag || xPrintSelectedTag || xPrintAllWords;
  std::cout.precision(5);
  if (xCheckLetters) {
    CheckHash();
    // doLetterTest();
    return 0;
  }
  DevelopersTagger tagger;
  tagger.Load(lexiconDir);
  if (xExtractTesaurus) {
    tagger.ExtractTesaurusWordTags("/afs/nada.kth.se/misc/tcs/lexicons/riksdan/tesaurus/tesaurus.txt");
    return 0;
  }
  if(xReadSettings) { // jonas
    ReadSettings(tagger.LexiconDir(), "settings");
  }
  if (xExtractKeyWords) {
    tagger.LoadTesaurus("/afs/nada.kth.se/misc/tcs/granska/suc/lexicons/tesaurus.wtl",
			"/afs/nada.kth.se/misc/tcs/lexicons/riksdan/tesaurus/tesaurus.txt");
    std::ifstream in;
    tagger.ExtractIndexWords(&argv[i], argc-i);
    return 0;
  }
  Message(MSG_COUNTS, "during loading");
  if (xPrintUnknownLemmas)
    return 0;
  if (xCheckLexicons)
    tagger.LexiconCheck();
  if (xGenerateExtraWordTags) {
    tagger.GenerateExtraWordTags(xSelectUnknownLemmas);
    return 0;
  }
  if (xGenerateInflections) {
    tagger.GenerateInflections();
    return 0;
  }
  if (xGuessRulesForWords) {
    tagger.GuessRulesForWords();
    return 0;
  }
  if (xTestInflections) {
    tagger.TestInflections();
    return 0;
  }
  for (; i<argc; i++) {
    std::ifstream in;
    in.open(argv[i]);
    if(!in) {
      std::cerr << "could not open " << argv[i] << std::endl;
      continue;
    }

    in.seekg (0, std::ios::end); // jonas
    int inlength = in.tellg(); // jonas
    in.seekg (0, std::ios::beg); // jonas
    if(inlength < 1000)
      inlength = 1000;
    tagger.SetStream(&in, inlength);//jonas tagger.SetStream(&in, !strcmp(Extension(argv[i]), "html"));
    if (xOptimize) {
      tagger.Optimize(opt);
      if (xPrint)
	tagger.GetText().Print();
    } else {
      if (strcmp(Extension(argv[i]), "wt")) {
	/* do ordinary tagging */
	xEvaluateTagging = false;
	xTaggedText = false;
	xPrintCorrectTag = false;
	if (!xPrint)
	  xPrint = true;
	else
	  xPrintAllWords = true;
	tagger.ReadText(); 
	tagger.TagText();  
	if (xTakeTime) 
	  tagger.PrintTimes();
	if (xPrintAllWords || xPrintCorrectTag || xOutputWTL)
	  tagger.GetText().Print();
	in.close();
	return 0; 
      } else { // extension is .wt
	xTaggedText = true;
	tagger.ReadTaggedText();
      }
      Message(MSG_COUNTS, "during text reading");
      if (xRepeatTest)
	RepeatTest(tagger);
      else {
	tagger.ResetWords();
	if(xNoTagging)
	  tagger.DontTagText(); // use "correct" tags as tagging
	else
	  tagger.TagText();
	if (xTakeTime) 
	  tagger.PrintTimes();
	if (xListNewWords) {
	  const Tagger *t = &tagger;
	  t->NewWords().PrintObjects();
	} else {
	  if (xPrint)
	    tagger.GetText().Print();
	  if (xEvaluateTagging) 
	    tagger.EvaluateTagging();
	}
      }
    }
    in.close();
  }
  if (xCheckLexicons)
    tagger.PrintStatistics();
  Message(MSG_COUNTS, "during tagging");
  return 0;
}

