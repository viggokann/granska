/* matching.cc
 * author: Viggo Kann (Johan Carlberger)
 * last Viggo change: 1998-09-29
 * last Johan change: 2000-03-14
 * comments: Matching and ElementMatching classes
 */

#include "rule.h"
#include "rules.h"
#include "rulesettings.h"
#include "matching.h"
#include "sentence.h"

DefObj(Matching);
DefObj(ElementMatching);

void ElementMatching::Print(std::ostream &out) const {
  out << GetElement()->GetIdEntry()->Name() << ": \"";
  for (int i = 0; i < Occurrences(); i++)
    out << GetWordToken(i) << ' ';
  out << '"';
}

void ElementMatching::PrintParse(std::ostream& out) const {
  if (GetMatching())
    GetMatching()->PrintParse(out);
  else
    for (int i=0; i<Occurrences(); i++)
      out << GetWordToken(i)->RealString() << ' ';
}

DynamicSentence *Matching::DuplicateAltSentences() {
  DynamicSentence dummy, *s2 = &dummy;
  for (DynamicSentence *d=altSentence; d; d = d->Next())
    s2 = s2->SetNext(new DynamicSentence(d)); // new OK
  ensure(dummy.Next());
  return dummy.Next();
}

void Matching::AddAltSentences(DynamicSentence *ds) {
  if (altSentence) {
    DynamicSentence *last = NULL;
    for (DynamicSentence *d = altSentence; d; last = d, d = d->Next());
    last->SetNext(ds);
  } else
    altSentence = ds;
}

void Matching::Print(std::ostream &out) const {
  out << '(';
  GetSentence()->Print(Start(), End(), out);
  if (GetRule()) {
    out << ") "<< GetRule()->Header();
    if (GetRule()->NTerms() > 1)
      out << ", term #" << GetRuleTerm()->Number();
  } else
    std::cerr << "ERROR, " << GetRuleTerm() << " has no rule " << std::endl;
  if (GetRule()->IsHelpRule()) {
    out << '[';
    ChangeableTag *c = GetElementMatching(GetRuleTerm()->NElements()).GetHelpRuleTag();
    if (c) out << c->String();
    out << ']';
    if (xVerbose)
      for (int i=0; i < GetRuleTerm()->NElements(); i++) {
	out << std::endl;
	GetElementMatching(i).Print(out);
      }
  }
}

void Matching::PrintParse(std::ostream& out) const {
  out << GetRule()->Name(); 
  out << "( ";
  int i;
  for (i=0; i<GetRuleTerm()->NElements(); i++) {
    GetElementMatching(i).PrintParse(out);
    out << ' ';
  }
  out << ')';
}

