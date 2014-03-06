/* element.hh
 * author: Viggo Kann (Johan Carlberger)
 * last Viggo change: 1999-09-13
 * last Johan change: 2000-03-24
 * comments: Element is a record for evaluated variable
 */

#ifndef _element_hh
#define _element_hh

class IdEntry;
class Expr;
class Rule;
class HelpRule;

/* Possible values for the variable occurrences:
   These should be in Element, but Visual C++ complains */

static const int ExactlyOne = 0; /* default */
static const int ZeroOrMore = -1; /* * */
static const int OneOrMore = -2; /* + */
static const int ZeroOrOne = -3; /* ? */

class Element {
  friend class RuleTerm;
public:
  Element() { NewObj(); }
  ~Element() { DelObj(); }
  enum elementtype { Word, Regexp };
  void Init(enum elementtype, char *regexp, IdEntry*, Expr*, int occurrences);
  bool MultiOcc() const { return (occurrences != ExactlyOne); }
  bool ExactScope() const;
  Rule *ResolveHelpRule(const Rule*);
  bool IsHelpRule() const { return helpRule != NULL; }
  HelpRule *GetHelpRule() const { return helpRule; }
  void Print(std::ostream& = std::cout) const;
  int MinScope() const;
  int MinRemainingWords() const { return minRemainingWords; }
  const IdEntry *GetIdEntry() const { return id; }
  int Occurrences() const { return occurrences; }
  const Expr *GetExpr() const { return expr; }
  Expr *GetExprToChange() const { return expr; }
  const char *Name() const;
  Expr *expr; /* used if type == Word */
private:
  enum elementtype type; /* Is this a regexp element or a word-wise element? */
  IdEntry *id;
  HelpRule *helpRule; /* pointer to help rule if the element is a help rule */
  char *regexp; /* used if type == Regexp */
  int occurrences; /* number of occurrences, see also above */
  int minRemainingWords; /* minimum no of words left in a matching of the rule (including this) */
  DecObj();
};

inline std::ostream& operator<<(std::ostream& os, const Element &e) {
  os << e.Name(); return os;
}
inline std::ostream& operator<<(std::ostream& os, const Element *e) {
  if (e) os << *e; else os << "(null Element)"; return os;
}

#endif
