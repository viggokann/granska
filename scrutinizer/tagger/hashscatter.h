/* hashscatter.hh
 * author: Johan Carlberger
 * last change: 2000-05-16
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
