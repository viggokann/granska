/* wordtag.hh
 * author: Johan Carlberger
 * last change: 2000-03-14
 * comments: WordTag class
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

#ifndef _wordtag_hh
#define _wordtag_hh

#include <iostream>
#include "inflectrule.h"
#include "message.h"
#include "tag.h"

enum VerbType {
  VERB_NO_TYPE,
  VERB_INTRANSITIVE,
  VERB_BITRANSITIVE,
  NOUN_FEMININ
};

class WordLexicon;
class Tag;

class WordTag {
  friend class Tagger;
  friend class DevelopersTagger;
  friend class Lexicon;
  friend class WordLexicon;
  friend class NewWordLexicon;
  friend class MorfLexicon;
  friend class Word;
  friend class NewWord;
public:
  void Reset();
  void Init(WordTag *next, bool isWord);
  float Cwt_Ct() const { return float(TagFreq())*GetTag()->FreqInv(); }
  const WordTag* Next() const { return (next->IsWord() ? NULL : next); }
  WordTag* Next() { return (next->IsWord() ? NULL : next); }
  Word* GetWord() const;
  const char* String() const;
  Tag* GetTag() const { return Tag::tags[tagIndex]; }
  float LexProb() const { return lexProb; }
  uchar TagIndex() const { return tagIndex; }
  bool HasStyleRemark() const { return style; }
  const int NLemmas() const { return lemma ? nExtraLemmas+1 : 0; }
  WordTag* Lemma(int n) const;
  bool IsLemma() const { return lemma == this; }
  bool IsExtraLemma() const { return extraLemma; }
  bool IsExtraWordTag() const { return extraWordTag; }
  bool IsWord() const { return isWord; }
  bool IsAddedByTagger() const { return addedByTagger; }
  int VerbType() const { return verbtype; }
  int TagFreq() const { return tagFreq; }
  uint InflectRule(int n) const;
  void GuessInflectRule();
  int NInflectRules() const { return (inflectRule==INFLECT_NO_RULE) ? 0 : nExtraInflectRules+1; }
  WordTag* GetForm(const Tag*, int lemmaNumber, int ruleNumber) const;
  int StemLen() const;
  void Print(std::ostream& = std::cout) const;
  void PrintTag(std::ostream& = std::cout) const;
  void PrintInfo(std::ostream& = std::cout) const;
protected:
  WordTag();
  ~WordTag() { DelObj(); }
  static WordLexicon *words;
  float lexProb;            // lexical prob, C(w, t) / C(t) right now
  int tagFreq;
  WordTag *next;
  WordTag *lemma;
  uchar tagIndex;
  Bit inflectRule : 9;
  Bit extraWordTag : 1;
  Bit extraLemma : 1;
  Bit style : 1;
  Bit nExtraLemmas : 1;
  Bit nExtraInflectRules : 2;
  Bit isWord : 1;
  Bit addedByTagger :1;
  Bit verbtype : 2;
  // 5 bits free
  DecObj();
};

std::ostream& operator<<(std::ostream&, const Word*);
std::ostream& operator<<(std::ostream&, const Tag*);

// jbfix: bit field initiated in init list to silent purify
inline WordTag::WordTag()
  : lexProb(0),
    tagFreq(0),
    next(NULL),
    lemma(NULL),
    tagIndex(TAG_INDEX_NONE),
    inflectRule(INFLECT_NO_RULE),
    extraWordTag(0),
    extraLemma(0),
    style(0),
    nExtraLemmas(0),
    nExtraInflectRules(0),
    isWord(false),
    addedByTagger(0),
    verbtype(0)
{
    NewObj();
}


inline std::ostream& operator<<(std::ostream& os, const WordTag &wt) {
  os << wt.GetWord() << ' ' << wt.GetTag(); return os;
}

inline std::ostream& operator<<(std::ostream& os, const WordTag *wt) {
  if (wt) os << *wt; else os << "(null WordTag)";
  return os;
}

inline void WordTag::Init(WordTag *n, bool isW) {
  next = n;
  isWord = isW;
}

inline void WordTag::Reset() {
  tagIndex = TAG_INDEX_NONE;
  lemma = NULL;
  inflectRule = INFLECT_NO_RULE;
  tagFreq = 0;
  lexProb = 0;
  addedByTagger = 0;
  verbtype = 0;
}

inline Word* WordTag::GetWord() const {
  const WordTag *wt=this;
  while(!wt->IsWord()) {
    wt=wt->next;
#ifdef ENSURE
    if (wt == this)
      Message(MSG_ERROR, "infinite loop in WordTag::GetWord()");
#endif
  }
  return (Word*) wt;
}

#endif
