/* tagtrigram.hh
 * author: Johan Carlberger
 * last change: 2000-03-27
 * comments: TagTrigram class
 */

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

#ifdef OLDDEFINITIONOFTAGTRIGRAMBEFOREPROBCHECK
  union {
    float prob;
    int freq;
  } pf;
#else
  struct {
    float prob;
    int freq;
  } pf;
#endif
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


