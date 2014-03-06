/* rules.hh
 * author: Viggo Kann (Johan Carlberger)
 * last Viggo change: 1999-02-07
 * last Johan change: 2000-03-27
 */

#ifndef _rules_hh
#define _rules_hh

#include <stdio.h>

#include "stringbuf.h"
#include "text.h"
#include "hashtable.h"
#include "evaluatedvariable.h"
#include "element.h"
#include "semantictype.h"
#include "operation.h"
#include "expr.h"

class Scrutinizer;
class Rule;
class RuleSet;

extern StringBuf infoStringBuf;
extern StringBuf evalStringBuf; 

extern int hashcode;

RuleSet *ReadRules(Scrutinizer*, const char* ruleFile);

#define MAXNOOFFEATURECLASSES 20 /* max allowed number of feature classes */
#define MAXNOOFLEXTOKENS 10 /* max allowed number of different lex variables in the same PROB */

struct RuleElement {
  IdEntry *ruleId;   /* IdEntry for the actual help rule (in global table) */
  int elementIndex;  /* index of the ruleelement as an element in a rule */
};

class IdEntry { /* symbol record */
  friend int yyparse(void);
  friend Expr *CreateConstant(const char*, enum semantictype);
  friend IdEntry *CreateFeatureClass(const char*, int);
  friend void DefinePredefined();
  static IdEntry *firstLocalSymbol,  *lastLocalSymbol;
  IdEntry *next; /* next symbol in symbol table */
public:
  enum idtype {UndefinedId, AttributeId, MethodId, FunctionId, RuleId, 
	       ElementId, RuleElementId, LabelId, ConstantId} type;
  IdEntry(const char *name);
  IdEntry(const char *name, enum idtype, enum semantictype);
  IdEntry();
  virtual ~IdEntry() {}	    // jbfix: mem must be released in subclasses

  const char *Name() const { return name; }
  int FeatureClass() const { return u.featureClass; }
  int RuleNo() const { return u.ruleNo; }
  const struct RuleElement& Re() const { return u.re; }
  void IntoGlobalTable(enum idtype);
  void IntoLocalTable();
  int ElementIndex() const {
    if (type == ElementId) return u.elementIndex;
    else if (type == RuleElementId) return u.re.elementIndex;
    fprintf(stderr, "? ElementIndex anropat med type=%d\n", (int) type); return 0; }
  void SetElementIndex(int i) {
    if (type == ElementId) u.elementIndex = i;
    else if (type == RuleElementId) u.re.elementIndex = i;
    else fprintf(stderr, "? SetElementIndex anropat med type=%d\n", (int) type); }
  void Print() const;
  int GetRuleIndex() const { ensure(type == RuleId || type == LabelId); return u.ruleNo; }
  Element *Elt(Element *environment) const; // ElementId, element  
  static IdEntry *LookUp(const char *identifier);
  static void NewScope();
  static bool RulesOK();
  static HashTable<IdEntry> *globalsTable;
protected:
  static StringBuf stringBuf;
  enum semantictype semtype;
  char *name;
  union {
    int featureClass; /* AttributeId:FeatureClass */
    Expr *expr; /* ConstantId */
    int elementIndex; /* ElementId, index of the element in the rule */
    int ruleNo; /* RuleId and LabelId, number of rule */
    struct RuleElement re; /* RuleElementId */
  } u;
  DecObj();
};

inline std::ostream& operator<<(std::ostream& os, const IdEntry &id) {
  id.Print(/* os */); return os;
}
inline std::ostream& operator<<(std::ostream& os, const IdEntry *id) {
  if (id) os << *id; else os << "(null IdEntry)"; return os;
}

class GotoEntry { /* semantic record for GOTO and GOTO TOKEN */
public:
  IdEntry *id; /* label, where to go */
  Expr *nooftokens; /* tokens to skip in GOTO TOKEN, NULL means simple GOTO */
  GotoEntry(IdEntry *id_, Expr *nooftokens_);
  void Print() const;
  IdEntry *GetId() const { return id; }
  Expr *GetNoOfTokens() const { return nooftokens; }
};

inline std::ostream& operator<<(std::ostream& os, const GotoEntry &p) {
  os << p.GetId();
  if (p.GetNoOfTokens()) os << " " << p.GetNoOfTokens(); 
  return os;
}
inline std::ostream& operator<<(std::ostream& os, const GotoEntry *p) {
  if (p) os << *p; else os << "(null GotoEntry)"; return os;
}


class MethodOrFunctionEntry : public IdEntry { /* semantic record for methods and functions */
public:
  int noofargs; /* number of arguments */
  static const int variablenoofargs; /* = -1; * possible value of noofargs */
  enum semantictype *argtypes; /* array [0..noofargs-1] with the types of the arguments */
  union {
    void (*method)(MethodOrFunctionEntry *id /* the id of the method itself */,
	       EvaluatedVariable *p /* object (the tagged word) */, 
	       const char *t /* text (if the object is a text) */,
	       const Expr *args /* actual parameter list (expressions) */,
	       union value argval[] /* values of actual parameters */,
	       union value &res /* reference to result */);
    void (*function)(MethodOrFunctionEntry *id, const Expr *args,
			      union value argval[], union value &res);
  } func;
  int featureClass; /* used only when semtype is Feature or SemFeatureClass */

  MethodOrFunctionEntry(char *name_, enum idtype type_, 
			enum semantictype semtype_, int noofargs_);
  ~MethodOrFunctionEntry();
  int ParametersOK(Expr *actuals) const;
};

class Lex { /* used for handling of attribute lex in expr */
public:
  enum lexmode {NotInLexMode, InitLexMode, EvalLex};
  static enum lexmode mode;
  static double EvalExpr(const Expr *p);
  static void AddToken(WordToken *p);
private:
  static int currentindex; /* current index in lexTokens */
  static WordToken *lexTokens[MAXNOOFLEXTOKENS]; /* lex tokens used in current P() expression */
  static double sumprob;
  static void CheckTokensFrom(const Expr *p, int start, double prob);
};

void ParseError(const char *s); /* error reporting function used in the parser */
void ParseErrorArg(const char *s, const char *arg); /* error reporting with one argument */
void ParseWarningArg(const char *s, const char *arg); /* warning reporting with one argument */

/* Memory handling */
extern const int NEW_ELEMENTS_BUF_SIZE;
Element *NewElements(Element *p, int n);
void DeleteElements();

#endif
