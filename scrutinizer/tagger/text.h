/* text.hh
 * author: Johan Carlberger
 * last change: 2000-03-23
 * comments: Text class
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
