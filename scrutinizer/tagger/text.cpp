/* text.cc
 * author: Johan Carlberger
 * last change: 2000-03-23
 * comments: Text class
 */

#include "sentence.h"
#include "text.h"

DefObj(Text);

// jonas, remove all sentences here, instead of using recursive delete
void Text::delete_sentences() {
  Sentence *next, *cur = firstSentence;
  while(cur) {
    next = cur->next;
    delete cur;
    cur = next;
  }
}

Text::~Text() {
  //jonas  if (firstSentence) delete firstSentence;
  delete_sentences();
  DelObj();
}

void Text::Print(std::ostream& out) const {
  for (const Sentence *s=FirstSentence(); s; s=s->Next())
    s->Print(out);
}

void Text::CountContents() {
  nSentences = 0;
  nWordTokens = 0;
  nNewWords = 0;
  for (const Sentence *s = FirstSentence(); s; s=s->Next()) {
    nSentences++;
    nWordTokens += s->NWords();
    for (int i=2; i<s->NWords()+2; i++) {
      if (s->GetWord(i)->IsNewWord())
	nNewWords++;
    }
  }
}

const WordToken *Text::GetWordTokenInPos(int pos) const {
  const Sentence *prev = NULL;
  for (const Sentence *s = FirstSentence(); s; s=s->Next()) {
    if (s->GetWordToken(2)->Offset() > pos)
      break;
    prev = s;
  }
  if (prev)
    for (int i=2; i<prev->NWords()+2; i++)
      if (prev->GetWordToken(i)->Offset() <= pos &&
	  prev->GetWordToken(i+1)->Offset() > pos)
	return prev->GetWordToken(i);
  return NULL;
}

void Text::Reset() {
  Message(MSG_STATUS, "resetting text...");
  for (const Sentence *s = FirstSentence(); s; s=s->Next())
    for (int i=2; i<s->NTokens()-2; i++)
      s->GetWord(i)->Reset();
  //jonas  if (firstSentence) delete firstSentence;
  //jonas firstSentence = NULL;
}
