/* predictorsettings.hh
 * author: Johan Carlberger
 * last change: 990105
 * comments:
 */

#ifndef _predictorsettings_hh
#define _predictorsettings_hh

#include "basics.h"

const int MAX_WORD_SUGGESTIONS = 100;
extern float xNewWordFactor;
extern float xRecencyFactor;
extern float xRecencyFactorBase;
extern bool xRecency;
extern bool xFreqOnly;
extern int xNSuggestionsWanted;
extern bool xRepeatSuggestions;
extern bool xAutoInsertSpace;
extern uint xMinLettersToSave;
extern uint xMinWordLength;
extern float xLexProbExp;
extern bool xPrintContext;
extern bool xPrintSuggestions;
extern bool xSuggestNewWords;

enum SortMode {
  NONE,
  ALPHA,
  LENGTH,
  PROB
};

extern SortMode xSortMode;

#endif
