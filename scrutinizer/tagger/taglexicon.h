/* taglexicon.hh
 * author: Johan Carlberger
 * last change: 2000-04-27
 * comments: TagLexicon class
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

#ifndef _taglexicon_hh
#define _taglexicon_hh

#include "bitmap.h"
#include "feature.h"
#include "hasharray.h"
#include "tag.h"
#include "tagtrigram.h"
#include "token.h"
#include "word.h"

class TagLexicon : public HashArray<Tag> {
public:
  TagLexicon() : dummyTag(0), /*bigramProbs(NULL),*/ bigramFreqs(NULL)/*, CT(0)*/ {
    ensure(MAX_VALUES < 256); NewObj(); }
  ~TagLexicon();
  void LoadSlow(const char *dir);
  bool LoadFast(const char *dir, bool warn=true);
  bool Save() const;
  Tag* FindTag(const char *s) const { Tag t(s); return Find(t); }
  float Pt3_t1t2(const Tag* t1, const Tag* t2, const Tag* t3) const;
  static float Pt3_t1t2(uchar t1, uchar t2, uchar t3) {   // P(t3 | t1, t2)
    if (ttOff[t1][t2] >= 0) {
      TagTrigram *t = ttt + ttOff[t1][t2];
      for (; t->tag3<t3; t++);
      if (t->tag3 == t3 && t->tag2 == t2)
	return t->pf.prob;
    }
    return bigramProbs[t2][t3];
    //    TagTrigram *t = tagTrigrams.Find(TagTrigram(t1, t2, t3));
    //    return t ? t->pf.prob : bigramProbs[t2][t3];
  }
  float Pt1t2(uchar t1, uchar t2) const {               // P(t1, t2) = P(t1)*P(t2 | t1)
    ensure(bigramFreqs);
    return Element(t1).UniProb() * bigramFreqs[t1][t2] / Element(t1).Frq();
  }
  static int ttt_freq(uchar t1, uchar t2, uchar t3) {
    if (ttOff[t1][t2] >= 0) {
      TagTrigram *t = ttt + ttOff[t1][t2];
      for (; t->tag3<t3; t++);
      if (t->tag3 == t3 && t->tag2 == t2)
	return t->pf.freq;
    }
    return 0; //bigramFreqs[t2][t3];
  }
  int tt_freq(uchar t1, uchar t2) const
  {
      return bigramFreqs[t1][t2];
  }
  int t_freq(uchar t1) const
  {
      return Element(t1).Frq();
  }
  int Ct() const { return CT; }
  Tag* SpecialTag(Token t) const { return specialTags[t]; }
  void PrintStatistics() const;
  bool IsLoaded() const { return CT > 0; }
  void ComputeProbs();
  int FeatureValueIndex(const char*) const;
  int FeatureClassIndex(const char*) const;
  bool IsOKFeatureIndex(int f) const { return f >= 0 && f < nFeatures; }
  bool IsCompatible(int f1, int f2) const;
  void SetCompatible(int f1, int f2);
  const FeatureValue& GetFeature(int) const;
  const FeatureClass& GetFeatureClass(int) const;
  int NFeatures() const { return nFeatures; }
  int NFeatureClasses() const { return nFeatureClasses; }
  void TestFeatures() const;
  Tag *dummyTag; // jonas
  Tag *DummyTag() { if(!dummyTag) dummyTag = new Tag("DummyTag"); return dummyTag;} // jonas
private:
  void LoadInfo();
  void LoadFeatures();
  void LoadTags();
  void LoadTagInfo();
  void LoadTagBigrams();
  void LoadTagTrigrams();
  void LoadTagMember();
  void LoadTagCapital();
  void AllocateMemory(bool fastMode);
  void SetTags();
  const char *lexiconDir;
  static float bigramProbs[MAX_TAGS][MAX_TAGS];
  int **bigramFreqs;
  //  HashArray<TagTrigram> tagTrigrams;
  static TagTrigram *ttt;
  static int ttOff[MAX_TAGS][MAX_TAGS];
  static int CT, CTT, CTTT, CWT;
  Tag *specialTags[MAX_TAGS];
  int specialTagIndices[MAX_TAGS];
  int nFeatures;
  int nFeatureClasses;
  FeatureValue features[MAX_VALUES+1];
  FeatureClass featureClasses[MAX_CLASSES+1];
  BitMap65536 equalFeaturesBitMap;
  const Word *probsWord;
  DecObj();
};

inline float TagLexicon::Pt3_t1t2(const Tag* t1, const Tag* t2, const Tag* t3) const {
  return Pt3_t1t2(t1->index, t2->index, t3->index); 
}

inline int TagLexicon::FeatureValueIndex(const char *s) const {
  for (int i=0; i<nFeatures; i++)
    if (!strcmp(s, features[i].Name()))
      return i;
  return UNDEF;
}

inline int TagLexicon::FeatureClassIndex(const char *s) const {
  for (int i=0; i<nFeatures; i++)
    if (!strcmp(s, featureClasses[i].Name()))
      return i;
  return UNDEF;
}

inline bool TagLexicon::IsCompatible(int f1, int f2) const {
  //  ensure(IsOKFeatureIndex(f1));
  //  ensure(IsOKFeatureIndex(f2));
  return equalFeaturesBitMap.GetBit(f1 | (f2 << 8));
}

inline void TagLexicon::SetCompatible(int f1, int f2) {
  ensure(IsOKFeatureIndex(f1));
  ensure(IsOKFeatureIndex(f2));
  equalFeaturesBitMap.SetBit(f1 | (f2 << 8));
  equalFeaturesBitMap.SetBit(f2 | (f1 << 8));
}
 
inline const FeatureValue& TagLexicon::GetFeature(int f) const {
  ensure(IsOKFeatureIndex(f));
  return features[f];
}
 
inline const FeatureClass& TagLexicon::GetFeatureClass(int f) const {
  ensure(f >= 0);
  ensure(f < nFeatureClasses);
  return featureClasses[f];
}

#endif
