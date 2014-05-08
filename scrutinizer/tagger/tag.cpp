/* tag.cc
 * author: Johan Carlberger
 * last change: 991103
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

#include "message.h"
#include "tag.h"
#include "taglexicon.h"

Tag *Tag::tags[MAX_TAGS+1];
const TagLexicon *Tag::tagLexicon = NULL;
const Word *Tag::probsWord = NULL;
DefObj(Tag);
DefObj(ChangeableTag);

void Tag::Init() {
  index = TAG_INDEX_NONE;
  frq = 0;
  lemmaIndex = TAG_INDEX_NONE;
  lemmaIndex2 = TAG_INDEX_NONE;
  uniProb = 0;
  members = 0;
  uniProb = 0;
  ctm_cwt = 0;
  int i;
  //  for (i=0; i<MAX_VALUES; i++)
  //   features[i] = false;
  features.Clear();
  for (i=0; i<MAX_CLASSES; i++)
    featureValue[i] = UNDEF;
  content = sentenceDelimiter = punctuationOrEnder = properNoun = false;
  silly = false;
}

bool ChangeableTag::SetFeature(int c, int f) {
  if (c < 0 || c >= MAX_CLASSES) {
    Message(MSG_WARNING, "no such feature class", int2str(c));
    return false;
  }
  if (f < 0 || f >= MAX_VALUES) {
    Message(MSG_WARNING, "no such feature value", int2str(f));
    return false;
  }
  const FeatureClass &fc = tagLexicon->GetFeatureClass(c);
  for (int i=0; i<fc.NFeatures(); i++)
    features.UnSetBit(fc.GetFeature(i));
  //    features[fc.GetFeature(i)] = false;
  features.SetBit(f);
  // features[f] = true;
  featureValue[c] = f;
  *string = '\0';
  return true;
}

const char *ChangeableTag::String() {
  if (*string)
    return string;
  for (int j=0; j<MAX_CLASSES; j++)
    if (featureValue[j] != UNDEF) {
      if (string[0] != '\0')
	strcat(string, ".");
      strcat(string, tagLexicon->GetFeature(featureValue[j]).Name());
    }
  return string;
}

void ChangeableTag::Reset() {
  *string = '\0';
  int i;
  features.Clear();
  //  for (i=0; i<MAX_VALUES; i++)
  //    features[i] = false;
  for (i=0; i<MAX_CLASSES; i++)
    featureValue[i] = UNDEF;
}

Tag *ChangeableTag::FindMatchingTag(int &n) {
  Tag *t = tagLexicon->FindTag(String());
  if (t) {
    if (n < 0) {
      n = 0;
      return t;
    }
  }
  if (n < 0)
    n = 0;
  // the try to find a compatible tag:
  int fvset = tagLexicon->FeatureValueIndex("set"); // messy fix
  for (; n<tagLexicon->Ct(); n++) {
    Tag *t2 = tags[n];
    if (t != t2)
      for (int j=0; j<MAX_CLASSES; j++)
	if (featureValue[j] != UNDEF &&
	    !tagLexicon->IsCompatible(featureValue[j], t2->FeatureValue(j))
	    && featureValue[j] != fvset)
	  goto next;
    n++;
    return t2;
  next:;
  }
  return NULL;
} 

void Tag::PrintVerbose(std::ostream& out, bool a) const {
  bool jobbigt = false;
  for (int i=0; i<MAX_CLASSES; i++)
    if (featureValue[i] != UNDEF) {
      if (jobbigt) out << ", ";
      jobbigt = true;
      if (a) out << tagLexicon->GetFeatureClass(i).Description() << " = ";
      out << tagLexicon->GetFeature(featureValue[i]).Description();
    }
}
