/* settings.hh
 * author: Johan Carlberger
 * last change: 2000-05-05
 * comments: All settings start with 'x'
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

#ifndef _settings_hh
#define _settings_hh

#include "basics.h"
#include <limits.h>
#include <float.h>

#if defined DEVELOPERS
#define optConst
#else
#define optConst const 
#endif

enum OutputMode {
  OUTPUT_MODE_UNIX,
  OUTPUT_MODE_HTML,
  OUTPUT_MODE_PC,
  OUTPUT_MODE_XML
};

// debugging & developping:
extern bool xTryLatestFeature;
extern const int clusterLimit;
void SetFormatSettings(OutputMode);
class RuleTerm;
extern const RuleTerm *xCurrRuleTerm;
class AbstractSentence;
extern const AbstractSentence *xCurrSentence;

// input:
extern bool xTaggedText;
extern bool xNewlineMeansNewSentence;
extern optConst bool xIgnoreCitation;
extern optConst bool xAddPeriodIfMissing;
extern bool xNoCollocations; //jonas  // johnny removed optConst

// output:
extern bool xOutputWTL;
extern const char *xEndl, *xTab;
extern const char *xBlue, *xRed, *xGreen, *xNoColor;
extern const char *xItalic, *xNoItalic;
extern const char *xHeading, *xNoHeading;
extern const char *xBold, *xNoBold;
extern const char *xSmall, *xNoSmall;
extern const char *xTag, *xNoTag;
extern bool xPrintAllWords;
extern bool xPrintAllWordTags;
extern bool xPrintLemma;
extern bool xPrintLexicalProbs;
extern bool xPrintWordInfo;
extern bool xPrintSelectedTag;
extern bool xPrintProbs;
extern optConst bool xPrintCorrectTag;
extern bool xPrintOneWordPerLine;
extern bool xPrintSentenceProbs;
extern bool xPrintHTML;
extern bool xMarkSuspiciousSentences;
extern bool xPrintParameters;
extern optConst bool xCountFaults;
extern bool xListMultipleLemmas;
extern optConst bool xPrintUnknownLemmas;

// lexicon:
extern char *xSettingsFile;
extern const char xVersion[60];

// new words analysis:
extern bool xAnalyzeNewWords;
extern bool xCompoundRequirePrefix;
extern optConst uint xCompoundMinLength;
extern optConst uint xCompoundPrefixMinLength;
extern optConst uint xCompoundSuffixMinLength;

// tagging:
#ifdef PROB_DOUBLE
typedef double probType;
#else
typedef float probType;
#endif
extern const probType MAX_PROB;
extern const probType MIN_PROB;
extern optConst bool xAlwaysNormalizeNewWords;
extern bool xTaggingExtraWords;
extern optConst bool xTagTrigramsUsed;
extern optConst int xTaggingEquation;
// jonas extern optConst bool xMorfCapital; // used as int
// jonas extern optConst bool xMorfNonCapital; // used as int
extern optConst int xMorfCapital; // jonas, used as int
extern optConst int xMorfNonCapital; // jonas, used as int

extern optConst bool xMorfUnknownCapital;
extern optConst bool xMorfUnknownNonCapital;
extern optConst float xAlphaCapital;
extern optConst float xAlphaUnknownCapital;
extern optConst float xAlphaNonCapital;
extern optConst float xAlphaUnknownNonCapital;
extern optConst bool xMorfCommonSuffix;
//jonas extern bool xAmbiguousNewWords; // used as int
extern int xAmbiguousNewWords; // jonas, used as int
extern optConst bool xNewWordsMemberTaggingOnly;
extern optConst int xNWordVersions;
extern optConst int xNNewWordVersions;
extern optConst float xAlphaSuffix;
extern optConst int xMinLastChars;
extern optConst int xMaxLastChars;
extern optConst float xAlphaLastChar[10];
extern optConst float xLambdaUni;
extern optConst float xLambdaBi;
extern optConst float xLambdaTri;
extern optConst float xLambdaTriExp;
extern optConst float xLambda19;
extern optConst float xLambdaExtra;
extern optConst float xAlphaExtra;
extern optConst float xEpsilonExtra;
extern optConst float xEpsilonTri;
extern optConst float xAlphaMember;

// statistics:
extern optConst bool xCountPunctuationMarksAsWords;
extern optConst bool xAcceptAnyTagWhenCorrectIsSilly;
extern bool xEvaluateTagging;

// optimization:
extern bool xOptimizeMatchings;
extern optConst bool xOptimize;
extern optConst bool xOptimizeImportantParameters;
extern optConst bool xGoldenRatio;
extern float xNewParameter;
extern float xNewParameter2;
extern float xScope;
extern bool xRandomize;
extern optConst bool xComputeLexProbs;

// diagnostics:
extern bool xVerbose;
extern bool xWarnAll;
extern optConst bool xCheckLexicons;
extern optConst bool xCheckLetters;
extern bool xRepeatTest;
extern bool xTestFeatures;
extern optConst bool xCheckNumbers;

// correcting:
extern int xNSuggestionsWanted;
extern bool xSuspectAllUnknownWords;
extern bool xSuggestSuspicious;
extern int xEditDistanceLimit;
extern bool xProbChange;
extern bool xProbConcatenate;
extern bool xProbDelete;
extern bool xProbSwap;
extern float xChangeFactor;
extern float xConcatenateFactor;
extern float xDeleteFactor;
extern float xSwapFactor;

// predictor:
extern float xLexProbExp; 

#endif
