/* rulesettings.cc
 * author: Johan Carlberger
 * last Johan change: 2000-05-05
 * comments: All settings start with 'x'
 */

#include "rulesettings.h"

bool xPrintAllSentences = false;
bool xPrintRulesOnly = false;
bool xPrintMatchings = false;
bool xPrintMatchingHelpRules = false;
bool xPrintMatchingAcceptingRules = false;
bool xPrintRuleHeadersOnly = false;
bool xPrintGramErrors = true;
bool xPrintOptimization = false;
bool xCheckAccept = false;
bool xAcceptSpellCapitalWords = true;
bool xAcceptSpellProperNouns = true;
bool xWarnRepeatedWords = true;
bool xAcceptAllWordsInCompounds = false;
bool xAcceptNonImprovingCorrections = false;
bool xAcceptRepeatedSuggestions = false;
bool xSuggestionSameAsOriginalMeansFalseAlarm = true;
bool xPrintRuleCount = false;
int xMaxSpellSuggestions = 4;

int xCase[20];
