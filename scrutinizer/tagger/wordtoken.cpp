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
  int caps = 0;
  int nots = 0;

  int internal = 0;
  const char *start = s;
  
  if (IsUpper(*s)) {
    firstCapped = 1;
  }
  
  allCapped = 1;
  for(; *s; s++) {
    if (IsLower(*s)) {
      allCapped = 0;
      nots++;
    } else if(IsUpper(*s)) {
      caps++;

      if(s > start) {
	if(*(s - 1) != ' '
	   && *(s - 1) != '.'
	   && *(s - 1) != '-') {
	  internal++;
	}
      }
      
    }
    
    if(*s == '-' || *s == ':') {
      hyphen = 1;
    }
  }

  /*
  if(nots > 0 && (caps > 1 || (caps > 0 && !firstCapped))) {
    manyCapped = 1;
  }
  */
  if(internal > 0 && !allCapped) {
    manyCapped = 1;

    if(caps == 2 && *start == 'M' && *(start + 1) == 'c' && IsUpper(*(start+2))) {
      manyCapped = 0;
    }
    if(caps == 2 && *start == 'M' && *(start + 1) == 'a' && *(start + 2) == 'c' && IsUpper(*(start+3))) {
      manyCapped = 0;
    }
    if(caps == 2 && *start == 'D' && *(start + 1) == 'e' && IsUpper(*(start+2))) {
      manyCapped = 0;
    }
    if(caps == 2 && *start == 'D' && *(start + 1) == 'i' && IsUpper(*(start+2))) {
      manyCapped = 0;
    }
    if(caps == 2 && *start == 'L' && *(start + 1) == 'a' && IsUpper(*(start+2))) {
      manyCapped = 0;
    }
    if(caps == 2 && *start == 'L' && *(start + 1) == 'e' && IsUpper(*(start+2))) {
      manyCapped = 0;
    }
    if(nots == 1 && (*(s-1) == 's' || *(s-1) == 'e' || *(s-1) == 't' || *(s-1) == 'n')) {
      manyCapped = 0;
    } 

    /*
    std::cerr << caps << " "
	      << nots << " "
	      << internal << ", "
	      << start << ", '"
	      << *(start) << "' '"
	      << *(start+1) << "' '"
	      << *(start+2) << "', "
	      << IsUpper(*(start+2)) << std::endl;
    */
  }
}

bool WordToken::AllCappedAgain() const {
  const char *s = string;

  for(; *s; s++) {
    if (!IsUpper(*s)) {
      return 0;
    }
  }
  return 1;
}

bool WordToken::ManyCappedAgain() const {
  const char *s = string;
  int caps = 0;
  int nots = 0;
  int all = 1;

  int internal = 0;
  
  for(; *s; s++) {
    if (IsLower(*s)) {
      all = 0;
      nots++;
    } else if(IsUpper(*s)) {
      caps++;

      if(s > string) {
	if(*(s - 1) != ' '
	   && *(s - 1) != '.'
	   && *(s - 1) != '-') {
	  internal++;
	}
      }
      
    }
  }

  /*
  if(nots > 0 && (caps > 1 || (caps > 0 && !firstCapped))) {
    return 1;
  }
  */
  if(internal > 0 && !all) {
    if(caps == 2 && *string == 'M' && *(string + 1) == 'c' && IsUpper(*(string+2))) {
      return 0;
    }
    if(caps == 2 && *string == 'M' && *(string + 1) == 'a' && *(string + 2) == 'c' && IsUpper(*(string+3))) {
      return 0;
    }
    if(caps == 2 && *string == 'D' && *(string + 1) == 'e' && IsUpper(*(string+2))) {
      return 0;
    }
    if(caps == 2 && *string == 'D' && *(string + 1) == 'i' && IsUpper(*(string+2))) {
      return 0;
    }
    if(caps == 2 && *string == 'L' && *(string + 1) == 'a' && IsUpper(*(string+2))) {
      return 0;
    }
    if(caps == 2 && *string == 'L' && *(string + 1) == 'e' && IsUpper(*(string+2))) {
      return 0;
    }
    if(nots == 1 && (*(s-1) == 's' || *(s-1) == 'e' || *(s-1) == 't' || *(s-1) == 'n')) {
      return 0;
    }
    
    return 1;
  }
  return 0;
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
  if(RealString()) { // jonas, program crashes when RealString() is null otherwise
    for (const char *s = RealString(); *s; s++) {
      if (IsSpace(*s)) {
	out << ' ';
	while(IsSpace(*(s+1))) {
	  s++;
	}
      } else {
	out << *s;
      }
    }
  }
}
  
