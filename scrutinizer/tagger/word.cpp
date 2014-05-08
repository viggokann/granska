/* word.cc
 * author: Johan Carlberger
 * last change: 2000-02-10
 * comments: Word class
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
#include "word.h"
#include "newword.h"

StringBuf Word::stringBuf;  // jbfix: NewWord:: => Word
DefObj(Word);
DefObj(NewWord);
DefObj(StringBuf);

void Word::ComputeLexProbs() {
  ensure(!HasExtraWordTag() || this == Tag::ProbsWord());
//    tagger->TagUnknownWord(this, true, false, token);
  WordTag *wt;
  if (IsExtra()) 
    for (wt = this; wt; wt = wt->Next()) {
      ensure(wt->IsExtraWordTag());
      if (!wt->GetTag()->IsContent())
	wt->lexProb = (float) xEpsilonExtra * wt->GetTag()->FreqInv();
      else {
	wt->lexProb = (float) xEpsilonExtra * wt->GetTag()->LexProb() * wt->GetTag()->FreqInv();
	ensure(wt->lexProb > 0);
      }
    }
  else {
    ensure(Freq() > 0);
    for (wt = this; wt; wt = wt->Next()) {
      if (wt->IsExtraWordTag()) {
	if (!wt->GetTag()->IsContent()) {
	  wt->lexProb = (float) (xEpsilonExtra * pow(Freq(), -xAlphaExtra) * xLambdaExtra);
	  ensure(wt->lexProb > 0);
	  ensure(wt->lexProb < 10000);
	} else {
	  wt->lexProb = (float) (wt->GetTag()->LexProb() *
				 pow(Freq(), -xAlphaExtra) * xLambdaExtra);
	  ensure(wt->lexProb > 0);
	} 
      } else {
	if (wt->TagFreq() <= 0)
	  Message(MSG_WARNING, "ComputeLexProbs, word has negative tag-freq", String());
	ensure(wt->TagFreq() > 0);
	if (xTaggingEquation == 19)
	  wt->lexProb = float(wt->TagFreq()) * wt->GetTag()->FreqInv();
	else if (xTaggingEquation == 20)
	  wt->lexProb = float(wt->TagFreq())/Freq();
	else if (xTaggingEquation == 21)
	  wt->lexProb = xLambda19*float(wt->TagFreq())/Freq() + 
	    float(wt->TagFreq()) * wt->GetTag()->FreqInv();
	else
	  ensure(0);
	//	if (wt->GetTag()->IsSilly())
	//	  wt->lexProb *= 1;
	ensure(wt->lexProb > 0);
      }
      ensure(wt->lexProb > 0);
    }
  }
}

void NewWord::ComputeLexProbs() {
  if (IsAmbiguous()) {
    ensure(this == Tag::ProbsWord());
    //    tagger->TagUnknownWord(this, true, false, token);
    for (WordTag *wt = this; wt; wt = wt->Next()) {
      if (wt->GetTag()->IsContent()) {
	wt->lexProb = (float) (wt->GetTag()->LexProb());
	ensure(wt->lexProb > 0);
      } else
	Message(MSG_ERROR, "new-word", String(), "has non-content tag",
		wt->GetTag()->String());
    }
  } else
    lexProb = 1;
}

void Word::Print(std::ostream& out) const {
  out << String() << ' ';
  if (xPrintWordInfo) {
    if (IsNewWord())
      ((NewWord*)this)->PrintInfo(out);
    else
      PrintInfo(out);
  }
  if (xPrintAllWordTags) 
    out << std::endl << tab;
  WordTag::Print(out);
  if (xPrintAllWordTags)
    for (const WordTag *wt=Next(); wt; wt=wt->Next()) {
      out << tab;
      wt->Print(out);
    }
}

void Word::PrintTags(std::ostream& out) const {
  //  out << std::endl << tab;
  out << tab;
  WordTag::PrintTag(out);
  for (const WordTag *wt=Next(); wt; wt=wt->Next()) {
    out << tab;
    wt->PrintTag(out);
  }
}

void Word::PrintInfo(std::ostream& out) const {
  out << Freq() << ' ' << TextFreq() << ' ';
  if (IsExtra()) out << 'E';
  if (IsForeign()) out << "f";
  if (MayBeCapped()) out << "m";
  if (IsCompoundBeginOK()) out << 'b';
  if (IsCompoundEndOK()) out << 'q';
  if (IsSpellOK()) out << 's';
}


