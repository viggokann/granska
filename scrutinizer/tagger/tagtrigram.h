/* tagtrigram.hh
 * author: Johan Carlberger
 * last change: 2000-03-27
 * comments: TagTrigram class
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

#ifndef _tagtrigram_hh
#define _tagtrigram_hh

#include <iostream>
#include "basics.h"
#include "ensure.h"
#include "hashscatter.h"

class TagTrigram {
public:
  TagTrigram() { NewObj(); }
  ~TagTrigram() { DelObj(); }
  TagTrigram(uchar t1, uchar t2, uchar t3) : tag1(t1), tag2(t2), tag3(t3) { NewObj(); }
  //  uint Key() const { return tag1 ^ (tag2<<4) ^ (tag3<<9); }
  uchar tag1;
  uchar tag2;
  uchar tag3;
  uchar nothing;

#ifdef PROBCHECK
  struct {
    float prob;
    int freq;
  } pf;
#else  // PROBCHECK
  union {
    float prob;
    int freq;
  } pf;
#endif // PROBCHECK
  DecObj();
};

inline std::ostream& operator<<(std::ostream& os, const TagTrigram &t) {
  os << (int)t.tag1 << ' ' << (int)t.tag2 << ' ' << (int)t.tag3 << ' ' << t.pf.prob;
  return os;
}
inline std::ostream& operator<<(std::ostream& os, const TagTrigram *t) {
  if (t) os << *t;
  else os << "(null)";
  return os;
}
inline int CompareTagTrigrams(const TagTrigram &t1, const TagTrigram &t2) {
  return t1.tag1 != t2.tag1 || 
    t1.tag2 != t2.tag2 || 
    t1.tag3 != t2.tag3;
}
inline int RankTagTrigrams(const TagTrigram &t1, const TagTrigram &t2) {
  return t1.pf.freq - t2.pf.freq;
}

inline uint KeyTagTrigram(const TagTrigram &t) {
  //  return t.key;   not faster
  return t.tag3 ^ (t.tag2<<4) ^ (t.tag1<<10);
}

// better spread, but slower !
/*
inline uint KeyTagTrigram(const TagTrigram &t) {
  return Scatter(t.tag1) ^ (Scatter(t.tag2)>>1) ^ (Scatter(t.tag3)>>2);
}
*/

#endif


