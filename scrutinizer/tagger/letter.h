/* letter.hh
 * author: Johan Carlberger
 * last change: 2000-01-19
 * comments: letter and string manipulation routines
 */

#ifndef _letter_hh
#define _letter_hh

#include <iosfwd>
#include <string.h>
#include <ctype.h>
#include "basics.h"

extern const char letters[256];                   
extern const uchar uppers[256];
extern const uchar lowers[256];

const char P = -4; // punctuation except sentence terminators
const char E = -3; // sentence terminators
const char S = -2; // space and nonprintables
const char D = -1; // digits
const char C = 1;  // consonants
const char V = 2;  // vowels

void InitEditDistance();
int EditDistance(const char*, const char*);
void doLetterTest();

const char *Str2html(const char*);
void SpaceFix(char*);
void Space2Punct(char*);
void PunctFix(char*);
inline bool IsPrint(char c) { return letters[(uchar) c] != 0; }
inline bool IsDigit(char c) { return letters[(uchar) c] == D; }
inline bool IsPunct(char c) { return letters[(uchar) c] <= E; }
inline bool IsSpace(char c) { return letters[(uchar) c] == S; }
inline bool IsLetter(char c) { return letters[(uchar) c] >= C; }
inline bool IsUpper(char c) { return uppers[(uchar) c] == (uchar) c && IsLetter(c); }
inline bool IsLower(char c) { return lowers[(uchar) c] == (uchar) c && IsLetter(c); }
inline bool IsVowel(char c) { return letters[(uchar) c] == V; }
inline bool IsConsonant(char c) { return letters[(uchar) c] == C; }
inline bool IsEnder(char c) { return letters[(uchar) c] == E; }
inline void Capitalize(char *s) { s[0] = uppers[(uchar) s[0]]; }
inline char Lower(char c) { return (char) lowers[(uchar) c]; }
inline char Upper(char c) { return (char) uppers[(uchar) c]; }
inline bool IsForeign(char c) { return c == 'Ø' || c == Lower('Ø'); } // more to do

inline bool ContainsVowel(const char *string) { // true if string has at least one vowel
  for (;*string; string++)
    if (IsVowel(*string))
      return true;
  return false;
}

inline bool ContainsDigit(const char *string) { // true if string has at least one vowel
  for (;*string; string++)
    if (IsDigit(*string))
      return true;
  return false;
}

inline void ToLower(char *string) {
  while ((*string = (char) lowers[(uchar) *string]) != '\0')
    string++;
}  

inline void ToUpper(char *string) {
  while ((*string = (char) uppers[(uchar) *string]))
    string++;
}  

inline int StrCaseCmp(const char *s1, const char *s2) {
  for (; *s1 && Lower(*s1) == Lower(*s2); s1++, s2++); 
  return *s1 - *s2;
}

inline void Reverse(char *string) {
  int i=0;
  for (int j=strlen(string)-1; i<j; i++, j--)
    Swap(string[i], string[j]);
}  

#endif




