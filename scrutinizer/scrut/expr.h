/* expr.hh
 * author: Viggo Kann (Johan Carlberger)
 * last Viggo change: 1999-09-10
 * last Johan change: 2000-04-27
 * comments: Expr class is a semantic record for expr
 */

#ifndef _expr_hh
#define _expr_hh

#include "evaluatedvariable.h"
#include "matching.h"
#include "operation.h"
#include "rulesettings.h"
#include "semantictype.h"

class Expr;

enum CorrOp {
  CORR_DELETE,
  CORR_INSERT,
  CORR_REPLACE,
  CORR_JOIN,
  CORR_DO_NOTHING
};

class DynamicSentence;

class CorrThing {
public:
  CorrThing(CorrOp op, WordToken* wt) : operation(op),
					wordToken(wt), nStrings(0), next(NULL) {
    NewObj();
  }
  ~CorrThing() { DelObj(); }
  void Add(Word*, const char*);
  CorrOp Operation() const { return operation; }
  WordToken *GetWordToken() const { return wordToken; }
  int NStrings() const { return nStrings; }
  Word *GetWord(int n) const { return words[n]; }
  const char *String(int n) const { return strings[n]; }
  void SetNext(CorrThing *ct) { next = ct; }
  CorrThing *Next() const { return next; }
  void Print(std::ostream& = std::cout) const;
  DynamicSentence *ds[MAX_SUGGESTIONS];
private:
  CorrOp operation;
  WordToken *wordToken;
  int nStrings;
  CorrThing *next;
  const char *strings[MAX_SUGGESTIONS];
  Word *words[MAX_SUGGESTIONS];
  DecObj();
};

inline std::ostream& operator<<(std::ostream& os, const CorrThing &ct) {
  os << &ct; return os;
}
inline std::ostream& operator<<(std::ostream& os, const CorrThing *s) {
  if (s) s->Print(os); else os << "(null CorrThing)"; return os;
}


struct CompiledRegexp { /* semantic record for compiled regexp */
  char *regexp, *compiled;
};

struct ActualMethod { /* semantic record for method or function call */
  IdEntry *id; /* pointer to id_entry for method */
  Expr *actuals; /* list of actual parameters */
};

union value { /* Values of nodes in the expression tree */
  Operation op; /* Operation */
  CompiledRegexp regexp; /* Regexp */
  IdEntry *id;    /* Leaf:Variable, Attribute */
  ActualMethod method; /* Function, Method */
  bool boolean;   /* Leaf:Boolean */
  int integer;    /* Leaf:Integer */
  double real;    /* Leaf:Real */
  const char *string;   /* Leaf:String */
  int feature;    /* Leaf:Feature, Leaf:SemFeatureClass */
  EvaluatedVariable evalVar; /* Evaluated Variable */
  CorrThing *corrThing;
  void Print(enum semantictype semtype) const;
  char *ToString(enum semantictype semtype) const;
};

class Expr { /* semantic record for expression (tree node) */
  friend class Lex;
public:
  enum nodetype {Operation, Attribute, Leaf, Function, Method, Constant};
  enum EvalEnvironment {NoEnvironment, LHSEnvironment, HelpruleEnvironment,
			ActionEnvironment, CorrectionEnvironment, MarkEnvironment};
  enum nodetype type;
  enum semantictype semtype;
  union value c;
  Expr(enum nodetype type, enum semantictype semtype); /* leaf */
  Expr(int op, enum semantictype semtype, Expr *left, Expr *right); /* op */
  Expr(int op, Expr *left, Expr *right); /* numeric op */
  int Eval(union value &res, Matching*, int elementIndex, int tokenIndex, enum EvalEnvironment env) const;
  ~Expr() { DelObj(); }
  int OptEval(bool &unknown, union value &res, int *o, int fi, Tag *ft, 
	      int ei, Tag *ct, int anchor_) const;
  void Print(std::ostream&) const { Print(); }
  void Print() const;
  int GetFeatureClass() const { return featureClass; }
  int featureClass; /* used only when semtype is Feature or SemFeatureClass */
  static Matching *CurrentMatching() { return matching; }
  static int CurrentTokenIndex() { return tokenIndex; }
  Expr *Next() const { return next; }
  void SetNext(Expr *e) { next = e; }
private:
  Expr *next;     // used to link corr-exprs
  void CorrectionEval(union value val) const;
  int Eval(union value &res) const;
  int OptEval(bool &unknown, union value &res) const;
  static Matching* matching;
  static int elementIndex;
  static int tokenIndex;
  static EvalEnvironment evalEnvironment;
  static int formerIndex;
  static int *occurrences;
  static Tag *formerTag;
  static Tag *currentTag;
  static int anchor;
public:
  static Timer::type evalTime;
  DecObj();
};

inline std::ostream& operator<<(std::ostream& os, const Expr &e) {
  e.Print(os); return os;
}
inline std::ostream& operator<<(std::ostream& os, const Expr *e) {
  if (e) os << *e; else os << "(null Expr)"; return os;
}

#endif
