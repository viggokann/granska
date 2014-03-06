/* operation.cc
 * author: Viggo Kann (Johan Carlberger)
 * last Viggo change: 1998-10-13
 * last Johan change: 2000-02-15
 * comments: Operation class is a semantic record for operation
 */

#include "expr.h"
#include "operation.h"

void Operation::Print(std::ostream &out) const {
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
    case NESYM:       out << " != "; break;
    case ASSIGNSYM:   out << " := "; break;
    case LESYM:	      out << " <= "; break;
    case GESYM:	      out << " >= "; break;
    case PROBSYM:     out << " sannolikhet för "; break;
    case IFSYM:	      out << '?'; break;
    case ELSESYM:     out << ':'; break;
    default:	      out << ' ' << (char) Op() << ' ';
    }
    out << Right();
  }
  out << ']';
}
