/* operation.hh
 * author: Viggo Kann (Johan Carlberger)
 * last Viggo change: 1998-10-13
 * last Johan change: 1999-02-04
 * comments: Operation class is a semantic record for operation
 */

#ifndef _operation_hh
#define _operation_hh

#include <iosfwd>

class IdEntry;
class MethodOrFunctionEntry;
class GotoEntry;
class Rule;
class RuleTerm;
class Element; 
class Expr;

#ifndef TAGIDENTSYM 
#include "rules.tab.h"
#endif

class Operation {
public:
  void Init(int o, Expr *l, Expr *r) { op = o; left = l, right = r; }
  Expr* Left() const { return left; }
  Expr* Right() const { return right; }
  Expr** LeftToChange() { return &left; }
  Expr** RightToChange() { return &right; }
  int Op() const { return op; }
  bool IsUnary() const { return !right; }
  bool IsBinary() const { return right != NULL; }
  void Print(std::ostream& out) const; 
private:
  int op;       // code for operation (usually the operator itself as a character */
  Expr *left;   // left operand
  Expr *right;  // right operand
};

/*
#include "expr.h"

inline void Operation::Print(std::ostream &out) const {
  out << '[';
  if (IsUnary()) {
    out << "Unary operation: ";
    switch (Op()) {
    case NEG:	         out << "-"; break;
      //	case EXISTSYM:	out << "existerar "; break;
      //	case FORALLSYM:	out << "för alla "; break;
    case CHECKIDENTSYM:	 out << "granskningsregel "; break;
    case TAGIDENTSYM:	 out << "taggningsregel "; break;
    case LOOKUPIDENTSYM: out << "sökningsregel "; break;
    case EDITIDENTSYM:	 out << "redigeringsregel "; break;
    case HELPIDENTSYM:	 out << "hjälpregel "; break;
    case ACCEPTIDENTSYM: out << "accepteranderegel "; break;
    case '*':	         out << "markera allt "; break;
    default:	         out << (char) Op() << ' ';
    }
    out << Left();
  } else {
    // out << "Binary operation: ";
    out << Left();
    switch (Op()) {
    case NESYM:       out << "!="; break;
    case ASSIGNSYM:   out << ":="; break;
    case LESYM:	      out << "<="; break;
    case GESYM:	      out << ">="; break;
    case PROBSYM:     out << " sannolikhet för "; break;
    case IFSYM:	      out << '?'; break;
    case ELSESYM:     out << ':'; break;
    default:	      out << (char) Op() << ' ';
    }
    out << Right();
  }
  out << ']';
}
*/

inline std::ostream& operator<<(std::ostream& os, const Operation &r) {
  r.Print(os); return os;
}
inline std::ostream& operator<<(std::ostream& os, const Operation *r) {
  if (r) os << *r; else os << "(null Operation)"; return os;
}

#endif
