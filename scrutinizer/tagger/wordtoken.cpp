/* wordtoken.cc
 * author: Johan Carlberger
 * last change: 2000-03-23
 * comments: WordToken class
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
#include "settings.h"
#include "token.h"
#include "newword.h"
#include "wordtoken.h"

StringBuf WordToken::stringBuf;
DefObj(WordToken);

void WordToken::SetWord(Word *w, const char *s, Token t) {
  word = w;
  token = t;
  if (s && strcmp(s, w->String())) {
    string = stringBuf.NewString(s);
    SetCapped(s);
  } else
    string = w->String();
}

void WordToken::SetCapped(const char *s) {
  if (IsUpper(*s))
    firstCapped = 1;
  for(; *s; s++)
    if (!IsUpper(*s))
      return;
  allCapped = 1;
}

void WordToken::SetFirstInSentence(bool b) {
  //  std::cout << "sf " << string << std::endl;
  char st[MAX_WORD_LENGTH];
  firstInSentence = b;
  if (b) {
    if (IsUpper(*string)) 
      return;
    *st = Upper(*string);
  } else {
    if (IsLower(*string))
      return;
    *st = Lower(*string);
  }
  strcpy(st+1, string+1);      
  SetWord(word, st, token);
}

const char *WordToken::LemmaString() const {
  const WordTag *wt = GetWordTag();
  if (wt && wt->Lemma(0))
    return wt->Lemma(0)->String();
  return GetWord()->String();
}

void WordToken::Print(std::ostream& out, bool printSpace) const {
  if (xOutputWTL) {
    if (GetWord())
      out << LexString() << tab << SelectedTag() << tab << LemmaString() << std::endl;
    else
      Message(MSG_WARNING, "no word");
    return;
  }

  bool printTag = xPrintSelectedTag;

  const Word *w = GetWord();
  if (w) {
    if (xPrintHTML)
      out << Str2html(RealString());
    else
      PrettyPrintTextString(out);
    
    if (xPrintWordInfo) {
      out << " [";
      if (w->IsNewWord())
	((NewWord*)w)->PrintInfo(out);
      else
	w->PrintInfo(out);
      out << ' ' << GetToken() << ']';
      if (IsFirstInSentence())
	out << '~';
    }
  } else
    out << "(NULL word-token)";
  if (printTag) {
    if(xPrintAllWordTags) {
      w->PrintTags();
    } else 
      out << tab << SelectedTag() << tab;

  }
  if (xPrintLemma) {
    out << LemmaString() << tab;
  }
  if (xPrintOneWordPerLine || xPrintWordInfo)
    out << xEndl;
  else if (printSpace && HasTrailingSpace())
    out << ' ';
}

void WordToken::PrintVerbose(std::ostream& out) const {
  out << '[';
  PrettyPrintTextString(out);
  out << "]"
      << (GetWord()->IsNewWord() ? "* " : " ") 
      << " [" << LexString() << "] "
      << Token2String(token) << ' ' 
      << offset << std::endl;
}

void WordToken::PrettyPrintTextString(std::ostream& out) const {
  if(RealString()) // jonas, program crashes when RealString() is null otherwise
    for (const char *s = RealString(); *s; s++)
      if (IsSpace(*s)) {
	out << ' ';
	while(IsSpace(*(s+1)))
	  s++;
      } else
	out << *s;
}
  
