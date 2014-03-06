/* morflexicon.hh
 * author: Johan Carlberger
 * last change: 2000-05-08
 * comments: MorfLexicon class
 */

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
