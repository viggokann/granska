/* main.cc
 * author: Johan Carlberger
 * last change: 2001-12-06
 * comments: main for Tagger only
 */

/******************************************************************************

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

******************************************************************************/

#include "file.h"
#include "letter.h"
#include "settings.h"
#include "tagger.h"
#include <iostream>
#include <fstream>

bool xGenerateInflections = false;
bool xReadTaggedText = false;

void PrintUsage(char *progName) {
  std::cerr << "usage:" << std::endl
       << progName << tab << "[-ABDMNPUatuv] [-l lexicon-directory]" << std::endl
       << tab << "[textFile]*" << std::endl
       << "input:" << std::endl
       <<tab<<"N:"<<dont(xNewlineMeansNewSentence)<<"interpret newline to indicate end of sentence/paragraph"<<std::endl
	    <<tab<<"Q:"<<dont(xReadTaggedText)<<"read text that is already tagged"<<std::endl
	    <<tab<<"R:"<<dont(xNoCollocations)<<"make collocations many tokens"<<std::endl
       << "output:" << std::endl
       <<tab<<"A:"<<dont(xPrintAllWords)<<"print all words"<<std::endl
       <<tab<<"B:"<<dont(xPrintLemma)<<"print lemma"<<std::endl
       <<tab<<"D:"<<dont(xListMultipleLemmas)<<"list multiple lemma"<<std::endl
       <<tab<<"G:"<<dont(xGenerateInflections)<<"generate inflections"<<std::endl
       <<tab<<"I:"<<dont(xPrintWordInfo)<<"print words info"<<std::endl
       <<tab<<"L:"<<dont(xPrintLexicalProbs)<<"print lexical probs"<<std::endl
       <<tab<<"M:"<<dont(xPrintAllWordTags)<<"print all word-tags of words"<<std::endl
       <<tab<<"P:"<<dont(xPrintProbs)<<"print estimated probabilities of chosen tags"<<std::endl
       <<tab<<"S:"<<dont(xPrintSelectedTag)<<"print selected tag"<<std::endl
       <<tab<<"W:"<<dont(xOutputWTL)<<"print wtl-format"<<std::endl
       <<"tagging mode:"<<std::endl
       <<tab<<"a:"<<dont(!!xAmbiguousNewWords)<<"tag new words ambiguously"<<std::endl
       <<tab<<"u:"<<dont(xAnalyzeNewWords)<<"analyze new words"<<std::endl
       <<"diagnostics:"<<std::endl
       <<tab<<"F:"<<dont(xTryLatestFeature)<<"try latest feature"<<std::endl
       <<tab<<"f:"<<dont(xTestFeatures)<<"test feaures"<<std::endl
#ifdef TIMER
       <<tab<<"t:"<<dont(xTakeTime)<<"take time"<<std::endl
#endif
       <<tab<<"v:"<<dont(xVerbose)<<"verbose"<<std::endl<<std::endl
	    <<"if no lexicon directory is given with option -l, the program uses the path"<<std::endl;
  
  const char * temp = getenv("TAGGER_LEXICON");
  if(!temp)
    temp = "";  // seg fault on 'std::cerr << (char*) 0 << std::endl;'
  std::cerr <<"in environment variable TAGGER_LEXICON = "<<temp<<std::endl
	    <<"or the current working directory if TAGGER_LEXICON is null"<<std::endl;
  temp = getenv("TAGGER_TEST_TEXT");
  if(!temp)
    temp = "";
  std::cerr <<"if no test text is given, the program uses the path"<<std::endl
	    <<"in environment variable TAGGER_TEST_TEXT = "<<temp<<std::endl;
}

int main(int argc, char **argv) {
  char *lexiconDir = getenv("TAGGER_LEXICON");
  int i;
  xPrintAllWords = xPrintSelectedTag = true;
  for (i=1; i<argc && argv[i][0] == '-'; i++)
    if (argv[i][1] == 'l') {
      if (++i < argc)
	lexiconDir = argv[i];
      else {
	PrintUsage(argv[0]); return 1;
      }
    } else for (int j=1; argv[i][j]; j++)
      switch(argv[i][j]) {
      case 'A': neg(xPrintAllWords); break;
      case 'B': neg(xPrintLemma); break;
      case 'D': neg(xListMultipleLemmas); break;
      case 'G': neg(xGenerateInflections); break;
      case 'I': neg(xPrintWordInfo); break;
      case 'L': neg(xPrintLexicalProbs); break;
      case 'M': neg(xPrintAllWordTags); break;
      case 'N': neg(xNewlineMeansNewSentence); break;
      case 'P': neg(xPrintProbs); break;
      case 'S': neg(xPrintSelectedTag); break;
      case 'W': neg(xOutputWTL); break;
      case 'Q': neg(xReadTaggedText); break;
      case 'R': neg(xNoCollocations); break;
      case 'a': neg(xAmbiguousNewWords); break;
      case 'f': neg(xTestFeatures); break;
      case 'F': neg(xTryLatestFeature); break;
#ifdef TIMER
      case 't': neg(xTakeTime); break;
#endif
      case 'u': neg(xAnalyzeNewWords); break;
      case 'v': neg(xVerbose); break;
      default: PrintUsage(argv[0]); return 1;
      }
  if (!lexiconDir)
    lexiconDir = ".";
  if (i == argc) {
    argv[i] = getenv("TAGGER_TEST_TEXT");
    if (!argv[i]) {
      PrintUsage(argv[0]); return 1;
    }
    argc++;
  }
  if (xTakeTime) {
    xPrintProbs = xPrintAllWords = false;
  }
  else if(xPrintProbs) { // jonas
    xPrintAllWords = false; // jonas
    xPrintSelectedTag = true;
  }
  else if (!xPrintAllWords && !xPrintAllWordTags)
    xPrintAllWords = xPrintSelectedTag = true;

  //  std::cout.precision(0);
  std::cout.setf(std::ios::fixed); 
  Tagger tagger;
  if(!tagger.Load(lexiconDir))
      return 0;
  Message(MSG_COUNTS, "during loading");
  if (xGenerateInflections) {
    tagger.GenerateInflections();
    return 0;
  }
  if (xVerbose) 
    std::cout << argv[0] << ' ' << xVersion << std::endl;
  for (; i<argc; i++) {
    std::ifstream in;
    if (!FixIfstream(in, argv[i]))
      continue;

    in.seekg (0, std::ios::end); // jonas
    int inlength = in.tellg(); // jonas
    in.seekg (0, std::ios::beg); // jonas
    tagger.SetStream(&in, inlength);
    if(xReadTaggedText) {
      tagger.ReadTaggedTextQnD();
      //      tagger.TagText();
    } else {
      tagger.ReadText();
      tagger.TagText();
    }
    if (xTakeTime) tagger.PrintTimes();
    if (xPrintAllWords || xPrintCorrectTag || xOutputWTL)
      tagger.GetText().Print();
    in.close();
  }
  Message(MSG_COUNTS, "during tagging");
  return 0;
}

