/* sentence.cc
 * author: Johan Carlberger
 * last change: 2000-03-23
 * comments: Sentence class
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

#include "letter.h"
#include "sentence.h"
#include "tagger.h"

Tagger *AbstractSentence::tagger = NULL;
DefObj(Sentence);
DefObj(DynamicSentence);

void AbstractSentence::TagMe() {
  ensure(tagger);
  for (int i=2; i<NWords()+2; i++)
    tokens[i]->SetSelectedTag(NULL);
  tagger->TagSentence(this);
  FixUp();
}

void AbstractSentence::SetContentBits() {
  bitsSet = 1;
  
  isHeading = tagger->Tags().SpecialTag(TOKEN_DELIMITER_HEADING) ==
    GetWordToken(0)->SelectedTag();

  int nOK = 0, nForeign = 0;
  int i;
  for (i=2; i<NWords()+2; i++) {
    const WordToken *t = GetWordToken(i);
    if (t->GetWord()->HasStyleRemark())
      containsStyleWord = 1;
    if (t->GetWord()->IsForeign())
      nForeign++; 
    else if (t->GetWord()->IsNewWord())
      containsNewWord = 1;
    else if (!t->SelectedTag()->IsPunctuationOrEnder() &&
	     !t->SelectedTag()->IsProperNoun())
      nOK++;
  }
  seemsForeign = NWords() > 4 && ((nOK == 0 && nForeign > 0)
				  || (float(nOK) / NWords() < 0.15));
  
  for (i=2; i<NWords()-1; i++) {
    if (GetWord(i) == GetWord(i+1) && GetWordToken(i) == GetWordToken(i+1) &&
	GetWordToken(i)->GetToken() != TOKEN_CARDINAL) {
      GetWordToken(i+1)->repeated = 1;
      containsRepeatedWords = 1;
      break;
    }
    for (int j=i+2; (NWords()+2-j)>(j-i); j++) {
      int k;
      for (k=0; k<j-i; k++)
	if (GetWord(i+k) != GetWord(j+k))
	  goto pip;
      for (k=0; k<j-i; k++)
	if (GetWordToken(i+k) != GetWordToken(j+k))
	  goto pip;
      // fix check strings
      for (k=0; k<j-i; k++)
	GetWordToken(j+k)->repeated = 1;
      containsRepeatedWords = 1;
      return;
    pip:;
    }
  }
}

void AbstractSentence::Print(int from, int to, std::ostream& out) const {
  ensure(from >= 0);
  ensure(to < NTokens());
  if (from == 1) from = 2;
  if (to == nWords+2) to--;
  bool green = false, red = false;
  for (int i=from; i<=to; i++) {
    if (GetWordToken(i)->IsChanged()) {
      if (!green) { out << xGreen; green = true; }
    } else {
      if (green) { out << xNoColor; green = false; }
    }
    if (GetWordToken(i)->IsMarked2()) {
      if (!red) { out << xRed; red = true; }
    } else {
      if (red) { out << xNoColor; red = false; }
    }
    if (green && red) Message(MSG_WARNING, "both green and red means purple");
    GetWordToken(i)->Print(out, to!=i);
  }
  if (green) out << xNoColor;
  if (red) out << xNoColor;
}

void DynamicSentence::PrintOrgRange(int from, int to, std::ostream& out) const {
  for (int i=from; i<NTokens(); i++)
    if (tokens[i]->OrgPos() == to+1) {
      Print(from, i-1, out);
      return;
    }
  Message(MSG_WARNING, "PrintOrgRange() failed");
}

void AbstractSentence::Print(std::ostream& out) const {
  if (IsHeading()) out << xHeading;
  if (xOutputWTL)
    Print(0, NTokens()-1, out);
  else if (xPrintSelectedTag)
    Print(2, NWords()+1, out);
  else
    Print(2, NWords()+1, out);
  if (IsHeading()) out << xNoHeading;
}

void AbstractSentence::FixUp() {
  bool setFirst = true;
  bool changedFound = false;
  for (int i=0; i<NTokens(); i++) {
    if (!tokens[i]->SelectedTag())
      Message(MSG_ERROR, "FixUp(): no selected tag in pos", int2str(i));
    if ((i < 2 || i > NTokens()-3)) {
      if (!tokens[i]->SelectedTag()->IsSentenceDelimiter())
	Message(MSG_ERROR, "FixUp(): not OK tag in pos", int2str(i));
    } else {
      if (tokens[i]->SelectedTag()->IsSentenceDelimiter())
	Message(MSG_WARNING, "FixUp() not OK tag in pos", int2str(i));
      tokens[i]->trailingSpace =
	tokens[i+1]->SelectedTag()->IsPunctuationOrEnder() ? 0 : 1;
      if (setFirst && !tokens[i]->SelectedTag()->IsPunctuationOrEnder()) {
	tokens[i]->SetFirstInSentence(true);
	setFirst = false;
      } else if (tokens[i]->IsFirstInSentence())
	tokens[i]->SetFirstInSentence(false);
      if (tokens[i]->IsChanged())
	changedFound = true;
      else if (!changedFound && tokens[i]->OrgPos() != i)
	tokens[i]->changed = 1;
    }
  }
}

DynamicSentence::DynamicSentence(const AbstractSentence *s) : AbstractSentence() {
  NewObj();
  size = nTokens = s->NTokens();
  nWords = s->NWords();
  actualTokens = new WordToken[size]; // new OK
  tokens = new WordToken*[size]; // new OK
  for (int i=0; i<nTokens; i++) {
    actualTokens[i] = *s->GetWordToken(i);
    actualTokens[i].orgPos = (uchar) i;
    actualTokens[i].marked = 0;
    actualTokens[i].marked2 = 0;
    tokens[i] = actualTokens + i;
  }
  prob = s->Prob();
  next = NULL;
  status = SEEMS_OK;
}

DynamicSentence::~DynamicSentence() {
  DelObj();
  if (!tokens) return;
  for (int i=1; i<nTokens-1; i++)
    if (tokens[i]->inserted)
      delete tokens[i];
  delete tokens;
  delete[] actualTokens;
  tokens = NULL;
}

void DynamicSentence::Delete(int from, int to) {
  ensure(size > 0);
  ensure(from <= to);
  ensure(from >= 1);
  ensure(to < nTokens-1);
  short n = (short) (to - from + 1);
  for (int i=to+1; i<nTokens; i++)
    tokens[from++] = tokens[i];
  nTokens -= n;
  nWords -= n;
  ensure(nTokens > 1);
}

bool DynamicSentence::Delete(int orgPos) {
  int pos = 0;
  for (int i=2; i<NTokens()-1; i++)
    if (tokens[i]->orgPos == orgPos) {
      pos = i; break;
    }
  if (pos == 0) return false;
  Delete(pos, pos);
  return true;
}

bool DynamicSentence::Replace(Word *w, const char *string, int orgPos) {
  int pos = 0;
  for (int i=2; i<NTokens()-1; i++)
    if (tokens[i]->orgPos == orgPos) {
      pos = i; break;
    }
  if (pos == 0) return false;
  if (w == GetWord(pos) && !strcmp(string, tokens[pos]->RealString()))
    return true;
  tokens[pos]->SetWord(w, string, TOKEN_WORD);
  tokens[pos]->changed = 1;
  return true;
}

bool DynamicSentence::Insert2(int orgPos, Word *w, const char *string) {
  int pos = 0;
  for (int i=2; i<NTokens()-1; i++)
    if (tokens[i]->orgPos == orgPos) {
      pos = i; break;
    }
  if (pos == 0) return false;
  Insert(pos, w, string);
  return true;
}

void DynamicSentence::Insert(int pos, Word *w, const char *string) {
  ensure(pos < nTokens);
  ensure(pos > 0);
  ensure(size > 0);
  if (size <= nTokens) {
    size += 5;
    WordToken **t = new WordToken*[size]; // new OK
    for (int i=0; i<nTokens; i++)
      t[i] = tokens[i];
    delete tokens;
    tokens = t;
  }
  ensure(pos < size);
  for (int i=nTokens; i>pos; i--)
    tokens[i] = tokens[i-1];
  tokens[pos] = new WordToken(); // new OK
  tokens[pos]->SetWord(w, string, TOKEN_WORD);
  tokens[pos]->selectedTagErasable = 1;
  tokens[pos]->SetSelectedTag(NULL);
  tokens[pos]->changed = tokens[pos]->inserted = 1;
  nTokens++;
  nWords++;
}

bool AbstractSentence::IsEqual(const AbstractSentence *s) const {
  if (NTokens() != s->NTokens())
    return false;
  for (int i=2; i<nTokens-2; i++)
    if (/*GetWord(i) != s->GetWord(i) ||*/ // jonas: if the surface form is equal, treat as equal 
	strcmp(GetWordToken(i)->RealString(), s->GetWordToken(i)->RealString()))
      return false;
  return true;
}

Sentence::~Sentence() {
  DelObj();
  delete [] tokens; // jonas, delete -> delete []
  // jonas, do not delete next, this is now handled in
  // Text::delete_sentences instead (otherwise Stack overflow)
#if 0
  // delete next
#endif
}

