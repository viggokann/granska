/* inflectlexicon.hh
 * author: Johan Carlberger, based on work by Johan Bertenstam
 * last change: 990803
 * comments: derives inflected forms from any Word
 * which has an applicable inflection rule.
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





