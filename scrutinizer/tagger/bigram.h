/* bigram.hh
 * author: Johan Carlberger
 * last change: 990806
 * comments: Bigram classes
 */

/******************************************************************************

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

******************************************************************************/

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

