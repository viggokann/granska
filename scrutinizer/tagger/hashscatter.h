/* hashscatter.hh
 * author: Johan Carlberger
 * last change: 2000-05-16
 * comments:
 */

#ifndef _hashscatter_hh
#define _hashscatter_hh

extern const unsigned long hashScatter[256];

inline unsigned int Scatter(unsigned char c) {
  return hashScatter[c];
}

inline unsigned int Hash(const char *s) {
  unsigned int val = 0;
  for (; *s; s++)
    val = (val >> 1) ^ Scatter(*s);  // + or ^, which is fastest?
  return val;
}

void CheckHash();

#endif
