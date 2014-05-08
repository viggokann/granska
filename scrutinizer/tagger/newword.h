/* newword.hh
 * author: Johan Carlberger
 * last change: 2000-05-08
 * comments: NewWord class
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

#ifndef _newword_hh
#define _newword_hh

#include "word.h"

class WordToken;

const int MAX_WORD_TAG_SUFFIXES = 8;

class NewWord : public Word {
  friend class Tagger;
  friend class DevelopersTagger;
  friend class Lexicon;
  friend class WordLexicon;
  friend class NewWordLexicon;
  friend class InflectLexicon;
  friend int wt2wwt(int);
  friend int wt2ww(int);
public:
  explicit NewWord(const char*);
  ~NewWord() {
    //    std::cout << "deleting " << String() << std::endl;
    DelObj();
    WordTag *nextWt = NULL;
    for (WordTag *wt=Next(); wt; wt=nextWt) {
      nextWt = wt->Next();
      delete wt;
    }
  }
  void Init();
  void PrintInfo(std::ostream& = std::cout) const;
  int NSuffixes() const { return nSuffixes; }
  const WordTag* Suffix(int n) const { return suffix[n]; }
  WordTag* Suffix(int n) { return (WordTag*) suffix[n]; }
  bool AddSuffixes(const Word *w);
  bool IsCompound() const { return NSuffixes() > 0; }
  bool IsAnalyzed() const { return isAnalyzed; }
  bool IsCompoundAnalyzed() const { return isCompoundAnalyzed; }
  bool IsDerived() const { return isDerived; }
  bool IsAlwaysCapped() const { return isAlwaysCapped; }
  void Reset();
  void ComputeLexProbs();
private:
  NewWord() { isWord = 1; next = this; NewObj(); }
  static void ResetStrings() { stringBuf.Reset(); }
  const WordTag *suffix[MAX_WORD_TAG_SUFFIXES];
  unsigned int nSuffixes;
  Bit isAnalyzed : 1;
  Bit isCompoundAnalyzed : 1;
  Bit isDerived : 1;
  Bit isAlwaysCapped : 1;
  // 21 bits free
  DecObj();
};

inline std::ostream& operator<<(std::ostream& os, const NewWord &w) {
  os << w.String(); return os;
}

// jbfix: bit field initiated in init list to silent purify
inline NewWord::NewWord(const char *s)
  : nSuffixes(0),
    isAnalyzed(0),
    isCompoundAnalyzed(0),
    isDerived(0),
    isAlwaysCapped(0)
{
    NewObj();
    ensure(s);
    Init();
    Set(s);
}

inline void NewWord::Init() {
  Word::Init();
  WordTag::Init(this, true);
  nSuffixes = 0;
  newWord = 1;
  isForeign = 1;
  isSpellOK = 0;
  isAnalyzed = 0;
  isCompoundAnalyzed = 0;
  isDerived = 0;
  isAlwaysCapped = 0;
  mayBeCapped = 1;
  compoundBeginOK = compoundEndOK = 1;
}

inline void NewWord::Reset() {
  WordTag::Reset();
  next = this;
  isAnalyzed = 0;
  isCompoundAnalyzed = 0;
  isDerived = 0;
  nSuffixes = 0;
}

inline bool NewWord::AddSuffixes(const Word *w) {
  for (const WordTag *wt = w; wt; wt=wt->Next())
    if (!wt->GetTag()->IsCompoundStop()) {
      if (NSuffixes() >= MAX_WORD_TAG_SUFFIXES) {
	Message(MSG_MINOR_WARNING, "too many suffixes for word", String());
	return false;
      }
      suffix[nSuffixes++] = wt;
    }
  return true;
}
 
inline void NewWord::PrintInfo(std::ostream& out) const {
  Word::PrintInfo();
  out << '*';
  if (IsDerived())
    out << 'd';
  if (NSuffixes() > 0 )
    out << " -" << Suffix(0)->String();
}

inline uint KeyNewWord(const NewWord &w) {
  return Hash(w.String());
}
inline int CompareNewWords(const NewWord &w1, const NewWord &w2) {
  return strcmp(w1.String(), w2.String());
}
inline int CompareStringAndNewWord(const char *s, const NewWord &w2) {
  return strcmp(s, w2.String());
}

#endif

