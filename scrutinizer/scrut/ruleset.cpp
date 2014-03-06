/* ruleset.cc
 * authors: Johan Carlberger
 * last change: 2000-04-27
 * comments: RuleSet class
 */

#include "rules.h"
#include "ruleset.h"
#include "rulesettings.h"
#include "timer.h"

DefObj(RuleSet);

RuleSet::RuleSet() :
  nRules(0), nHelpRules(0), nRuleTerms(0),
  nActiveRules(0), nCategories(0), isFixed(false), matchTime(0) {
  ensure(MAX_RULES > MAX_HELP_RULES);
  NewObj();
}

void RuleSet::AddCategory(const char *name, const char *info,
			  const char *linkURL, const char *linkText) {
  if (nCategories >= MAX_CATEGORIES)
    Message(MSG_ERROR, "too many categories; max is", int2str(MAX_CATEGORIES), "must recompile");
  categories[nCategories].name = name;
  categories[nCategories].info = info;
  categories[nCategories].linkURL = linkURL;
  categories[nCategories].linkText = linkText;
  nCategories++;
}

Category *RuleSet::FindCategory(const char *s) {
  for (int i=0; i<nCategories; i++)
    if (!strcmp(categories[i].Name(), s))
      return categories + i;
  return NULL;
}

Rule *RuleSet::Add(Rule *r) {
  if (nRules >= MAX_RULES)
    Message(MSG_ERROR, "too many rules; max is", int2str(MAX_RULES), "must recompile");
  HelpRule *hr = r->ThisIfHelpRule();
  if (hr) {
    if (nHelpRules >= MAX_HELP_RULES)
      Message(MSG_ERROR, "too many help rules; max is", int2str(MAX_HELP_RULES), "must recompile");
    helpRules[nHelpRules] = hr;
    nHelpRules++;
  } else {
    if (nActiveRules > MAX_RULES - MAX_HELP_RULES)
      Message(MSG_ERROR, "too many non-help rules, increase MAX_RULES and recompile");
    activeRules[nActiveRules++] = r;
  }
  rules[nRules] = r;
  r->number = nRules;
  nRules++;
  nRuleTerms += r->NTerms();
  if (nRuleTerms >= MAX_RULE_TERMS)
    Message(MSG_ERROR, "too many ruleterms, increase MAX_RULE_TERMS and recompile");
  return r;
}

void RuleSet::Activate(Rule *r) {
  if (r->IsHelpRule()) {
    Message(MSG_WARNING, "cannot activate a help rule");
    return;
  }
  for (int i=0; i<nActiveRules; i++) {
    if (activeRules[i] == r) // r is already active
      return;
    if (activeRules[i]->Number() > r->Number()) { // r must be put in pos i
      int j;
      for (j=nActiveRules; j>i; j--)
	activeRules[j] = activeRules[j-1];
      activeRules[i] = r;
      nActiveRules++;
      for (j=i+1; j<nActiveRules; j++) {
	ensure(activeRules[j] != r);
	ensure(activeRules[j]->Number() > activeRules[j-1]->Number());
      }
      return;
    }
  }
}

void RuleSet::InActivate(Rule *r) {
  if (r->IsHelpRule()) {
    Message(MSG_WARNING, "cannot inactivate a help rule");
    return;
  }
  for (int i=0; i<nActiveRules; i++) {
    if (activeRules[i] == r) {
      for (int j=i+1; j<nActiveRules; j++)
	activeRules[j-1] = activeRules[j];
      nActiveRules--;
      return;
    }
  }
} 

void RuleSet::InActivate(const char *ctg) {
  for (int i=0; i<nRules; i++) {
    Rule *r = rules[i];
    if (!r->IsHelpRule() && r->CategoryName() && !strcmp(ctg, r->CategoryName()))
      InActivate(r);
  }
}

void RuleSet::Print(std::ostream& out) const {
  if (xPrintRuleHeadersOnly)
    for (int i=0; i<nRules; i++)
      out << rules[i]->Header() << std::endl;
  if (xPrintRulesOnly)
    for (int i=0; i<nRules; i++) {
      rules[i]->Print(out);
      out << std::endl;
    }
}

bool RuleSet::FixRules() {
  isFixed = true;
  for (int i=0; i<nRules; i++) {
    Rule *r = rules[i]; 
    if (r->Id() && r->Id()->RuleNo() < 0) {
      Message(MSG_WARNING, "help rule", r->Name(), "is not defined");
      isFixed = false;
    } else 
      r->ResolveHelpRules();
    for (RuleTerm *rt = r->FirstRuleTerm(); rt; rt = rt->Next())
      if (rt->GetJump() && rt->GetJump()->GetId()->GetRuleIndex() <= i) {
	Message(MSG_WARNING, r->Name(), "jumps backwards");
	isFixed = false;
      }
    if (!r->IsHelpRule()) {
      if (!r->CategoryName()) {
	Message(MSG_WARNING, "rule", r->Name(), "has no category");
	//isFixed = false;
      } else {
	Category *cat = FindCategory(r->CategoryName());
	if (!cat) {
	  Message(MSG_WARNING, "category", r->CategoryName(), "never declared");
	  isFixed = false;
	} else 
	  for (RuleTerm *rt = r->FirstRuleTerm(); rt; rt = rt->Next())
	    rt->category = cat;
      }
      if (r->FirstRuleTerm() && !r->FirstRuleTerm()->IsScrutinizing() &&
	  r->FirstRuleTerm()->GetInfo()) {
	Message(MSG_WARNING, r->Name(), ", only scrutinizing rules can have info");
	isFixed = false;
      }
    }
  }
  return isFixed;
}

void RuleSet::ComputeScope() {
  for (int i=0; i < nRules; i++)
    if (rules[i]->ComputeScope() == 0)
      Message(MSG_WARNING, rules[i]->Header(), "accepts zero tokens");
}

#include <iostream>
void RuleSet::SetJumpIndex(int startPos, int endPos, int ruleNo) {
  //std::cout << xCurrRuleTerm << " sets jump " << startPos << " - " << endPos << " to " << ruleNo << std::endl;
  if (startPos < 1)
    Message(MSG_WARNING, "bad startPos", int2str(startPos));
  ensure(startPos >= 0); // Viggo changed from >0 to >= 0 2000-04-04
  for (int p=startPos; p<=endPos; p++) {
    if (jumpIndex[p] > ruleNo &&
	jumpIndex[p] != NRules() &&
	!rules[ruleNo]->IsHelpRule()) {
      Message(MSG_WARNING, GetRule(ruleNo)->Name(), int2str(p), 
	      "bad jump; rules already jumped to",
	      jumpIndex[p] == NRules() ? "endrules" : GetRule(jumpIndex[p])->Name());
    }
    jumpIndex[p] = ruleNo;
  }
}

void RuleSet::PrepareMatching(const AbstractSentence *s) {
  //  if (xPrintMatchings) std::cout <<  "preparing matchings" << std::endl;  
  int i;
  for (i=0; i<nHelpRules; i++)
    helpRules[i]->ResetTried(s->NWords()+2);
  for (i=1; i<s->NTokens(); i++) // +4 just to avoid warnings
    jumpIndex[i] = -1;
}

void RuleSet::FindMatchings(AbstractSentence *s) {
  PrepareMatching(s);
  for (int i=0; i<nActiveRules; i++) {
    Rule *r = activeRules[i];
    r->FindMatchings(s);
  }
}

void RuleSet::OptimizeMatchings() {
  Message(MSG_STATUS, "optimizing rules...");
  RuleTerm::CreateMatchingCheck();
  for (int i = 0; i < nRules; i++)
    rules[i]->OptimizeRuleMatching();
}

void RuleSet::PrintEvaluationTimes() {
  matchTime = 0;
  std::cout.precision(2);
  std::cout.setf(std::ios::fixed);
  int i;
  for (i=0; i<NActiveRules(); i++)
    matchTime += activeRules[i]->evaluationTime;
  const float limit = 1.0;
  std::cout << 0.0 << tab << "rules using more than " << limit << " % of evaluation time:" << std::endl;
  for (i=0; i<NActiveRules(); i++)
    if (100.0*activeRules[i]->evaluationTime/matchTime > limit)
      std::cout << 100.0*activeRules[i]->evaluationTime/matchTime << tab << activeRules[i] << std::endl;
}
