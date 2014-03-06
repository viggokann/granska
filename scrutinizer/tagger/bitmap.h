/* bitmap.hh
 * author: Johan Carlberger
 * last change: 2000-03-21
 * comments: BitMap128, BitMap65536 classes
 */

#ifndef _bitmap_hh
#define _bitmap_hh

#include "basics.h"
#include "ensure.h"
#include "file.h"
//#include <iostream>
//#include <fstream>

static const int N_128_FIELDS = 8;

class BitMap128 {
public:
  static int Size() { return N_128_FIELDS * 16; }
  void SetBit(int b) {
    ensure(b<Size());
    Bits(b>>4)|=Mask(b&15);
  }
  void UnSetBit(int b) {
    ensure(b<Size());
    Bits(b>>4)&=(Mask(b&15)^0xFFFF);
  }
  inline bool GetBit(int b) const {
    ensure(b<Size());
    return (Bits(b>>4)&Mask(b&15)) != 0;
  }
  void Clear() {
    for (int i=0; i<N_128_FIELDS; i++) Bits(i) = 0;
  }
private:
  ushort& Bits(int n) { return bits[n]; }
  const ushort& Bits(int n) const { return bits[n]; }
  ushort Mask(int n) const { return mask[n]; }
  static ushort mask[16];
  ushort bits[N_128_FIELDS];
};

class BitMap65536 {
public:
  static int Size() { return 65536; }
  void SetBit(int b) {
    ensure(b<Size());
    Bits(b>>4)|=Mask(b&15);
  }
  void UnSetBit(int b) {
    ensure(b<Size());
    Bits(b>>4)&=(Mask(b&15)^0xFFFF);
  }
  inline bool GetBit(int b) const {
    ensure(b<Size());
    return (Bits(b>>4)&Mask(b&15)) != 0;
  }
  void Clear() {
    for (int i=0; i<Fields(); i++)
      Bits(i) = 0;
  }
private:
  static int Fields() { return 4096; }
  ushort& Bits(int n) { return bits[n]; }
  const ushort& Bits(int n) const { return bits[n]; }
  ushort Mask(int n) const { return mask[n]; }
  static ushort mask[16];
  ushort bits[4096];
};

#endif




