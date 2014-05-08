/* wordsuggestions.hh
 * author: Johan Carlberger
 * last change: 990104
 * comments: WordSuggestions class
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

#ifndef _wordsuggestions_hh
#define _wordsuggestions_hh

#include "basics.h"
#include "predictorsettings.h"

const int MAX_ALL_SUGGESTED = 1000;

class WordSuggestions {
public:
  WordSuggestions() { SetNWanted(5); nSuggested = 0; }
  void SetNWanted(int n) { nWanted = n; }
  void AddCandidate(Word*w, probType prob);
  void Clear(const char *p) { prefix = p; nFound = 0; worstProb = 0; }
  void ClearAllSuggested() { 
    for (int i=0; i<nSuggested; i++)
      allSuggested[i]->suggested = 0;
    nSuggested = 0;
  }
  void Sort();
  bool Contains(const Word *w) const;
  void Print(std::ostream &out) const;
  // void PrintHTML(std::ostream &out) const;
  const Word *operator[](int n) const { return words[n]; }
  int NFound() const { return nFound; }
  int NWanted() const { return nWanted; }
  probType WorstProb() { return worstProb; }
private:
  void FindWorst();
  int nWanted;
  int nFound;
  int nSuggested;
  probType worstProb;
  int worstIndex;
  const char *prefix;
  Word *words[MAX_WORD_SUGGESTIONS];
  probType probs[MAX_WORD_SUGGESTIONS];
  Word *allSuggested[MAX_ALL_SUGGESTED];
};

inline void WordSuggestions::AddCandidate(Word *w, probType prob) {
  ensure(!w->IsSuggested());
  ensure(prob >= worstProb);
  if (nFound < nWanted) {
    words[nFound] = w;
    probs[nFound] = prob;
    nFound++;
    if (nFound == nWanted)
      FindWorst();
    return;
  }
  words[worstIndex] = w;
  probs[worstIndex] = prob;
  FindWorst();
}

inline void WordSuggestions::FindWorst() {
  worstProb = probs[0];
  worstIndex = 0;
  for (int i=1; i<nFound; i++)
    if (probs[i] < worstProb) {
      worstProb = probs[i];
      worstIndex = i;
    }
}

inline void WordSuggestions::Sort() {
  if (!xRepeatSuggestions && nSuggested < MAX_ALL_SUGGESTED-MAX_WORD_SUGGESTIONS)
    for (int i=0; i<nFound; i++) {
      words[i]->suggested = 1;
      allSuggested[nSuggested++] = words[i];
    }
  switch (xSortMode) {
  case NONE:
    return;
  case ALPHA:
    for (int j=nFound; j>1; j--)
      for (int i=1; i<j; i++)
	if (strcmp(words[i-1]->String(), words[i]->String()) > 0) {
	  Swap(words[i-1], words[i]);
	  Swap(probs[i-1], probs[i]);
	}
    break;
  case LENGTH:
    for (int j=nFound; j>1; j--)
      for (int i=1; i<j; i++) {
	int cmp = words[i-1]->StringLen() - words[i]->StringLen();
	if (cmp > 0 || (cmp == 0 && strcmp(words[i-1]->String(), words[i]->String()) > 0)) {
	  Swap(words[i-1], words[i]);
	  Swap(probs[i-1], probs[i]);
	}
      }
    break;
  case PROB:
    for (int j=nFound; j>1; j--)
      for (int i=1; i<j; i++)
	if (probs[i-1] < probs[i]) {
	  Swap(words[i-1], words[i]);
	  Swap(probs[i-1], probs[i]);
	}
    break;
  }
}

inline bool WordSuggestions::Contains(const Word *w) const {
  for (int i=0; i<NFound(); i++)
    if (w == words[i])
      return true;
  return false;
}

inline void WordSuggestions::Print(std::ostream &out) const {
  if (nFound == 0)
    out << "---" << std::endl;
  else {
    int maxLen = 0;
    for (int i=0; i<nFound; i++) {
      int len = words[i]->StringLen();
      if (len > maxLen)
	maxLen = len;
    }
    for (int i=0; i<nFound; i++) {
      out << i+1 << ' ' << prefix
	  << words[i]->String()+strlen(prefix);
      int spaces = maxLen - words[i]->StringLen() + 2;
      for (int j=0; j<spaces; j++)
	out << ' ';
      out << probs[i];
      out << std::endl;
    }
  }
}

inline std::ostream& operator<<(std::ostream& os, const WordSuggestions &s) {
  //  if (xPrintHTML) s.PrintHTML(os); else 
  s.Print(os);
  return os;
}
inline std::ostream& operator<<(std::ostream& os, const WordSuggestions *s) {
  if (s) os << *s; else os << "(null WordSuggestions)"; return os;
}

#endif
