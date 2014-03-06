/* matching.hh
 * author: Viggo Kann (Johan Carlberger)
 * last Viggo change: 1998-12-13
 * last Johan change: 2000-04-19
 * comments: Matching and ElementMatching classes
 */

#ifndef _matching_hh
#define _matching_hh

#include <iosfwd>
#include "wordtoken.h"
#include "element.h"
#include "rule.h"

class ElementMatching {
  friend class MatchingSet;
  friend class RuleTerm;
public:
  ElementMatching()
      : tokenX(0), element(0),
        occurrences(0), helpRuleTag(0),
	matching(0)
  {
      NewObj(); // no initializing is faster
  }
  ~ElementMatching() { DelObj(); }
  void Print(std::ostream&) const;
  bool IsHelpRule() const { return !element || element->IsHelpRule(); } 
  WordToken *GetWordToken(int n = 0) const { return tokenX[n]; }
  WordToken **GetWordTokensAt(int n = 0) const { return tokenX + n; }
  const Element *GetElement() const { return element; }
  Tag *GetTag() const	// jbfix: ElementMatching was not initialized if (NP)()?, gender:=NP.gender
  {
      if(helpRuleTag)
	  return helpRuleTag;
      else
      {
	  if(tokenX)
	     return (*tokenX)->SelectedTag();  
	  else
	     return 0;
      }
  }
  int Occurrences() const { return occurrences; }
  ChangeableTag *GetHelpRuleTag() const { return helpRuleTag; }
  Matching *GetMatching() const { return matching; }
  void PrintParse(std::ostream& = std::cout) const; 
private:
  void Init(WordToken **t, int occ, ChangeableTag *tag) {
    tokenX = t; element = NULL; occurrences = occ; helpRuleTag = tag; }
  WordToken **tokenX;
  const Element *element;              // matched element in rule
  int occurrences;                     // no of times the element is matched (if indexed)
  ChangeableTag *helpRuleTag;          // created tag (used only for helprule)
  Matching *matching;                  // (used only for helprule)
  DecObj();
};

class Matching {
  friend class HelpRule;
  friend class RuleTerm;
  friend class Rule;
  friend class MatchingSet;
  friend class SuspectArea;
public:
  Matching() { NewObj(); }
  Matching(AbstractSentence *s, RuleTerm *rt, ElementMatching *es, int st) :
    sentence(s), ruleTerm(rt), elements(es), start(st)
    /*, altSentence(NULL), nextHelpRuleMatching(NULL)*/ { NewObj(); }
  ~Matching() { DelObj(); }
  RuleTerm* GetRuleTerm() const { return ruleTerm; }
  Rule* GetRule() const { return GetRuleTerm()->GetRule(); }
  ElementMatching &GetElementMatching(int n) const { return elements[n]; }
  ElementMatching *Elements() const { return elements; }
  int NElements() const { return ruleTerm->NElements(); }
  int Start() const { return start; }
  int StartWithoutContext() const { return start + GetRuleTerm()->LeftContextLength(); }
  int End() const { return start + nTokens - 1; }
  int NTokens() const { return nTokens; }
  int NTokensWithoutContext() const { return nTokensWithoutContext; }
  void Print(std::ostream& = std::cout) const;
  AbstractSentence *GetSentence() const { return sentence; }
  const Matching *NextHelpRuleMatching() const { return nextHelpRuleMatching; }
  DynamicSentence *GetAltSentence() const { return altSentence; }
  void SetAltSentence(DynamicSentence* s) { altSentence = s; }
  void AddAltSentences(DynamicSentence*);
  DynamicSentence *DuplicateAltSentences();
  bool HasCorrection() const { return GetRuleTerm()->GetCorr() != NULL; }
  void PrintParse(std::ostream& = std::cout) const;
private:
  AbstractSentence *sentence;          // sentence that the matching is included in
  RuleTerm *ruleTerm;                  // matched rule term
  ElementMatching *elements;           // array of ruleTerm->NElements() matched elements
  uchar start;                         // index in sentence of first word in matching
  uchar nTokens;                       // total number of tokens in matching
  uchar nTokensWithoutContext;         // no of tokens in matching without context
  uchar rightContextStart;             // token no of start of right context
  DynamicSentence *altSentence;
  Matching *nextHelpRuleMatching;
  DecObj();
};

/*
class HelpRuleMatching : public Matching {
public:
private:
Matching *nextHelpRuleMatching;
ChangeableTag *helpRuleTag;
};
*/

inline std::ostream& operator<<(std::ostream& os, const Matching &m) {
  m.Print(os); return os;
}
inline std::ostream& operator<<(std::ostream& os, const Matching *m) {
  if (m) os << *m; else os << "(null Matching)"; return os;
}
inline std::ostream& operator<<(std::ostream& os, const ElementMatching &m) {
  m.Print(os); return os;
}
inline std::ostream& operator<<(std::ostream& os, const ElementMatching *m) {
  if (m) os << *m; else os << "(null ElementMatching)"; return os;
}

#endif
