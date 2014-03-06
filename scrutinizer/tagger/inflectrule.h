/* inflectrule.hh
 * author: Johan Carlberger, based on work by Johan Bertenstam
 * last change: 2000-02-13
 * comments: derives inflected forms from any Word
 * which has an applicable inflection rule.
 */

#ifndef _inflectrule_hh
#define _inflectrule_hh

#include "basics.h"
#include <fstream>
#include <limits.h>

const uint MAX_INFLECTION_RULES = 450;
const int MAX_INFLECTION_FORMS = 22;
const int MAX_INFLECTION_ENDINGS = 10;
const uint INFLECT_NO_RULE = 511;
const int INFLECTION_FORM_NONE = SHRT_MAX;
const int MAX_INFLECTION_RULES_PER_WORDTAG = 3; //cannot exceed 4 without more bits for nExtraInflectRules in Word

class InflectLexicon;

class InflectRule {
  friend class InflectLexicon;
public:
  bool IsApplicable(const char *string) const;
  bool IsApplicable(const char *string, uchar baseFormTagIndex) const;
  const char *Apply(const char *string, int form) const;
  const char *Name() const { return strings + nameIndex; }
  const char *Ending(int n) const { return strings + endingIndex[n]; }
  const char *Form(short f) const { return strings + f; }
  int NCharsToRemove() const { return nCharsToRemove; }
  uchar TagIndex(int n) const { return tagIndex[n]; } 
  int NForms() const { return nForms; }
private:
  InflectRule() : used(false) {}
  static char *strings;
  short nameIndex;
  short endingIndex[MAX_INFLECTION_ENDINGS];
  uchar tagIndex[MAX_INFLECTION_FORMS];
  short formIndex[MAX_INFLECTION_FORMS];
  int nCharsToRemove;
  int nEndings;
  int nForms;
  bool used;
};

inline std::ostream& operator<<(std::ostream& os, const InflectRule &r) {
  os << r.Name();
  return os;
}

inline std::ostream& operator<<(std::ostream& os, const InflectRule *r) {
  if (r) os << *r; else os << "(null InflectRule)";
  return os;
}

#endif





