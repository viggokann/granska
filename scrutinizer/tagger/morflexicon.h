/* morflexicon.hh
 * author: Johan Carlberger
 * last change: 2000-05-08
 * comments: MorfLexicon class
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

#ifndef _morflexicon_hh
#define _morflexicon_hh

#include "word.h"
#include "hasharray.h"
#include "taglexicon.h"
#include "styleword.h"

class MorfLexicon : public HashArray<Word> {
public:
  MorfLexicon() : strings(NULL), CW(0) { NewObj(); }
  ~MorfLexicon();
  bool LoadFast(const char *dir, bool warn = true);
  void LoadSlow(const char *dir, TagLexicon& tgs);
  bool Save();
  int Cwt() const { return CWT; }
  int Cw() const { return CW; }
  bool IsLoaded() const { return CW > 0; }
  void PrintStatistics() const; // jonas
private:
  void CompressStrings();
  void LoadInfo();
  void AllocateMemory();
  void SetPointersFromIndices();
  const char *lexiconDir;
  WordTag *more;
  char *strings;
  int CL, CW, CWT, CMW;
  DecObj();
};

#endif
