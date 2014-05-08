/* morf.hh
 * author: Johan Carlberger
 * last change: 981030
 * comments: MIN_PREFIX_LENGTH determines how many letters must precede a suffix.
 *           used in tagger and freqstatmorf.cc.
 *           the longer prefix the faster tagger and smaller lexicon.
 *           an experiment 980917 gave the following results:
 *           MIN_PREFIX_LENGTH  #morfs  #correct tags
 *           0                  62995   13632
 *           1                  55933   13634
 *           2                  47128   13634
 *           3                  38393   13619
 *
 *           MIN_PREFIX_LENGTH=2 was chosen for obvious reasons
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

#ifndef _morf_hh
#define _morf_hh

const uint MIN_PREFIX_LENGTH = 2;
const int MAX_LAST_CHARS = 5;
const int MIN_LAST_CHARS = 1;


#endif
