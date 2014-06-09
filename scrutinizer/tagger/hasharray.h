/* hasharray.hh
 * author: Johan Carlberger
 * last change: 2000-05-08
 * comments: A fixed size template hashtable class
 *           Use carefully.
 *           don't forget to hashify before using Find()
 *           don't move the body of Init() outside the declaration of the class,
 *           unless you how to do it (the typedefs makes it tricky, I guess)
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

#ifndef _hasharray_hh
#define _hasharray_hh

#include <iostream>
#include <iosfwd>
#include <string.h>
#include "basics.h"
#include "ensure.h"
#include "file.h"
#include "message.h"

template <class T>
class HashArray {
public:
  typedef uint (*keyFunc)(const T&);
  typedef uint (*strKeyFunc)(const char*);
  typedef int (*compareFunc)(const T&, const T&);
  typedef int (*strCompareFunc)(const char*, const T&);
  HashArray() : array(0), links(0), size(0), hashified(0), name(0) { NewObj(); }
  ~HashArray();
  void Init(const char *n, int s, compareFunc c, keyFunc k,
	    compareFunc r, strCompareFunc scf, strKeyFunc sk) {
    name = n;
    if (s < 1)
      Message(MSG_ERROR, "trying to initialize hash array", name, "to size", int2str(s));
    compare = c;
    key = k;
    strCompare = scf;
    strKey = sk;
    rank = r;
    size = s;
    uint one;
    if (size > 1) {
      for (one = 0x40000000; !((size-1) & one); one >>= 1);
      mask = 2*one-1;
      mask2 = one-1;
    } else
      mask = mask2 = 0;

    // jb: where is this memory freed when allocated in HashArray<Tag>?
    array = new T[size]; // assume bytes for T counted elsewhere
    ensure(array);
    Message(MSG_STATUS, "initializing hash array", name);
    links = new int[size]; // new OK
    ensure(links);
    ExtByt(size *sizeof(int));
  }

  int Size() const { return size; }
  T &operator[](int n) const {
    if (n >= size)
      Message(MSG_ERROR, "hash array", name, "out of bounds", int2str(n));
    ensure(n < size);
    return array[n];
  }
  T &Element(int n) const { ensure(n < size); return array[n]; }
  T *Find(const T&) const;
  T *Find(const char*) const;
  void Hashify(bool cluster = true);
  bool IsHashified() const { return hashified; }
  void Statistics() const;
  void Load(std::ifstream &in);
  void Store(std::ofstream &out) const;
protected:
  //T* Array() const { return array; } 
private:
  int Key(const T &t) const;
  int Key(const char*) const;
  T *array;
  int* links;
  int size;
  uint mask;
  uint mask2;
  int hashified;
  compareFunc compare;
  keyFunc key;
  strCompareFunc strCompare;
  strKeyFunc strKey;
  compareFunc rank;
  const char *name;
  DecObj();
};

template <class T>
HashArray<T>::~HashArray() {
// jb: purify claims mem leak
  if (size)
  { 
	  Message(MSG_STATUS, "deleting", name, "hash array...");
      if (strcmp(name, "tags"))    // jb: why is this?
	  delete [] array; 
      delete [] links;		    // jbfix: delete => delete []
      ExtByt(-size*sizeof(int));
  }
  DelObj();
}

template <class T>
int HashArray<T>::Key(const T &t) const {
  int k = (*key)(t)&mask;
  return (k < size) ? k : k&mask2;
}

template <class T>
int HashArray<T>::Key(const char *s) const {
  int k = (*strKey)(s)&mask;
  return (k < size) ? k : k&mask2;
}

template <class T>
T *HashArray<T>::Find(const T &e) const {
  ensure(hashified);
  for (int i = Key(e); ;i = links[i]) {
    if (!(*compare)(e, array[i]))
      return array + i;
    if (i == links[i])
      return NULL;
  }
  return NULL; // unreachable
}

template <class T>
T *HashArray<T>::Find(const char *s) const {
  ensure(hashified);
  for (int i = Key(s); ;i = links[i]) {
    if (!(*strCompare)(s, array[i]))
      return array +i;
    if (i == links[i])
      return NULL;
  }
  return NULL; // unreachable
}

template <class T>
void HashArray<T>::Hashify(bool cluster) {
  hashified = 1;
  int i;
  //  std::cerr << "hashifying " << name << ' ' << size << "..." << std::endl;

  // put as many elements as possible in correct position:
  for (i=0; i<size;) {
    int n = Key(array[i]);
    int m = Key(array[n]);
    if (m != n)
      Swap(array[i], array[n]);
    else i++;
  }
  
  if (cluster) {
    //    extern const int clusterLimit;
    int nBad = 0;
    for (i=0; i<size; i++)
      if (Key(array[i]) != i)
	links[nBad++] = i;
    /*    if (rank)
      for (int j=nBad-1; j>=0; j--)
	for (i=0; i<j; i++)
	  if (rank(array[links[i]], array[links[i+1]]) < 0)
	  Swap(links[i], links[i+1]); */
    int D;
    for (D=1; nBad > 1 && D<200; D++) {
      for (i=0; i<nBad;) {
	int m = links[i];
	int g = m%2 ? 1 : -1;
	for (int a=0; a<2; a++) {
	  int j = Key(array[m]) + g*D;
	  if (j < size && j>=0) {
	    int d = Abs(Key(array[j]) - j);
	    if (d > D) {
	      Swap(array[m], array[j]);
	      links[i] = links[--nBad];
	      goto next;
	    }
	  }
	  g *= -1;
	}
	i++;
      next:;
      }
      //      if (D == 1 || D%200 == 0)
      //	std::cout << 100.0*(size-nBad)/size << "% = " << (size-nBad) << " out of "
      //	     << size << " within " << D << std::endl;
    }
    //    std::cout << 100.0*(size-nBad)/size << "% = " << (size-nBad) << " out of "
    //	 << size << " within " << D << std::endl;
  }
  
    // set all links:
  for (i=0; i<size; i++)
    links[i] = i;
  for (i=0; i<size; i++) {
    int n = Key(array[i]);
    if (n != i) {                     // array[i] is in wrong place
      if (links[n] != n)              // link from n exists
	links[i] = links[n];
      links[n] = i;
    }
  }

  // sort linked elements by rank (high rank -> more likely to be requested):
  if (rank) {
    bool swapped;
    for (i=0; i<size; i++)
      if (Key(array[i]) == i)
	do {
	  swapped = false;
	  for (int j=i; links[j] != j; j = links[j])
	    if (rank(array[j], array[links[j]]) < 0) {
	      //std::cout << "swapping " << array[j] << " and " << array[links[j]] << std::endl;
	      Swap(array[j], array[links[j]]);
	      swapped = true;
	    }
	} while (swapped);
  }
}

template <class T>
void HashArray<T>::Statistics() const {
  std::cout << name << " hash array of size " << size << ", statistics:" << std::endl;
  std::cout.precision(2);
  ensure(hashified);
  int good = 0;
  int collisions = 0;
  int maxCollisions = 0;
  int gap = 0, maxGap = 0;
  for (int i=0; i<size; i++) {
    if (Key(array[i]) == i) {
      good++; gap = 0;
    } else {
      gap++;
      if (gap >maxGap) maxGap = gap;
    }
    int c = 1;
    for (int j=i; links[j] != j; j = links[j]) {
      if (c > size)
	Message(MSG_ERROR, "infinite loop in hasharray");
      c++;
    }
    collisions += c;
    if (c > maxCollisions) maxCollisions = c;
  }
  std::cout << tab << size << " slots == #elements" << std::endl
       << tab << collisions << " collisions" << std::endl
       << tab << 100.0*good/size << " % slot coverage" << std::endl
       << tab << (0.0+collisions+good) /size << " average collisions" << std::endl
       << tab << maxCollisions << " maximum collisions" << std::endl
       << tab << maxGap << " maximum gap" << std::endl;
  if (maxGap > 50) Message(MSG_WARNING, "hash function seems bad");
}

template <class T>
void HashArray<T>::Load(std::ifstream &in) {
  int oldSize = size;
  ReadVar(in, size);
  if (oldSize != size)
    Message(MSG_ERROR, "while loading hasharray");
  ReadVar(in, hashified);
  ensure(hashified);
  ReadData(in, (char*)array, size*sizeof(T), "array");
  ReadData(in, (char*)links, size*sizeof(int), "links");
}

template <class T>
void HashArray<T>::Store(std::ofstream &out) const {
  WriteVar(out, size);
  WriteVar(out, hashified);
  WriteData(out, (char*) array, size*sizeof(T), "array");
  WriteData(out, (char*) links, size*sizeof(int), "links");
}  

#endif









