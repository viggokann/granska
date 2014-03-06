/* basics.hh
 * author: Johan Carlberger
 * last change: 2000-03-14
 * comments:
 */

#ifndef _basics_hh
#define _basics_hh

#include <stdio.h>
#include "ensure.h"

typedef const char cchar;
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned long Bit;

const char tab = '\t';

inline const char *int2str(int a) { // not good
  static char string[20];
  sprintf(string, "%i", a);
  return string;
}

template <class T>
inline T MaxOf(T a, T b) { return (a > b) ? a : b; }

template <class T>
inline T MinOf(T a, T b) { return (a < b) ? a : b; }

template <class T>
inline void Swap(T& a, T& b) { T tmp77 = a; a = b; b = tmp77; }

template <class T>
inline T Abs(T a) { if (a < T(0)) return -a; return a; }

inline void neg(bool &a) { a = !a; }
inline void neg(int &a) { a = !a; }

inline char *dont(bool a) { if (a) return " don't "; else return " "; }
inline char *noOrNuff(bool a) { if (a) return ""; else return "no "; }
inline char optS(bool a) { if (a) return 's'; else return '\0'; }

#ifdef COUNT_OBJECTS
#include <iostream>

#define DecObj() public: static int nObjects; static int nExtraBytes
#define DefObj(ClassName) int ClassName::nObjects = 0; int ClassName::nExtraBytes = 0
#define NewObj() nObjects++
#define DelObj() nObjects--
#define ExtByt(n) nExtraBytes += (n)
#define NBytes(ClassName) (ClassName::nExtraBytes + sizeof(ClassName)*(ClassName::nObjects < 0 ? 0 : ClassName::nObjects))
#define PriObj(ClassName) std::cout << ClassName::nObjects << ' ' << #ClassName << optS(ClassName::nObjects!=1) << " = " << (NBytes(ClassName))/1024 << " k" << std::endl
#else
#define DecObj()
#define DefObj(ClassName)
#define NewObj()
#define DelObj()
#define PriObj(ClassName)
#define ExtByt(n)
#define NBytes(ClassName)
#endif

#endif




