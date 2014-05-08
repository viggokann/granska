/* heap.hh
 * author: Johan Carlberger
 * last change: 990701
 * comments:
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

#ifndef _heap_hh
#define _heap_hh

static inline int Parent(int i) { ensure(i>0); return i/2; }
static inline int Left(int i) { ensure(i>0); return 2*i; }
static inline int Right(int i) { ensure(i>0); return 2*i+1; }

template <class T, class V>
inline void Heapify(T *a, const int n, int i, const V (*value)(const T)) {
  for(;;) {
    int l = Left(i);
    int min = (l <= n && (*value)(a[l]) < (*value)(a[i])) ? l : i;
    int r = Right(i);
    if (r <= n && (*value)(a[r]) < (*value)(a[min]))
      min = r;
    if (min != i) {
      Swap(a[i], a[min]);
      i = min;
    } else
      break;
  }
}

template <class T, class V>
inline void BuildHeap(T *a, const int n, const V (*value)(const T)) {
  for (int i=n/2; i>0; i--)
    Heapify(a, n, i, value);
}

template <class T>
inline void PrintHeap(T *a, const int n) {
  std::cout << " ";
  for (int i=1; i<=n; i++)
    std::cout << a[i] << ' ';
  std::cout << std::endl;
}

template <class T, class V>
inline void HeapSort(T *a, const int n, const V (*value)(const T)) {
  int m = n-1;
  for (int i=n; i>1; i--) {
    Swap(a[1], a[i]);
    Heapify(a, m--, 1, value);
  }
}

// a not so simple proc to select the best few n from array a.
// the heap procs are called with best-1 because index must run from 1..n
template <class T, class V>
inline void SelectBest(const T *a, const int aSize, T *best, const int n, const V (*value)(const T)) {
  ensure(n >= 1);
  ensure(n <= aSize);
  for (int i=0; i<n; i++)
    best[i] = a[i];
  BuildHeap(best-1, n, value);
  V worst = (*value)(best[0]);
  for (int i=n; i<aSize; i++) 
    if ((*value)(a[i]) > worst) {
      best[0] = a[i];
      Heapify(best-1, n, 1, value);
      worst = (*value)(best[0]);
    }
  //  HeapSort(best-1, n, value);
}

#endif




