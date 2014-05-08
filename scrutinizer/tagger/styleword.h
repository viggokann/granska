/* styleword.hh
 * author: Johan Carlberger
 * last change: 2000-03-23
 * comments: StyleWord class
 *           can't be loaded fast, please implement
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

#ifndef _styleword_hh
#define _styleword_hh

#include <iostream>
#include "tag.h"
#include "word.h"

// these should be in StyleWord:
static const int MAX_STYLES = 20;      // can be set arbitrarily
static const int MAX_ALTERNATIVES = 3;
static const uint MAX_PARAGRAPH = 10;

class StyleWord {
  friend class WordLexicon;
public:
  StyleWord();
  ~StyleWord() { DelObj(); }
  StyleWord(const Word *w) : word(w) { NewObj(); } // just for FindSyleWord()
  const Word* GetWord() const { return word; }
  const WordTag* GetWordTag() const { return wordTag; }
  const char* Paragraph() const { return paragraph; }
  const char* Comment() const { return comment; }
  int NAlternatives() const { return nAlts; }
  const Word *Alternative(int n) const { ensure(n < nAlts); return alt[n]; }
  uint Style() const { return style; }
  void Print(std::ostream&) const;
private:
  const Word *word;
  const WordTag *wordTag;
  char paragraph[MAX_PARAGRAPH];
  char *comment;
  Word *alt[MAX_ALTERNATIVES];
  bool  owns_mem[MAX_ALTERNATIVES];
  int nAlts;
  uint style;
  static char styles[MAX_STYLES][MAX_PARAGRAPH];
  DecObj();
};

inline StyleWord::StyleWord() {
  word = NULL;
  wordTag = NULL;
  style = 0;
  paragraph[0] = '\0';
  paragraph[MAX_PARAGRAPH-1] = '\0';
  comment = NULL;
  for (int i=0; i<MAX_ALTERNATIVES; i++)
  {
    alt[i]	= NULL;
    owns_mem[i] = false;
  }
  nAlts = 0;
  NewObj();
}

inline void StyleWord::Print(std::ostream& os) const {
  os << word <<' '<< wordTag->GetTag() <<' '<< style<< " [";
  uint mask = 1;
  for (int i=0; i<MAX_STYLES; i++, mask <<= 1)
    if (mask & style) 
      os << styles[i] << '.';
  os << "] " << paragraph;
  if (alt[0]) {
    os <<" (" << alt[0];
    for (int j=1; j<MAX_ALTERNATIVES; j++)
      if (alt[j])
	os << ", " << alt[j];
    os << ')';
  }
  if (comment)
    os <<" \""<< comment << "\"";
}

inline int CompareStyleWords(const StyleWord &w1, const StyleWord &w2) {
  return w1.GetWordTag() == w2.GetWordTag();
}
inline uint KeyStyleWord(const StyleWord &w) {
  return KeyWord(*(w.GetWord()));
}
inline std::ostream& operator<<(std::ostream& os, const StyleWord &w) {
  w.Print(os);
  return os;
}
inline std::ostream& operator<<(std::ostream& os, const StyleWord *w) {
  if (w) os << *w;
  else os << "(null StyleWord)";
  return os;
}

#endif

