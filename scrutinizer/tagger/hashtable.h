/* hashtable.hh
 * author: Johan Carlberger
 * last change: 20051230, minor bug-fix, Oscar Täckström
 * comments: dynamic hashtable, WARNING some ensures commented
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

#ifndef _hashtable_hh
#define _hashtable_hh

#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <iosfwd>
#include "basics.h"
#include "ensure.h"

// this should be in HashTable: const uint MAGNIFYING_FACTOR = 2;

static const int NO_LINK = -1;
static const int DEFAULT_SIZE = 2048; 

template <class T>
class HashTable {
public:
  typedef uint (*keyFunc)(const T&);
  typedef uint (*strKeyFunc)(const char*);
  typedef int (*compareFunc)(const T&, const T&);
  typedef int (*strCompareFunc)(const char*, const T&);
  HashTable() { NewObj(); }
  HashTable(keyFunc k, compareFunc c, strKeyFunc skf, strCompareFunc scf,
	    int s = DEFAULT_SIZE, float t = float(2)) {
    Init(k, c, skf, scf, s, t); NewObj();
  }
  ~HashTable() { 
    delete [] slots; // jonas, delete -> delete []
    delete [] links;  // jonas, delete -> delete []
    DelObj(); 
    ExtByt(-(max+1) * sizeof(T*)); ExtByt(-(max+1) * sizeof(int));
  }
  void Init(keyFunc k, compareFunc c, strKeyFunc skf, strCompareFunc scf,
	    int s = DEFAULT_SIZE, float t = float(2)) {
    key = k;
    compare = c;
    strKey = skf;
    strCompare = scf;
    threshold = t;
    Initialize(s);
  }
  void Clear() {
    nObjects = 0;
    int j;
    for (j=0; j<end; j++) {
      slots[j] = NULL;
      links[j] = NO_LINK;
    }
    for (;j<=max; j++) {
      ensure(slots[j] == NULL);
      ensure(links[j] == NO_LINK);
    }
  }
  void DeleteAndClear() {
    nObjects = 0;
    int j;
    for (j=0; j<end; j++) {
      if (slots[j])
	delete slots[j];
      slots[j] = NULL;
      links[j] = NO_LINK;
    }
    for (;j<=max; j++) {
      ensure(slots[j] == NULL);
      ensure(links[j] == NO_LINK);
    }
  }
  
  void PrintObjects(std::ostream& os = std::cout) const;
  const T* Insert(const T *t);
  T *Find(const T *t) const;
  T *Find(const char*) const;
  T *Remove(const T *t); // NOTE, compared by pointers
  void Resize(int s = 0);
  T *operator[](int *n) const;
  int NObjects() const { return nObjects; }
private:
  uint Key(const T &t) const { return (*key)(t)&mask; }
  uint Key(const char *s) const { return (*strKey)(s)&mask; }
  void Initialize(int s);
  keyFunc key;
  compareFunc compare;
  strKeyFunc strKey;
  strCompareFunc strCompare;
  const T **slots;
  int *links;
  int size;
  int end;
  uint mask;
  int nObjects;
  int max;
  float threshold;
  DecObj();
};

template <class T>
T *HashTable<T>::operator[](int *n) const {
  ensure(*n <= end);
  if (*n < end)
    while (!slots[*n] && *n < end)
      (*n)++;
  //else
  //  ensure(!slots[*n]);
  return (T*) slots[*n];
}

template <class T>
void HashTable<T>::PrintObjects(std::ostream& os) const {
  ensure(this);
  for (int i=0; i<end; i++)
      if (slots[i]) os << slots[i] << std::endl;
}

template <class T>
const T *HashTable<T>::Insert(const T *g) {
  ensure(g);
  // std::cout << name << ":inserting " << g << std::endl;
  nObjects++;
  uint s = Key(*g);
  if (slots[s]) {
    slots[end] = g;
    links[end] = links[s];
    links[s] = end;
    end++;
    if (end >= max)
      Resize();
  } else
    slots[s] = g;
  return g;
}

template <class T>
T *HashTable<T>::Find(const T *t) const {
  // std::cout << name << ": finding " << t << std::endl;
  for (int i = Key(*t); i != NO_LINK; i = links[i]) {
    //ensure(i<end);
    const T *p = slots[i];
    if (p && !(*compare)(*t, *p))
      return (T*) p;
  }
  return NULL;
}

template <class T>
T *HashTable<T>::Find(const char *s) const {
  ensure(strCompare);
  // std::cout << name << ": finding " << t << std::endl;
  for (int i = Key(s); i != NO_LINK; i = links[i]) {
    //ensure(i<end);
    const T *p = slots[i];
    if (p && !(*strCompare)(s, *p))
      return (T*) p;
  }
  return NULL;
}

template <class T>
void HashTable<T>::Resize(int newSize) {
  ensure(size > 0);
  int oldEnd = end;
  const T **oldSlots = slots;
  int *oldLinks = links;
  if (!newSize)
    newSize = size * 2; // MAGNIFYING_FACTOR;
  ExtByt(-(max+1) * sizeof(T*));
  ExtByt(-(max+1) * sizeof(int));
  Initialize(newSize);
  for (int i=0; i<oldEnd; i++)
    if (oldSlots[i])
      Insert(oldSlots[i]);
  delete [] oldSlots; // jonas, delete -> delete []
  delete [] oldLinks; // jonas, delete -> delete []
}

template <class T>
void HashTable<T>::Initialize(int s) {
  /*  std::cout << "initializing hash table" << name
      << " Objects: " << nObjects
      << " size: " << s;
      */
  if (s <= 0) s = 10;
  size = 0;
  nObjects = 0;
  for (int i=(sizeof(int)*8)-1; i>=0; i--)
    if ((s >> i) & 1)
      if (size) {
	size *= 2;
	break;
      } else
	size = s;
  mask = size-1;
  end = size;
  max = int(threshold*size);
  slots = new const T*[max+1];
  ExtByt((max+1) * sizeof(T*));
  links = new int[max+1];
  ExtByt((max+1) * sizeof(int));
  for (int j=0; j<=max; j++) {
    slots[j] = NULL;
    links[j] = NO_LINK;
  }
}

template <class T>
T *HashTable<T>::Remove(const T *t) {
  int prev = NO_LINK;
  for (int i=Key(*t); i != NO_LINK; i = links[i]) {
    if (slots[i] == t) {
      slots[i] = NULL;
      if (prev != NO_LINK)
	links[prev] = links[i];
      nObjects--;
      return (T*) t;
    }
    prev = i;
  }
  return NULL;
}


#endif






