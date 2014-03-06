/* element.cc
 * author: Viggo Kann (Johan Carlberger)
 * last Viggo change: 1999-09-13
 * last Johan change: 2000-02-15
 * comments: Element is a record for evaluated variable
 */

#include "rules.h"
#include "element.h"
#include "rule.h"
#include "ruleset.h"

extern int currentNoOfElement;
extern RuleSet ruleSet;
DefObj(Element);

const char *Element::Name() const {
  return id ? id->Name() : "";
}

int Element::MinScope() const { 
  if (helpRule) {
    if (occurrences != ExactlyOne) return 0;
    return helpRule->ComputeScope();
  }
  if (occurrences == ExactlyOne || occurrences == OneOrMore)
    return 1;
  else return 0;
}

bool Element::ExactScope() const {
  if (helpRule) {
    if (occurrences != ExactlyOne) return false;
    return helpRule->ExactScope();
  }
  return (occurrences == ExactlyOne);
}

Rule *Element::ResolveHelpRule(const Rule *rule) {
  if (id->type != IdEntry::RuleElementId)
    return NULL;
  if (!ruleSet.IsOKRuleIndex(id->Re().ruleId->RuleNo())) {
    std::cerr <<"ERROR, element in "<< rule 
	      <<" couldn't resolve help rule (with index: "
	      << id->Re().ruleId->RuleNo() << ')'<<std::endl; 
    return NULL;
  }
  // vilket skojigt index tycker Johan.
  helpRule = ruleSet.GetRule(id->Re().ruleId->RuleNo())->ThisIfHelpRule();
  if (!helpRule)
    std::cerr << "ERROR, an element in rule " << rule
	      << " wants to use a non-help-rule as a rule" << std::endl;
  return helpRule;
}

void Element::Init(enum elementtype t, char *r, IdEntry *i, Expr *e, int o) { 
  type = t;
  regexp = r;
  id = i;
  expr = e;
  occurrences = o;
  if (id) id->SetElementIndex(currentNoOfElement);
}

void Element::Print(std::ostream &out) const {
  out << Name() << "(";
  switch (type) {
  case Word:
    expr->Print(out);

    break;
  case Regexp:
    out << '\'' << regexp << '\'';
    break;
  default: out << "Okänd typ av element";
  }
  out << ')';
  switch (occurrences) {
  case ExactlyOne: break;
  case ZeroOrMore: out << '*'; break;
  case OneOrMore: out << '+'; break;
  case ZeroOrOne: out << '?'; break;
  default: out << "mysko: " << occurrences;
  }
}



