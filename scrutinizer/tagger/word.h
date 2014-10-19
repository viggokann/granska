/* word.hh
 * author: Johan Carlberger
 * last change: 2000-05-08
 * comments: class Word
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

#ifndef _word_hh
#define _word_hh

#include <iostream>
#include <string.h>
#include "hashscatter.h"
#include "inflectrule.h"
#include "message.h"
#include "tag.h"
#include "wordtag.h"
#include "stringbuf.h"

// this should be in Word:
const uint MAX_WORD_LENGTH = 150;      // can be set arbitrarily
const int MAX_LEMMAS_PER_WORDTAG = 2;
const int MAX_INFLECTION_CHARS_ON_NORMAL_WORD = 5;
const int MIN_PREFIX_CHARS_ON_NORMAL_WORD = 3;

class WordLexicon;
//class WordToken;

class Word : public WordTag {
  friend class Corrector;
  friend class Predictor;
  friend class Tagger;
  friend class DevelopersTagger;
  friend class Lexicon;
  friend class WordLexicon;
  friend class MorfLexicon;
  friend class NewWordLexicon;
  friend class WordSuggestions;
  friend class InflectLexicon;
  friend int wt2wwt(int);
  friend int wt2ww(int);
  friend int selectMoreForms();
public:
  explicit Word(const char* s = 0);
  ~Word() { DelObj(); ExtByt(sizeof(WordTag)); }
  void Init();
  void Reset() { textFreq = 0; someFormOccursInText = 0; }
  const char* String() const { return string; }
  uint StringLen() const { return strLen; }
  bool IsAmbiguous() const { return Next() != NULL; }
  WordTag* GetWordTag(const Tag*) const;
  WordTag* GetWordTag(const Tag*);
  const WordTag* GetWordTag(int tagIndex) const;
  bool IsNewWord() const { return newWord; }
  int Freq() const { return freq; }
  int TextFreq() const { return textFreq; }
  bool IsExtra() const { return extra; }
  bool HasExtraWordTag() const { return hasExtraWordTag; }
  bool HasStyleRemark() const { return style; }
  bool IsCompoundEndOK() const { return compoundEndOK; }
  bool IsCompoundBeginOK() const { return compoundBeginOK; }
  bool IsSuggested() const { return suggested; }
  bool IsOptSpace() const { return optSpace; }
  bool IsRuleAnchor() const { return isRuleAnchor; }
  bool IsForeign() const { return isForeign; }
  bool IsCollocation1() const { return collocation1; }
  bool IsCollocation23() const { return collocation23; }
  bool SomeFormOccursInText() const { return someFormOccursInText; }
  void ComputeLexProbs();
  void Print(std::ostream& out = std::cout) const;
  void PrintTags(std::ostream& out = std::cout) const;
  void PrintInfo(std::ostream& = std::cout) const;
  void SetRuleAnchor(bool b) { isRuleAnchor = b; }
  bool IsTesaurus() const { return tesaurus; }
  bool IsSpellOK() const { return isSpellOK; }
  bool MayBeCapped() const { return mayBeCapped; }

protected:
  void Set(const char *);

protected:
  static StringBuf stringBuf;
  char *string;
  int freq;
  uchar textFreq;
  uchar strLen;
  Bit extra : 1;
  Bit hasExtraWordTag : 1;
  Bit newWord : 1;
  Bit compoundEndOK: 1;
  Bit compoundBeginOK: 1;
  Bit suggested: 1;
  Bit someFormOccursInText : 1;
  Bit style : 1;
  Bit optSpace : 1;
  Bit isRuleAnchor : 1;
  Bit isForeign : 1;
  Bit collocation1 : 1;
  Bit collocation23 : 1;
  Bit tesaurus : 1;
  Bit isSpellOK : 1;
  Bit mayBeCapped : 1;
  DecObj();
};

inline std::ostream& operator<<(std::ostream& os, const Word &w) {
  os << w.String(); return os;
}

inline std::ostream& operator<<(std::ostream& os, const Word *w) {
  if (w) os << *w; else os << "(null Word)";
  return os;
}

inline void Word::Init() 
{
    WordTag::Init(this, true);
}

inline WordTag* Word::GetWordTag(const Tag *t) const {
  for (const WordTag *q=this; q; q=q->Next())
    if (q->GetTag() == t)
      return (WordTag*) q;
  return NULL;
}

inline const WordTag* Word::GetWordTag(int index) const {
  for (const WordTag *q=this; q; q=q->Next())
    if (q->TagIndex() == index)
      return q;
  return NULL;
}

inline WordTag* Word::GetWordTag(const Tag *t) {
  for (WordTag *q=this; q; q=q->Next())
    if (q->GetTag() == t)
      return q;
  return NULL;
}

// jbfix: bit field initiated in init list to silent purify
inline Word::Word(const char *s)
  : string(0),
    freq(0),
    textFreq(0),
    strLen(0),
    extra(0),
    hasExtraWordTag(0),
    newWord(0),
    compoundEndOK(1),
    compoundBeginOK(0),
    suggested(0),
    someFormOccursInText(0),
    style(0),
    optSpace(0),
    isRuleAnchor(0),
    isForeign(0),
    collocation1(0),
    collocation23(0),
    tesaurus(0),
    isSpellOK(1),
    mayBeCapped(0)
{
    if(s)
    {
	Init();
	Set(s);
    }
    else    // jb: behaviour from original version, needs a fix?
    {
	NewObj();
	ExtByt(-sizeof(WordTag));
    }
}

// jbfix: Word leaked its string memory
inline void Word::Set(const char *s)
{
  uint len = strlen(s);
  string = stringBuf.NewString(len+1);
  strcpy(string, s);
  strLen = (uchar) len;
}

inline static int CompareWordPointers(const void *w1, const void *w2) {
  return strcmp((*(Word**)w1)->String(), (*(Word**)w2)->String());
}
inline int CompareWordPointersByStringLength(const void *w1, const void *w2) {
  return (*(Word**)w1)->StringLen() - (*(Word**)w2)->StringLen();
}
inline int CompareWords(const Word &w1, const Word &w2) {
  return strcmp(w1.String(), w2.String());
}
inline int CompareStringAndWord(const char *s, const Word &w2) {
  return strcmp(s, w2.String());
}
inline int RankWords(const Word &w1, const Word &w2) {
  return w1.Freq() - w2.Freq();
}
inline uint KeyWord(const Word &w) {
  return Hash(w.String());
}
inline uint KeyWordString(const char *s) {
  return Hash(s);
}

#endif


