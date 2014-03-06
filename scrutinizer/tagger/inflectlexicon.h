/* inflectlexicon.hh
 * author: Johan Carlberger, based on work by Johan Bertenstam
 * last change: 990803
 * comments: derives inflected forms from any Word
 * which has an applicable inflection rule.
 */

#ifndef _inflectlexicon_hh
#define _inflectlexicon_hh

#include "basics.h"
#include "inflectrule.h"
#include "tag.h"

class TagLexicon;

class InflectLexicon {
  friend class DevelopersTagger;
  friend class WordLexicon;
public:
  InflectLexicon() : isLoaded(false), ruleStringLen(0) { }
  void LoadSlow(const char *dir, const TagLexicon*);
  bool LoadFast(std::ifstream&);
  bool IsLoaded() const { return isLoaded; }
  bool Save(std::ofstream&) const;
  uint NRules() const { return nRules; }
  uint FindRuleIndex(const char *string, const char *rule, 
		     uchar baseFormTagIndex=TAG_INDEX_NONE);
  const char *GetInflectedForm(const char *string, uint ruleIndex, uchar tagIndex) const;
  const InflectRule &Rule(int n) const { return rules[n]; }
private:
  short AddString(const char *s);
  bool isLoaded;
  const char* lexiconDir;
  InflectRule rules[MAX_INFLECTION_RULES];
  int ruleStringLen;
  uint nRules;
}; 

#endif





