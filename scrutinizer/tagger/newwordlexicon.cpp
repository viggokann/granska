/* newwordlexicon.cc
 * author: Johan Carlberger
 * last change: 2000-05-08
 * comments: NewWordLexicon class
 */

#include "newwordlexicon.h"
#include "hashtable.h"

DefObj(NewWordLexicon);
DefObj(HashTable<NewWord>);

NewWordLexicon::NewWordLexicon() {
  Init(KeyNewWord, CompareNewWords, KeyWordString, CompareStringAndNewWord);
  NewObj();
}

NewWordLexicon::~NewWordLexicon() {
  Message(MSG_STATUS, "deleting newwordlexicon...");
  DeleteAndClear();
  NewWord::ResetStrings();
  DelObj();
}

NewWord *NewWordLexicon::AddWord(const char* s, const Tag *t) {
  //  if (Find(s))
  //   Message(MSG_WARNING, "adding word twice", s);
  NewWord *w = new NewWord(s); // new OK
  Insert(w);
  if (t)
    AddWordTagUnsafe(w, t);
  else
    ensure(w->tagIndex == TAG_INDEX_NONE);
  return w;
}

WordTag *NewWordLexicon::AddWordTag(NewWord *w, const Tag *tag) {
  WordTag *wt = w->GetWordTag(tag);
  if (wt) return wt;
  return AddWordTagUnsafe(w, tag);
}

WordTag *NewWordLexicon::AddWordTagUnsafe(NewWord *w, const Tag *tag) {
  if (!tag->IsContent()) {
    Message(MSG_MINOR_WARNING, "adding non-content tag", tag->String(),
	    "to new-word", w->String());
  }
  WordTag *wt;
  if (w->tagIndex == TAG_INDEX_NONE)
    wt = w;
  else {
    wt = new WordTag(); // new OK
    wt->Init(w->next, false);
    w->next = wt;
  }
  wt->tagIndex = tag->Index();
  return wt;
}

void NewWordLexicon::Reset() {
  Message(MSG_STATUS, "resetting newwordlexicon...");
  DeleteAndClear();
  NewWord::ResetStrings();
}
