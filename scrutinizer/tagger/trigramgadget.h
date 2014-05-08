/* trigramgadget.hh
 * author: Johan Carlberger
 * last change: 990927
 * comments: A gadget to use for the dynamic programming tagging algorithm
 *           Used only by Tagger
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

#ifndef _trigramgadget_hh
#define _trigramgadget_hh

#include "settings.h"

class TrigramGadget {
  friend class Tagger;
private:
  TrigramGadget() {Reset(); } // Reset(); make sure this has no effect on tagging performance
  void Reset();
  void Normalize(int m, int n);
  void SetTags(const Word*);
  int n;
  int selected;
  Tag *tag[MAX_WORD_VERSIONS];
  probType lexProb[MAX_WORD_VERSIONS];
  probType prob[MAX_WORD_VERSIONS][MAX_WORD_VERSIONS];
  char prev[MAX_WORD_VERSIONS][MAX_WORD_VERSIONS];      // using char instead of int seemed faster on UNIX 981012
};

inline void TrigramGadget::Reset() {
  for (int i=0; i<MAX_WORD_VERSIONS; i++) {
    tag[i] = NULL;
    lexProb[i] = 0;
    for (int j=0; j<MAX_WORD_VERSIONS; j++) {
	prev[i][j] = -1;
	prob[i][j] = -1;
    }
    n=0;
  }
  selected = -1;
}

inline void TrigramGadget::Normalize(int m, int p) {
  probType sum = 0;
  int i;
  for (i=0; i<m; i++)
    for (int j=0; j<p; j++)
      sum += prob[i][j];
  for (i=0; i<m; i++)
    for (int j=0; j<p; j++)
      prob[i][j] /= sum;
}

inline void TrigramGadget::SetTags(const Word *w) {
  n = 0;
  for (const WordTag *q=w; q; q=q->Next()) {
    tag[n] = q->GetTag();
    prob[0][n] = q->LexProb();
    n++;
  }
}
    
#endif
