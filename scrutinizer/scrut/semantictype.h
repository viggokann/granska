/* sementictype.hh
 * author: Viggo Kann (Johan Carlberger)
 * last Viggo change: 1999-06-26
 * last Johan change: 2000-02-23
 * comments: Semantic types used
 */

#ifndef _semantictype_hh
#define _semantictype_hh

enum semantictype {
  NoType, AnyType,
  Boolean, Integer, Real, String, Regexp, Variable, Interval, WordTags, CorrT,
  Feature, SemFeatureClass, 
  LexVariable,
  TagAction, CheckAction, LookUpAction, EditAction,
  HelpAction, AcceptAction, NSemanticTypes, SemanticError 
}; 

extern enum semantictype matrix[NSemanticTypes][NSemanticTypes];
inline enum semantictype CoerseTypes(enum semantictype s1, enum semantictype s2) { return matrix[s1][s2]; }

inline bool IsNumber(enum semantictype semtype) { 
  return semtype == Integer || semtype == Real; 
}

inline bool IsFeature(enum semantictype semtype) { 
  return semtype == SemFeatureClass || semtype == Feature; 
}

inline enum semantictype HighestType(enum semantictype t1, enum semantictype t2) {
  if (t1 == Real && t2 == Integer) return Real;
  if (t2 == Real && t1 == Integer) return Real;
  return t1;
}

inline std::ostream &operator<<(std::ostream &out, enum semantictype s) {
  switch(s) {
  case Boolean:	out << "boolean"; break;
  case Integer:	out << "integer"; break;
  case Real:	out << "real"; break;
  case String:	out << "string"; break;
  case Variable:      out << "variable"; break;
  case LexVariable:	out << "lex-variable"; break;
  case Interval:	out << "interval"; break;
  case Feature:	out << "feature"; break;
  case WordTags:	out << "word-tag list"; break;
  case SemFeatureClass:	out << "feature class";
  case AnyType: out << "any type"; break;
  default: out << "unknown semantic type";
  }
  return out;
}

/*
void PrintSemanticTypeAndValue(enum semantictype semtype, void *v) const {
  switch (semtype) {
  case Boolean:	printf("%s", (bool) *v == 0 ? "false" : "true"); break;
  case Integer:	printf("%d", (int) *v); break;
  case Real:	printf("%f", (float) *v); break;
  case String:	printf("\"%s\"", (char*) *v); break;
  case Regexp:	printf("\'%s\'", *v); break;
  case Variable:	((IdEntry*)v)->Print(); break;
  case LexVariable:	printf("lex-variabel"); break;
  case Interval:	printf("intervall"); break;
  case Feature:	printf("%s: särdrag", taggertags->GetFeature(*v).Name()); break;
  case SemFeatureClass:	printf("särdragsklass (borde inte uppkomma)"); break;
  default: printf("okänd semantisk typ");
  }
}
*/

#endif
