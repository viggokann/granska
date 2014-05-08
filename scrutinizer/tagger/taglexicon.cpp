/* taglexicon.cc
 * author: Johan Carlberger
 * last change: 2000-05-12
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

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "file.h"
#include "letter.h"
#include "message.h"
#include "settings.h"
#include "taglexicon.h"

ushort BitMap128::mask[16] = { 1,2,4,8,16,32,64,128,256,512,
			       1024,2048,4096,8192,16384,32768 };

ushort BitMap65536::mask[16] = { 1,2,4,8,16,32,64,128,256,512,
			       1024,2048,4096,8192,16384,32768 };

DefObj(TagLexicon);
DefObj(HashArray<Tag>);
//DefObj(HashArray<TagTrigram>);
DefObj(TagTrigram);

int TagLexicon::CT = 0, TagLexicon::CTT = 0, TagLexicon::CTTT = 0, TagLexicon::CWT = 0;
float TagLexicon::bigramProbs[MAX_TAGS][MAX_TAGS];
int TagLexicon::ttOff[MAX_TAGS][MAX_TAGS];
TagTrigram *TagLexicon::ttt;

TagLexicon::~TagLexicon() {
  Message(MSG_STATUS, "deleting taglexicon...");
  /*  if (bigramProbs) {
    for (int j=0; j<CT; j++)
      delete bigramProbs[j];
    delete bigramProbs;
    ExtByt(-CT * sizeof(float*));
    ExtByt(-CT * CT * sizeof(float));
    }*/
  if (bigramFreqs) {
    for (int j=0; j<CT; j++)
      delete [] bigramFreqs[j];
    delete [] bigramFreqs;	// jbfix: delete => delete []
    ExtByt(-CT * sizeof(int*));
    ExtByt(-CT * CT * sizeof(int));
  }
  delete [] ttt;    // jb: ttt shouldn't be static, better to have TagLexicon singelton?
  ttt = 0;
  delete dummyTag; dummyTag = 0; // jonas
  DelObj();
}
   
//float TagLexicon::Pt3_t1t2(uchar t1, uchar t2, uchar t3) const {}

void TagLexicon::ComputeProbs() {
  ensure(bigramFreqs);
  static float prevUni = -1;
  static float prevBi = -1;
  static float prevTri = -1;
  static float prevExp = -1;
  static float prevEps = -1;
  int i;
  if (xLambdaUni != prevUni || xLambdaBi != prevBi ||
      xEpsilonTri != prevEps) {
    for (int j=0; j<CT; j++)
      for (i=0; i<CT; i++) {
	bigramProbs[i][j] = xLambdaUni*Element(j).UniProb();
	if (bigramFreqs[i][j] > 0)
	  bigramProbs[i][j] += xLambdaBi*bigramFreqs[i][j]*Element(i).FreqInv() + xEpsilonTri;
      }
  }
  if (xLambdaUni != prevUni || xLambdaBi != prevBi ||
      xLambdaTri != prevTri || xLambdaTriExp != prevExp)
    if (xLambdaTriExp < 0.6)
      for (i=0; i<CTTT; i++) {
	TagTrigram &t = ttt[i];
	ensure(bigramFreqs[t.tag1][t.tag2] > 0);
	t.pf.prob =  xLambdaUni*Element(t.tag3).UniProb()+ 
	  xLambdaBi*bigramFreqs[t.tag2][t.tag3]*Element(t.tag2).FreqInv()+
	  xLambdaTri*t.pf.freq*(float)pow(bigramFreqs[t.tag1][t.tag2], xLambdaTriExp-1);
	ensure(t.pf.prob > 0);
	/*	TagTrigram &t = tagTrigrams[i];
	ensure(bigramFreqs[t.tag1][t.tag2] > 0);
	t.pf.prob =  xLambdaUni*Element(t.tag3).UniProb()+ 
	  xLambdaBi*bigramFreqs[t.tag2][t.tag3]*Element(t.tag2).FreqInv()+
	  xLambdaTri*t.pf.freq*(float)pow(bigramFreqs[t.tag1][t.tag2], xLambdaTriExp-1);
	  ensure(t.pf.prob > 0); */
      }
    else
      Message(MSG_MINOR_WARNING, "xLambdaTriExp cannot exceed 0.6 (not tested)");
  prevUni = xLambdaUni;
  prevBi = xLambdaBi;
  prevTri = xLambdaTri;
  prevExp = xLambdaTriExp;
  prevEps = xEpsilonTri;
}

bool TagLexicon::Save() const {
  std::ifstream in;
  if (FixIfstream(in, lexiconDir, "fast", false))
    return false;
  ensure(bigramFreqs);
  Message(MSG_STATUS, "saving taglexicon fast...");
  in.close();
  std::ofstream out;
  FixOfstream(out, lexiconDir, "fast");
  SetVersion(out, xVersion);
  WriteVar(out, CT);
  WriteVar(out, CTT);
  WriteVar(out, CTTT);
  WriteVar(out, CWT);
  WriteData(out, (char*)specialTagIndices, sizeof(int)*MAX_TAGS, "specials");
  Store(out);
  //  tagTrigrams.Store(out);
  WriteData(out, (char*) ttt, sizeof(TagTrigram)*(CTTT+1), "tagtrigrams");
  WriteData(out, (char*) ttOff, sizeof(int)*MAX_TAGS*MAX_TAGS, "ttoffset");
  WriteVar(out, nFeatures);
  WriteVar(out, nFeatureClasses);
  WriteData(out, (char*) features, sizeof(FeatureValue)*(nFeatures+1), "features");
  WriteData(out, (char*) featureClasses, sizeof(FeatureClass)*(nFeatureClasses+1), "fclasses");
  WriteData(out, (char*) &equalFeaturesBitMap, sizeof(equalFeaturesBitMap), "equalFBM");
  int i;
  for (i=0; i<CT; i++)
    WriteData(out, (char*)bigramProbs[i], sizeof(float)*CT);
  // bigramFreqs must be saved last:
  for (i=0; i<CT; i++)
    WriteData(out, (char*)bigramFreqs[i], sizeof(int)*CT);
  return true;
}

bool TagLexicon::LoadFast(const char* dir, bool warn) {
  std::ifstream in;
  if (!FixIfstream(in, dir, "fast", warn))
    return false;
  if (!CheckVersion(in, xVersion)) {
    Message(MSG_WARNING, "fast tag lexicon obsolete");
    return false;
  }
  Message(MSG_STATUS, "loading tag lexicon fast...");
  ReadVar(in, CT);
  ReadVar(in, CTT);
  ReadVar(in, CTTT);
  ReadVar(in, CWT);
  ReadData(in, (char*)specialTagIndices, sizeof(int)*MAX_TAGS, "specials");
  AllocateMemory(true);
  Load(in);
  //  tagTrigrams.Load(in);
  ReadData(in, (char*) ttt, sizeof(TagTrigram)*(CTTT+1), "tagtrigrams");
  ReadData(in, (char*) ttOff, sizeof(int)*MAX_TAGS*MAX_TAGS, "ttoffset");
  //  ComputeProbs();
  ReadVar(in, nFeatures);
  ReadVar(in, nFeatureClasses);
  ReadData(in, (char*)features, sizeof(FeatureValue)*(nFeatures+1), "features");
  ReadData(in, (char*)featureClasses, sizeof(FeatureClass)*(nFeatureClasses+1), "fclasses");
  ReadData(in, (char*)&equalFeaturesBitMap, sizeof(equalFeaturesBitMap), "equalFBM");
  SetTags();
  int i;
  for (i=0; i<CT; i++)
    ReadData(in, (char*)bigramProbs[i], sizeof(float)*CT);
  if (bigramFreqs)
    for (i=0; i<CT; i++)
      ReadData(in, (char*)bigramFreqs[i], sizeof(int)*CT);
  TestFeatures();
  return true;
}

void TagLexicon::LoadInfo() {
  std::ifstream in;
  FixIfstream(in, lexiconDir, "info");
  char string[100];
  in >> CT >> string;
  in >> CTT >> string;
  in >> CTTT >> string;
  in >> CWT >> string;
}

void TagLexicon::AllocateMemory(bool fastMode) {
  Init("tags", CT, CompareTags, KeyTag, RankTags, CompareStringAndTag, KeyWordString);
  ttt = new TagTrigram[CTTT+1];
  //  tagTrigrams.Init("tagtrigrams", CTTT, CompareTagTrigrams, KeyTagTrigram, RankTagTrigrams);
  /*  bigramProbs = new float*[CT]; // new OK
  for (int i=0; i<CT; i++)
    bigramProbs[i] = new float[CT]; // new OK
  ExtByt(CT * sizeof(float*));
  ExtByt(CT * CT * sizeof(float)); */
  if (fastMode && !xOptimizeMatchings) return;
  bigramFreqs = new int*[CT]; // new OK
  for (int j=0; j<CT; j++)
    bigramFreqs[j] = new int[CT]; // new OK
  ExtByt(CT * sizeof(int*));
  ExtByt(CT * CT * sizeof(int));
}

void TagLexicon::LoadSlow(const char* dir) {
  ensure(MAX_VALUES <= 128);
  lexiconDir = dir;
  Message(MSG_STATUS, "loading tag lexicon slow...");
  LoadInfo();
  AllocateMemory(false);
  LoadTags();
  Message(MSG_STATUS, "loading tag info...");
  LoadTagInfo();
  Message(MSG_STATUS, "loading tag features...");
  LoadFeatures();
  Message(MSG_STATUS, "loading tag bigrams...");
  LoadTagBigrams();
  if (xTagTrigramsUsed) {
    Message(MSG_STATUS, "loading tag trigrams...");
    LoadTagTrigrams();
  }
  Message(MSG_STATUS, "loading tag member...");
  LoadTagMember();
  ComputeProbs();
  SetTags();
  TestFeatures();
}

void TagLexicon::LoadFeatures() {
  std::ifstream in;
  if (!FixIfstream(in, lexiconDir, "features")) {
    Message(MSG_WARNING, "cannot open file tags/features, no tag features loaded");
    return;
  } 
  ensure(UNDEF == 0);
  strcpy(features[0].name, "undef");
  strcpy(features[0].description, "undefined feature value");
  features[0].index = 0;
  strcpy(featureClasses[0].name, "undef");
  strcpy(featureClasses[0].description, "undefined feature class");
  featureClasses[0].index = 0;
  nFeatures = 1;
  nFeatureClasses = 1;
  equalFeaturesBitMap.Clear();
  SetCompatible(0, 0);
  ensure(IsCompatible(0, 0));
  while(in.peek() == '*') {
    ensure(nFeatureClasses < MAX_CLASSES);
    in.get();
    FeatureClass &fc = featureClasses[nFeatureClasses];
    in >> fc.name;
    ensure(nFeatures != 1 || !strcmp(fc.name, "wordcl"));
    while (IsSpace((char)in.peek())) in.get();
    in.getline(fc.description, MAX_DESCRIPTION);
    int i=0;
    while(in.peek() != '*' && in.peek() != EOF) {
      ensure(nFeatures < MAX_VALUES);
      ensure(i < MAX_FEATURES_PER_CLASS);
      FeatureValue& fv = features[nFeatures];
      in >> fv.name;
      fv.index = nFeatures;
      nFeatures++;
      while (IsSpace((char)in.peek())) in.get();
      in.getline(fv.description, MAX_DESCRIPTION);
      fv.featureClass = nFeatureClasses;
      fc.features[i] = fv.index;
      SetCompatible(fv.Index(), fv.Index());
      ensure(IsCompatible(fv.Index(), fv.Index()));
      if (strchr(fv.name, '/')) {
	char s[MAX_NAME];
	strcpy(s, fv.name);
	for(char *ff = strtok(s, "/"); ff; ff = strtok(NULL, "/")) {
	  int index2 = FeatureValueIndex(ff);
	  ensure(index2 != UNDEF);
	  SetCompatible(fv.index, index2);
	  ensure(IsCompatible(fv.index, index2));
	}
      }
      i++;
    }
    fc.nFeatures = i;
    nFeatureClasses++;
  }
  for (int i=0; i<CT; i++) {
    Tag &t = Element(i);
    char s[MAX_TAG_STRING];
    strcpy(s, t.string);
    for(char *f = strtok(s, "."); f; f = strtok(NULL, ".")) {
      int j = FeatureValueIndex(f);
      if (j == UNDEF)
	Message(MSG_WARNING, "unknown feature in tags/features:", f);
      t.features.SetBit(j);
      //      t.features[j] = true;
      t.featureValue[features[j].featureClass] = j;
    }
    strcpy(s, t.string);
    for(char *ff = strtok(s, "./"); ff; ff = strtok(NULL, "./")) {
      int j = FeatureValueIndex(ff);
      if (j == UNDEF)
	Message(MSG_WARNING, "unknown feature in tags/features:", ff);
      t.features.SetBit(j); //[j] = true;
      //      t.features[j] = true;
    }
  }
  //  SetCompatible(UNDEF, FeatureValueIndex("set"));
  //  SetCompatible(FeatureValueIndex("set"), UNDEF);
}

void TagLexicon::TestFeatures() const {
  if (!xTestFeatures)
    return;
  std::cout << "testing tag methods:"<<std::endl
       << "tags:"<<std::endl;
  int i;
  for (i=0; i<CT; i++) {
    Tag &t = Element(i);
    std::cout <<i<<tab<< t << std::endl;
  }
  std::cout << "features:"<<std::endl;
  for (i=0; i<CT; i++) {
    Tag &t = Element(i);
    std::cout << t << " (";
    int j;
    for (j=0; j<nFeatures; j++)
      if (t.HasFeature(j))
	std::cout<<features[j]<<' ';
    std::cout << ") ";
    for (j=0; j<nFeatureClasses; j++)
      if (t.FeatureValue(j) != UNDEF)
	std::cout << "("<<featureClasses[j]<<" "<<features[t.FeatureValue(j)]<<')';
    std::cout << std::endl;
  }
  std::cout << "testing compatible features:"<<std::endl;
  for (i=0; i<nFeatures; i++) {
    ensure(IsCompatible(i, i));
    for (int j=0; j<nFeatures; j++)
      if (i != j && IsCompatible(i, j))
	std::cout << features[i] << " and " << features[j] << " are compatible"<<std::endl;
  }
  std::cout << "testing features classes:"<<std::endl;  
  for (i=0; i<nFeatureClasses; i++) {
    const FeatureClass &fc = featureClasses[i];
    std::cout << fc << " can be either of: ";
    for (int j=0; j<fc.NFeatures(); j++)
      std::cout << GetFeature(fc.GetFeature(j)) << ' ';
    std::cout << std::endl;
  }
}

void TagLexicon::LoadTags() {
  std::ifstream in;
  FixIfstream(in, lexiconDir, "ct");
  int i, freq;
  for (i=0; i<CT; i++) {
    Tag &t = Element(i);
    int j;
    for (j=0; j<MAX_VALUES; j++)
      t.features.UnSetBit(j); //[j] = false;
    //  t.features[j] = false;
    for (j=0; j<MAX_CLASSES; j++)
      t.featureValue[j] = UNDEF;
    in >> freq >> t.string;
    t.SetFreq(freq);
    ensure(strlen(t.string)+4 < (uint)MAX_TAG_STRING);
    t.uniProb = float(freq) / CWT;
    if (strstr(t.string, "sms"))
      t.uniProb *= 0.000001f;
    if (!(strncmp(t.string, "vb", 2)))
      t.compoundProb = (float) 0.1;
    else
      t.compoundProb = (float) 1.0;
  }
  Hashify();
  for (uchar j=0; j<CT; j++) {
    Element(j).index = j;
  }
}

void TagLexicon::LoadTagInfo() {
  std::ifstream in;
  FixIfstream(in, lexiconDir, "taginfo");
  for (int i=0; i<CT; i++) {
    specialTags[i] = NULL;
    specialTagIndices[i] = -1;
    Element(i).lexProb = 0;
  }
  char minusOrPlus;
  Tag tag;
  Tag base;
  int k=0;
  while (in >> minusOrPlus >> tag.string >> base.string) {
    k++;
    Tag *t = Find(tag);
    if (!t) {
      Message(MSG_MINOR_WARNING, "unknown tag in tags/taginfo:", tag.string);
      while (in.peek() != '\n') in.get();
      continue;
    }
    Tag *b = Find(base);
    if (!b) 
      Message(MSG_ERROR, "unknown lemma tag in tags/taginfo:", base.string);
    t->lemmaIndex = b->Index();
    t->lexProb = 1;
    t->content = t->properNoun = t->punctuationOrEnder = t->silly = t->sentenceDelimiter = false;
    if (minusOrPlus == '+')
      t->content = true;
    else if (minusOrPlus == '-')
      t->content = false;
    else
      Message(MSG_ERROR, "bad format in tags/taginfo at line", int2str(k));
    while (in.peek() == '\t') {
      in >> base.string;
      b = Find(base);
      if (b) 
	t->lemmaIndex2 = b->Index();
      else {
	Token token = String2Token(base.string);
	switch(token) {
	case TOKEN_ERROR:
	  Message(MSG_WARNING, "token = ERROR in tags/taginfo at line", int2str(k));
	  continue; break;
	case TOKEN_SILLY:
	  t->silly = true; continue; break;
	case TOKEN_DELIMITER_PERIOD:
	case TOKEN_DELIMITER_QUESTION:
	case TOKEN_DELIMITER_EXCLAMATION:
	case TOKEN_DELIMITER_HEADING:
	case TOKEN_DELIMITER_OTHER:
	  t->sentenceDelimiter = true; break;
	case TOKEN_PROPER_NOUN:
	case TOKEN_PROPER_NOUN_GENITIVE:
	  t->properNoun = true; break;
	case TOKEN_PERIOD: 
	case TOKEN_QUESTION_MARK:
	case TOKEN_EXCLAMATION_MARK:
	case TOKEN_PUNCTUATION:
	  t->punctuationOrEnder = true; break;
	default: break;
	}
	if (specialTags[token])
	  Message(MSG_WARNING, "tags/taginfo: only one tag allowed for each token, line", int2str(k));
	else {
	  specialTags[token] = t;
	  specialTagIndices[token] = t->Index();
	}
      }
    }
  }
  for (int m=0; m<CT; m++)
    Element(m).compoundStop = (!Element(m).IsContent() || Element(m).IsSilly());
  if (k < CT)
    Message(MSG_WARNING, "tags/taginfo contains too few tags, only", int2str(k));
  if (k)
    for (int j=0; j<CT; j++)
      if (Element(j).lexProb == 0)
	Message(MSG_ERROR, "tags/taginfo, no info for tag", Element(j).String());
}

void TagLexicon::LoadTagBigrams() {
  ensure(bigramFreqs);
  std::ifstream in;
  int freq;
  for (int j=0; j<CT; j++)
    for (int k=0; k<CT; k++)
      bigramFreqs[j][k] = 0;
  FixIfstream(in, lexiconDir, "ctt");
  Tag tag1, tag2;
  while(in >> freq >> tag1.string >> tag2.string) {
    Tag *t1 = Find(tag1);
    Tag *t2 = Find(tag2);
    if (!t1)
      Message(MSG_ERROR, "unknown tag in tags/ctt", tag1.string);
    if (!t2)
      Message(MSG_ERROR, "unknown tag in tags/ctt", tag2.string);
    bigramFreqs[t1->index][t2->index] = freq;
  }
}

static int TagTriComp(const void *a1, const void *a2) {
  const TagTrigram *t1 = (TagTrigram*) a1;
  const TagTrigram *t2 = (TagTrigram*) a2;
  if (t1->tag1 > t2->tag1) return 1;
  if (t1->tag1 < t2->tag1) return -1;
  if (t1->tag2 > t2->tag2) return 1;
  if (t1->tag2 < t2->tag2) return -1;
  if (t1->tag3 > t2->tag3) return 1;
  if (t1->tag3 < t2->tag3) return -1;
  return 0;
}

void TagLexicon::LoadTagTrigrams() {
  std::ifstream in;
  FixIfstream(in, lexiconDir, "cttt");
  Tag tag1, tag2, tag3;
  int i=0;
  int freq;
  while(in >> freq >> tag1.string >> tag2.string >> tag3.string) {
    ensure(freq > 0);
    Tag *t1 = Find(tag1);
    Tag *t2 = Find(tag2);
    Tag *t3 = Find(tag3);
    if (!t1)
      Message(MSG_ERROR, "unknown tag in tags/ctt", tag1.string);
    if (!t2)
      Message(MSG_ERROR, "unknown tag in tags/ctt", tag2.string);
    if (!t3)
      Message(MSG_ERROR, "unknown tag in tags/ctt", tag3.string);
    ttt[i].tag1 = t1->index;
    ttt[i].tag2 = t2->index;
    ttt[i].tag3 = t3->index;
    ttt[i].pf.freq = freq;

    /*    tagTrigrams[i].tag1 = t1->index;
    tagTrigrams[i].tag2 = t2->index;
    tagTrigrams[i].tag3 = t3->index;
    tagTrigrams[i].pf.freq = freq; */
    i++;
  }
  ensure(i = CTTT);
  ttt[CTTT].tag1 = ttt[CTTT].tag2 = ttt[CTTT].tag3 = CT;
  qsort((char*)ttt, CTTT, sizeof(TagTrigram), TagTriComp);
  int a1 = -1, a2 = -1;
  for (i=0; i<CT; i++)
    for (int j=0; j<CT; j++)
      ttOff[i][j] = -1;
  for (i=0; i<CTTT; i++) {
    bool c = false;
    if (ttt[i].tag1 != a1) { a1 = ttt[i].tag1; c = true; }
    if (ttt[i].tag2 != a2) { a2 = ttt[i].tag2; c = true; }
    if (c) ttOff[a1][a2] = i;
  }
  //  tagTrigrams.Hashify();
}

void TagLexicon::PrintStatistics() const {
  std::cout << "tag lexicon statistics: ";
  Statistics();
  /*  if (xTagTrigramsUsed) {
    std::cout << "tag trigrams: ";
    tagTrigrams.Statistics();
    } */
}

void TagLexicon::LoadTagMember() {
  std::ifstream in;
  int freq;
  Tag tag;
  int sum=0;
  FixIfstream(in, lexiconDir, "ctm");
  for (int i=0; i<CT; i++) {
    in >> freq >> tag.string;
    Tag *t = Find(tag);
    if (!t) {
      Message(MSG_WARNING, "unknown tag in tags/ctm:", tag.string);
      continue;
    }
    ensure(t);
    t->members = freq;
    t->ctm_cwt = float(freq) / CWT;
    sum += freq;
  }
  if (sum != CWT)
    Message(MSG_MINOR_WARNING, "sum of tag members =", int2str(sum),
	    "; should be", int2str(CWT));
}

void TagLexicon::SetTags() {
  Tag::tagLexicon = this;
  int i;
  for (i=0; i<Ct(); i++) {
    Tag::tags[i] = &Element(i);
    specialTags[i] = specialTagIndices[i] < 0 ? NULL : &Element(specialTagIndices[i]);
  }
  for (i=Ct(); i<=MAX_TAGS; i++) {
    Tag::tags[i] = NULL;
    specialTags[i] = NULL;
  }
  Tag tag;
  for (i=0; i<Ct(); i++) {
    Tag &t = Element(i);
    if (strstr(t.String(), "set") || strstr(t.String(), "dat") ||
	strstr(t.String(), "mod") || strstr(t.String(), "aux") || 
	strstr(t.String(), "kop")) {
      strcpy(tag.string, t.String());
      if (strstr(t.String(), "imp.akt"))
	tag.string[strlen(t.String())-8] = '\0';
      else
	tag.string[strlen(t.String())-4] = '\0';
      Tag *tg = Find(tag);
      ensure(tg);
      t.originalTag = tg->Index();
    } else
      t.originalTag = (uchar) i;
  }
}
