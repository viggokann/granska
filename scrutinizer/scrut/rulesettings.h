/* rulesettings.hh
 * author: Johan Carlberger
 * last Johan change: 2000-05-05
 * comments: All settings start with 'x'
 */

#ifndef _rulesettings_hh
#define _rulesettings_hh

#include "basics.h"

#if defined DEVELOPERS
  #define optConst
#else
  #define optConst const 
#endif

const int MAX_SUGGESTIONS = 10;      // can be set arbitrarily

extern bool xPrintAllSentences;
extern bool xPrintRulesOnly;
extern bool xPrintMatchings;
extern bool xPrintMatchingHelpRules;
extern bool xPrintMatchingAcceptingRules;
extern bool xPrintRuleHeadersOnly;
extern bool xPrintGramErrors;
extern bool xPrintOptimization;
extern bool xCheckAccept;
extern bool xAcceptSpellCapitalWords;
extern bool xAcceptSpellProperNouns;
extern bool xWarnRepeatedWords;
extern bool xAcceptAllWordsInCompounds;
extern bool xAcceptNonImprovingCorrections;
extern bool xAcceptRepeatedSuggestions;
extern bool xSuggestionSameAsOriginalMeansFalseAlarm;
extern int xMaxSpellSuggestions;
extern bool xPrintRuleCount;

extern int xCase[20];

#endif
