/* wordruleterms.hh
 * author: Johan Carlberger
 * last change: 2000-03-23
 * comments: Wordruleterms class
 */

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
