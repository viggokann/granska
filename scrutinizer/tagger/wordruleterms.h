/* wordruleterms.hh
 * author: Johan Carlberger
 * last change: 2000-03-23
 * comments: Wordruleterms class
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

#ifndef _wordruleterms_hh
#define _wordruleterms_hh

#include "word.h"

class RuleTermList {
public:
  RuleTermList(const RuleTerm *r = NULL, RuleTermList *n = NULL) :
    ruleTerm(r),
    next(n) { NewObj();
  }
  ~RuleTermList() { DelObj(); if (next) delete next; }
  const RuleTerm* GetRuleTerm() const { return ruleTerm; }
  const RuleTermList *Next() const { return next; }
protected:
  const RuleTerm *ruleTerm;
  RuleTermList *next;
  DecObj();
};

class WordRuleTerms : public RuleTermList {
public:
  WordRuleTerms(Word *w) : word(w) {} 
  WordRuleTerms(Word *w, const RuleTerm *r) :
    RuleTermList(r),
    word(w) {
      word->SetRuleAnchor(true);
  }
  void AddRuleTerm(const RuleTerm *r) {
    next = new RuleTermList(ruleTerm, next);
    ruleTerm = r;
  }
  const Word *GetWord() const { return word; }
private:
  Word *word;
};

inline uint KeyWordRuleTerms(const WordRuleTerms &w) {
  return Hash(w.GetWord()->String());
}
inline int CompareWordRuleTerms(const WordRuleTerms &w1, const WordRuleTerms &w2) {
  return strcmp(w1.GetWord()->String(), w2.GetWord()->String());
}

#endif
