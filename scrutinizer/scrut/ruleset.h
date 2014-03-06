/* ruleset.hh
 * authors: Johan Carlberger
 * last change: 2000-04-27
 * comments: RuleSet class
 */

#ifndef _ruleset_hh
#define _ruleset_hh

#include "rule.h"

const int MAX_RULES = 600;
const int MAX_HELP_RULES = 200;
const int MAX_RULE_TERMS = 700;
const int MAX_CATEGORIES = 100;

class RuleSet {
public:
  RuleSet();
  ~RuleSet() { DelObj(); }
  int NRules() const { return nRules; }
  int NHelpRules() const { return nHelpRules; }
  int NActiveRules() const { return nActiveRules; }
  bool IsOKRuleIndex(int n) const { return n>=0 && n<nRules; }
  Rule *GetRule(int n) const { ensure(IsOKRuleIndex(n)); return rules[n]; }  
  Rule *Add(Rule*);
  void Activate(Rule*);
  void InActivate(Rule*);
  void InActivate(const char *category);
  bool FixRules();  // must be called before matching
  void ComputeScope();
  void PrepareMatching(const AbstractSentence*);
  void FindMatchings(AbstractSentence*);
  void OptimizeMatchings();
  void Print(std::ostream& = std::cout) const;
  int GetJumpIndex(int n) const { return jumpIndex[n]; }   // jumpIndex > ruleno means rule is jumped over 
  void SetJumpIndex(int startPos, int endPos, int ruleNo); // ruleNo is next rule to check
  void PrintEvaluationTimes();
  void AddCategory(const char *name, const char *info,
		   const char *linkURL, const char *linkText);
  Category *FindCategory(const char*);
  bool IsFixed() const { return isFixed; }
private:
  int nRules;
  int nHelpRules;
  int nRuleTerms;
  int nActiveRules;
  int nCategories;
  bool isFixed;
  Rule *rules[MAX_RULES];
  HelpRule *helpRules[MAX_HELP_RULES];
  Rule *activeRules[MAX_RULES - MAX_HELP_RULES];
  int jumpIndex[MAX_SENTENCE_LENGTH];
  Category categories[MAX_CATEGORIES];
public:
  Timer::type matchTime;
  DecObj();
};

inline std::ostream& operator<<(std::ostream& os, const RuleSet &r) {
  r.Print(os); return os;
}
inline std::ostream& operator<<(std::ostream& os, const RuleSet *r) {
  if (r) os << *r; else os << "(null RuleSet)"; return os;
}

#endif






