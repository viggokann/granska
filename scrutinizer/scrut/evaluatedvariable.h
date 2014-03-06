/* evaluatedvariable.hh
 * author: Viggo Kann (Johan Carlberger)
 * last Viggo change: 1998-10-13
 * last Johan change: 2000-03-25
 * comments: EvaluatedVariable is a record for evaluated variable
 */

#ifndef _evaluatedvariable_hh
#define _evaluatedvariable_hh

#include "matching.h"

class EvaluatedVariable {
  friend class Expr;
public:
  //  EvaluatedVariable() { NewObj(); } cant do
  //  ~EvaluatedVariable() { DelObj(); }
  inline void Init(bool lex, int index, const ElementMatching*);
  bool IsIndexed() const { return index >= 0; }   // is the variable indexed?
  inline Tag *GetTag() const; // tag of the token of the matching
  inline const Element *GetElement() const { return GetElementMatching()->GetElement(); }
  const char *LexString() const;   // the string corresponding to the matching
  const char *RealString() const;   // the string corresponding to the matching
  void Mark();
  bool IsLexical() const { return !!lex; }
  int Index() const { ensure(IsIndexed()); return index; }
  const ElementMatching *GetElementMatching() const { return elementMatching; }
  int NMatchedWordTokens() const;
  WordToken *GetWordToken(int) const;
private:
  char lex;                                 /* is it a .lex variable? */
  short index;                              /* -1 if not an indexed variable, otherwise the index */
  const ElementMatching *elementMatching;   /* the matching (token-element) */
  //  DecObj();
};

inline void EvaluatedVariable::Init(bool lex_, int index_, const ElementMatching *matching_) {
  lex = lex_; 
  index = index_; 
  elementMatching = matching_;
}

/* GetTag returns the tag of the token of the matching */
inline Tag *EvaluatedVariable::GetTag() const { 
  if (IsIndexed())
    return GetElementMatching()->GetWordToken(Index())->SelectedTag();
  return GetElementMatching()->GetTag();
}

inline int EvaluatedVariable::NMatchedWordTokens() const {
  if (!IsIndexed() && (GetElement()->MultiOcc() || GetElement()->IsHelpRule()))
    return GetElementMatching()->Occurrences();
  return 1;
}

inline WordToken *EvaluatedVariable::GetWordToken(int n=0) const { 
  return (IsIndexed() ? GetElementMatching()->GetWordToken(Index()) :
	  GetElementMatching()->GetWordToken(n));
}

#endif

