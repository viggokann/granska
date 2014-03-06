/* matchingset.hh
 * authors: Johan Carlberger
 * last change: 2000-03-24
 * comments: MatchingSet class
 */

#ifndef _matchingset_hh
#define _matchingset_hh

#include <iosfwd>

class Matching;
class ElementMatching;
class AbstractSentence;
class ChangeableTag;

class MatchingSet {
public:
  MatchingSet();
  ~MatchingSet();
  void Found(Matching*);
  Matching *SaveHelp(const Matching*);
  ChangeableTag *GetChangeableTag();
  void TerminateSearch(AbstractSentence*);
  int NFound() const { return nFound; }
  int NCheckModeFound() const { return nCheckModeFound; }
  void Clear();
  bool CheckMode() const { return checkMode; }
  void SetCheckMode(bool b) { checkMode = b; nCheckModeFound = 0; }
  void DeleteBuffers();
  const ElementMatching *GetHelpElementMatchings() const { return helpEmBuf; }
  int NHelpElementMatchings() const { return helpEmBufEnd; }
  const Matching *GetMatchings() const { return mBuf; }
  int NMatchings() const { return mBufEnd; }
private:
  Matching *Save(const Matching*, int);
  ElementMatching *CopyHelpElementMatchings(const ElementMatching*, const int);
  AbstractSentence *currSentence;
  bool checkMode;
  int nFound;
  int nCheckModeFound;
  ElementMatching *helpEmBuf;
  int helpEmBufEnd;
  Matching *mBuf;
  int mBufEnd;
  ChangeableTag *helpTagBuf;
  int helpTagBufEnd;
};

#endif

