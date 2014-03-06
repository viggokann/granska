/* expr.cc
 * author: Viggo Kann (Johan Carlberger)
 * last Viggo change: 1999-09-10
 * last Johan change: 2000-04-27
 * comments: Expr class is a semantic record for expr
 */

#include "expr.h"
#include "rules.h"
#include "scrutinizer.h"
#include "memhandler.h"

DefObj(CorrThing);
DefObj(Expr);
Timer::type Expr::evalTime;

extern Scrutinizer *scrutinizer;


void CorrThing::Add(Word *w, const char *s) {
  //  std::cout << w << ' ' << s << std::endl;
  for (int i=0; i<nStrings; i++)
    if (!strcmp(s, strings[i]))
      return;
  if (nStrings >= MAX_SUGGESTIONS) {
    Message(MSG_MINOR_WARNING, "too many strings in Corr");
    return;
  }
  if (!w) {
    if (operation != CORR_JOIN)
      w = scrutinizer->FindMainOrNewWordAndAddIfNotPresent(s);
  } else if (w->IsNewWord())
    scrutinizer->Words().AnalyzeNewWord((NewWord*)w);
  words[nStrings] = w;
  strings[nStrings] = s;
  nStrings++;
}

void CorrThing::Print(std::ostream &out) const {
  out << GetWordToken() << ": ";
  switch(Operation()) {
  case CORR_DELETE: out << "delete "; break;
  case CORR_INSERT: out << "insert "; break;
  case CORR_REPLACE: out << "replace "; break;
  case CORR_JOIN: out << "join "; break;
  case CORR_DO_NOTHING: out << "do nothing "; break;
  }
  for (int i=0; i<NStrings(); i++)
    out << '[' << GetWord(i) << ' ' << String(i) << ']';
}

Expr::Expr(enum nodetype type_, enum semantictype semtype_) :
  type(type_),
  semtype(semtype_),
  next(NULL) 
{
  NewObj();

  Handle_memory(this);
}

Expr::Expr(int op, enum semantictype semtype_, Expr *left, Expr *right) :
  type(Operation),
  semtype(semtype_),
  next(NULL) 
{
  c.op.Init(op, left, right);
  NewObj();

  Handle_memory(this);
}

/* Make numeric op */
Expr::Expr(int op, Expr *left, Expr *right) :
  type(Operation),
  next(NULL)
{
  if (!IsNumber(left->semtype) || !IsNumber(right->semtype)) {
    char s[10];
    if (op == LESYM) strcpy(s, "<="); else
      if (op == GESYM) strcpy(s, ">="); else
	sprintf(s, "%c", op);
    ParseErrorArg("Båda operanderna till %s ska vara tal.", s);
  }
  semtype = HighestType(left->semtype, right->semtype);
  c.op.Init(op, left, right);
  NewObj();

  Handle_memory(this);
}

void Expr::Print() const {
  switch (type) {
  case Operation:
    std::cout << c.op;
    break;
  case Constant:  printf("konstant (fel, ska inte uppkomma)"); break;
  case Leaf:      c.Print(semtype); break;
  case Function:  c.id->Print(); break;
  case Method:    c.id->Print(); break;
  case Attribute: c.id->Print(); break;
  default: printf("okänd typ på uttrycksnod");
  }
}
