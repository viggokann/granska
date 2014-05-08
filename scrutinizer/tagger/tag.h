/* tag.hh
 * author: Johan Carlberger
 * last change: 2000-05-08
 * comments: Tag class
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

#ifndef _tag_hh
#define _tag_hh

#include <string.h>
#include "basics.h"
#include "bitmap.h"
#include "feature.h"
#include "hashscatter.h"
#include "message.h"
#include "settings.h"
#include "tagdefines.h"

class TagLexicon;
class Word;
class RuleTerm;

class Tag {
  friend class Corrector;
  friend class Lexicon;
  friend class DevelopersTagger;
  friend class TagLexicon;
  friend class WordLexicon;
  friend class WordTag;
  friend class InflectLexicon;
  friend class Tagger;
  friend class MorfLexicon;
  friend class RuleTerm;
  friend class MatchingSet;
  friend int wt2wwt(int);
  friend int selectMoreForms();
  friend int freqstatmorf(uint, uint);
public:
  Tag() : lemmaIndex(TAG_INDEX_NONE), lemmaIndex2(TAG_INDEX_NONE), ruleBase(false) {
    NewObj(); }
  explicit Tag(const char* s) {
    strcpy(string, s); Init(); sentenceDelimiter = silly = properNoun = false;
    lemmaIndex = lemmaIndex2 = TAG_INDEX_NONE;
    NewObj(); }
  virtual ~Tag() { /*std::cout << "del " << String() << std::endl;*/ DelObj(); }
  const char* String() const { return string; }
  float LexProb() const { return lexProb; }; // temp storage of probs
  uchar Index() const { return index; }
  int Frq() const { return frq; }
  float FreqInv() const { return freqInv; }
  int Members() const { return members; }
  bool IsSilly() const { return silly; }
  bool IsContent() const { return content; }
  bool IsCompoundStop() const { return compoundStop; }
  bool IsPunctuationOrEnder() const { return punctuationOrEnder; }
  bool IsSentenceDelimiter() const { return sentenceDelimiter; }
  bool IsProperNoun() const { return properNoun; }
  bool IsGenitive() const { return strstr(String(), "gen") != NULL; }
  bool IsLemma() const { return LemmaTag() == this; }
  bool IsLemma2() const { return LemmaTag2() == this; }
  bool IsAdjective() const { return !strncmp(String(), "jj", 2); }
  bool IsNoun() const { return !strncmp(String(), "nn", 2); }
  bool IsVerb() const { return !strncmp(String(), "vb", 2); }
  bool IsSms() const { return strstr(String(), "sms") != NULL; }
  float UniProb() const { return uniProb; }
  int FeatureValue(int fc) const {
    ensure(fc>=0 && fc<=MAX_CLASSES); return featureValue[fc]; }
  int WordClass() const { return FeatureValue(1); }
  bool HasFeature(int f) const { return features.GetBit(f) != 0; } //features[f]; }  
  const Tag* LemmaTag() const { return tags[lemmaIndex]; }
  const Tag* LemmaTag2() const { return tags[lemmaIndex2]; }
  const Tag* OriginalTag() const { return tags[originalTag]; }
  bool IsCorrectGuess(const Tag *t) const { return this == t || OriginalTag() == t->OriginalTag() || (xAcceptAnyTagWhenCorrectIsSilly && IsSilly()); }
  float CompoundProb() const { return compoundProb; }
  void SetFreq(int f) { frq = f; freqInv = 1.0f / frq; }
  bool IsRuleBase() const { return ruleBase; }
  bool IsRuleBase2() const { return (LemmaTag2() == this && LemmaTag2()->IsRuleBase()); }
  static const Word* ProbsWord() { return probsWord; }
  virtual bool SetFeature(int, int) {
    Message(MSG_ERROR, "call to SetFeature() with unchangeable tag"); return false; }
  void PrintVerbose(std::ostream&, bool printFeatureClass = false) const;
protected:
  static Tag* tags[MAX_TAGS+1];
  static const TagLexicon *tagLexicon;
  static const Word *probsWord;
  void Init();
  float lexProb;
  int frq;
  float freqInv;
  uchar index;
  uchar lemmaIndex;
  uchar lemmaIndex2;
  uchar originalTag;
  float uniProb;
  float compoundProb;
  int members;
  float ctm_cwt;
  char string[MAX_TAG_STRING];
  //  char features[MAX_VALUES];
public:
  BitMap128 features;              // MAX_VALUES must at most 128
  uchar featureValue[MAX_CLASSES]; // feature-value muist be less than 255
protected:
  bool content;
  bool sentenceDelimiter;
  bool punctuationOrEnder;
  bool properNoun;
  bool silly;
  bool compoundStop;
  bool ruleBase;
  DecObj();
};

class ChangeableTag : public Tag {
public:
  ChangeableTag() : Tag("") {
    index = TAG_INDEX_NONE;
    NewObj();
  }
  ChangeableTag(const Tag &t) : Tag(t) {
    index = TAG_INDEX_NONE; 
    *string = '\0';
#ifdef COUNT_OBJECTS
    Tag::nObjects++;
#endif
    NewObj();
  }
  virtual ~ChangeableTag() {
    DelObj(); 
  }
  const char *String();
  bool SetFeature(int c, int f);
  Tag *FindMatchingTag(int&);
  void Reset();
  DecObj();
};

inline std::ostream& operator<<(std::ostream& os, const Tag &t) {
  os << xTag << t.String() << xNoTag;
  return os;
}
inline std::ostream& operator<<(std::ostream& os, const Tag *t) {
  if (t) os << *t; else os << "(null Tag)"; return os;
}
inline int CompareTags(const Tag &t1, const Tag &t2) {
  return strcmp(t1.String(), t2.String());
}
inline int CompareStringAndTag(const char *s, const Tag &t2) {
  return strcmp(s, t2.String());
}
inline int RankTags(const Tag &t1, const Tag &t2) {
  return t1.Frq() - t2.Frq();
}
inline uint KeyTag(const Tag &t) {
  return Hash(t.String());
}

#endif
