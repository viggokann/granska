/* settings.cc
 * author: Johan Carlberger
 * last change: 2000-05-05
 * comments:
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

#include "basics.h"
#include "file.h"
#include "settings.h"

void SetFormatSettings(OutputMode mode) {
  switch(mode) {
  case OUTPUT_MODE_UNIX:
    xPrintHTML = false;
    xBlue = xGreen = xRed = "[";
    xNoColor = "]";
    xItalic = "["; xNoItalic = "]";
    xHeading = xNoHeading = "";
    xBold = xNoBold = "";
    xSmall = xNoSmall = "";
    xTag = "<"; xNoTag = ">";
    xEndl = "\n"; xTab = "\t";
    break;
  case OUTPUT_MODE_HTML:
    xPrintHTML = true;
    xPrintOneWordPerLine = false;
    xBlue = "<FONT COLOR=\"#3366FF\">";
    xRed = "<FONT COLOR=\"#FF0000\">";
    xGreen = "<FONT COLOR=\"#009900\">";
    xNoColor = "</FONT>";
    xItalic = "<I>"; xNoItalic = "</I>";
    xHeading = "<H1>"; xNoHeading = "</H1>";
    xBold = "<B>"; xNoBold = "</B>";
    xSmall = "<FONT SIZE=\"-2\">"; xNoSmall = "</FONT>";
    xTag = "<FONT SIZE=\"-1\">&lt;"; xNoTag = "&gt;</FONT>";
    xEndl = "<BR>\n";
    xTab = "&nbsp;&nbsp;&nbsp;&nbsp;";
    break;
  case OUTPUT_MODE_PC:
    xPrintHTML = false;
    xBlue = xGreen = xRed = xNoColor = "";
    xItalic = "\""; xNoItalic = "\"";
    xHeading = xNoHeading = "";
    xBold = xNoBold = "";
    xSmall = xNoSmall = "";
    xTag = xNoTag = "";
    xEndl = "\n"; xTab = "\t";
    break;
  case OUTPUT_MODE_XML:
    xPrintHTML = false;
    xBlue = "<emph type=\"blue\">";
    xGreen = "<emph type=\"green\">";
    xRed = "<emph type=\"red\">";
    xNoColor = "</emph>";
    xItalic = "<emph type=\"italic\">";
    xNoItalic = "</emph>";
    xHeading = xNoHeading = "";
    xBold = xNoBold = "";
    xSmall = xNoSmall = "";
    xTag = "<"; xNoTag = ">";
    xEndl = "\n"; xTab = "\t";
    break;
  default:
    Message(MSG_ERROR, "unknown output mode");
  }
}

//debugging & developping:
bool xTryLatestFeature = false;
const AbstractSentence *xCurrSentence = NULL;
const RuleTerm *xCurrRuleTerm = NULL;
const int clusterLimit = 500;

// input:
bool xTaggedText = true;
bool xNewlineMeansNewSentence = false;
optConst bool xIgnoreCitation = true;
optConst bool xAddPeriodIfMissing = true;
bool xNoCollocations = false; //jonas  // johnny removed optConst

// output:
bool xOutputWTL;
const char *xEndl = "\n";
const char *xTab = "\t";
const char *xBlue = "", *xRed = "", *xGreen = "", *xNoColor = "";
const char *xItalic = "", *xNoItalic = "";
const char *xHeading = "", *xNoHeading = "";
const char *xBold = "", *xNoBold = "";
const char *xSmall = "", *xNoSmall = "";
const char *xTag = "", *xNoTag = "";
bool xPrintAllWords = false;
bool xPrintAllWordTags = false;
bool xPrintLemma = false;
bool xPrintLexicalProbs = false;
bool xPrintWordInfo = true;
bool xPrintSelectedTag = false;
bool xPrintProbs = false;
optConst bool xPrintCorrectTag = false;
bool xPrintOneWordPerLine = true;
bool xPrintSentenceProbs = false;
bool xPrintHTML = false;
bool xMarkSuspiciousSentences = false;
bool xPrintParameters = true;
optConst bool xCountFaults = false;
bool xListMultipleLemmas = false;
optConst bool xPrintUnknownLemmas = false;

// lexicon:
char *xSettingsFile = "settings";

const char xVersion[60] = "version 0.990913 PC   (c) 1999 KTH, NADA, Johan Carlberger";


// new words analysis:
bool xAnalyzeNewWords = true;
bool xCompoundRequirePrefix = true;
optConst uint xCompoundMinLength = 3;
optConst uint xCompoundPrefixMinLength = 1;
optConst uint xCompoundSuffixMinLength = 2;

// tagging:
#ifdef PROB_DOUBLE
const probType MAX_PROB = DBL_MAX * 0.01;
const probType MIN_PROB = DBL_MIN * 10000000;
#else
const probType MAX_PROB = (float) (FLT_MAX * 0.01);
const probType MIN_PROB = (float) (FLT_MIN * 1000000);
#endif
optConst bool xAlwaysNormalizeNewWords = false;
bool xTaggingExtraWords = false;
optConst bool xTagTrigramsUsed = true;
optConst bool xMorfCommonSuffix = true;
//jonas optConst bool xMorfCapital = true;
//jonas optConst bool xMorfNonCapital = true;
//jonas bool xAmbiguousNewWords = false;
optConst int xMorfCapital = true; // jonas
optConst int xMorfNonCapital = true; // jonas
int xAmbiguousNewWords = false; // jonas
optConst bool xNewWordsMemberTaggingOnly = false;
optConst int xMaxLastChars = 5;
optConst int xMinLastChars = 1;
optConst int xTaggingEquation = 19;
optConst float xLambda19 = (float) 0.00000333;
optConst float xLambdaUni = (float) 0.059549;
optConst float xLambdaBi = (float) 5.260;
optConst float xLambdaTri = (float) 2.218;
optConst float xLambdaTriExp = (float) 0.17452;
optConst float xEpsilonTri = (float) 0.006042;
optConst float xLambdaExtra = (float) 0.0001041;
optConst float xAlphaExtra = (float) 0.00010;  // was 0.466 before 2000-02-07
optConst float xEpsilonExtra = (float) 0.00000059014; // was  0.0000000059014  before 2000-02-07

optConst int xNWordVersions = 4;
optConst int xNNewWordVersions = 6;

optConst float xAlphaLastChar[10] = { 0.0f, 0.03227f, 0.08662f, 0.2241f, 0.8162f, 4.373f};
optConst float xAlphaSuffix = (float) 40;   // was 658000 before 2001-06-28
optConst float xAlphaMember = (float) 0.0000391;
optConst float xAlphaCapital = (float) 23.15;
optConst float xAlphaNonCapital = (float) 0.02243;
optConst float xAlphaUnknownCapital = (float) 27.06;
optConst float xAlphaUnknownNonCapital = (float) 0.02050;

// statistics:
optConst bool xAcceptAnyTagWhenCorrectIsSilly = true;
optConst bool xCountPunctuationMarksAsWords = true;
bool xEvaluateTagging = true;

// optimization:
bool xOptimizeMatchings = true;
optConst bool xOptimize = false;
optConst bool xOptimizeImportantParameters = true;
optConst bool xGoldenRatio = false;
float xNewParameter = (float) 0;
float xNewParameter2 = (float) 0;
float xScope = (float) 2;
bool xRandomize = false;
optConst bool xComputeLexProbs = false;

// diagnostics:
bool xVerbose = false;
bool xWarnAll = false;
optConst bool xCheckLexicons = false;
optConst bool xCheckLetters = false;
bool xRepeatTest = false;
bool xTestFeatures = false;
optConst bool xCheckNumbers = false;

// timings:
#ifdef TIMER
bool xTakeTime;
#endif

// correcting:
int xNSuggestionsWanted = 5;
bool xSuspectAllUnknownWords = true;
bool xSuggestSuspicious = false;
int xEditDistanceLimit = 200;
bool xProbChange = true;
bool xProbConcatenate = true;
bool xProbDelete = true;
bool xProbSwap = true;
float xChangeFactor = (float) 40;
float xConcatenateFactor = (float) 20;
float xDeleteFactor = (float) 1;
float xSwapFactor = (float) 1;

// predictor:
float xLexProbExp = (float) 1.02;



