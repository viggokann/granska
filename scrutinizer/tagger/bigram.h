/* bigram.hh
 * author: Johan Carlberger
 * last change: 990806
 * comments: Bigram classes
 */

#ifndef _bigram_hh
#define _bigram_hh

#include "mainword.h"

class Bigram {
public:
  const Word *word1;
  const Word *word2;
  float prob;
};

inline int CompareBigrams(const Bigram &b1, const Bigram &b2) {
  return !(b1.word1 == b2.word1 && b1.word2 == b2.word2);
}

inline uint KeyBigram(const Bigram &b) {
  return Hash(b.word1->String()) ^ Hash(b.word2->String());
}

inline std::ostream& operator<<(std::ostream& os, const Bigram &b) {
  os << b.word1->String() << ' ' << b.word2->String();
  return os;
}

inline std::ostream& operator<<(std::ostream& os, const Bigram *b) {
  if (b) os << *b; else os << "(null Bigram)";
  return os;
}

#endif

