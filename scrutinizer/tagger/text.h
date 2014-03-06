/* text.hh
 * author: Johan Carlberger
 * last change: 2000-03-23
 * comments: Text class
 */

#ifndef _text_hh
#define _text_hh

#include "basics.h"
#include <iostream>

class Matching;
class Sentence;
class Word;
class WordToken;

class Text {
  friend class Tagger;
public:
  Text() : firstSentence(NULL) { NewObj(); }
  ~Text();
  void Reset();
  int NSentences() const { return nSentences; }
  int NWordTokens() const { return nWordTokens; }
  int NNewWords() const { return nNewWords; }
  Sentence* FirstSentence() const { return firstSentence; }
  const WordToken *GetWordTokenInPos(int) const;
  void Print(std::ostream& = std::cout) const;
  void CountContents();
private:
  void delete_sentences(); // jonas
  int nSentences;
  int nWordTokens;       // not including sentence-delimiters // jonas, actually, it is including ...
  int nNewWords;
  Sentence *firstSentence;
  DecObj();
};

inline std::ostream& operator<<(std::ostream& os, const Text &t) {
  os <<"text with "<<t.NSentences()<<" sentences and "
     <<t.NWordTokens()<<" word-tokens";
  return os;
}

inline std::ostream& operator<<(std::ostream& os, const Text *t) {
  if (t) os << *t; else os << "(null Text)";
  return os;
}

#endif
