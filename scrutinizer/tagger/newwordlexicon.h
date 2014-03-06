/* newwordlexicon.hh
 * author: Johan Carlberger
 * last change: 2000-05-15
 * comments: NewWordLexicon class
 */

#ifndef _newwordlexicon_hh
#define _newwordlexicon_hh

#include "newword.h"
#include "hashtable.h"

class NewWordLexicon : public HashTable<NewWord> {
public:
  NewWordLexicon();
  ~NewWordLexicon();
  NewWord *AddWord(const char*, const Tag* = NULL);
  WordTag *AddWordTag(NewWord*, const Tag*);
  WordTag *AddWordTagUnsafe(NewWord*, const Tag*);
  void Reset();
  DecObj();
};

#endif

