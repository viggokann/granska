/* gramerror.hh
 * author: Johan Carlberger
 * last Viggo change:
 * last Johan change: 2000-04-27
 * comments: GramError class
 *           Only public methods should be used by user interface.
 *           Don't cast returned const objects into non-consts.
 *           Private methods are used by scrutinizer internally.
 */

#ifndef _gramerror_hh
#define _gramerror_hh

#include "matching.h"
#include "rulesettings.h"
#include "stringbuf.h"

class Scrutinizer;

static const int MAX_MARKED_SECTIONS = 5;

class GramError {
  friend class Scrutinizer;
  friend class MatchingSet;
public:
  const AbstractSentence *GetSentence() const { return sentence; }
  // returns the sentence in which the error was found

  int NMarkedSections() const { return nMarkedSections; }
  // returns number of separated marked word sequences

  int Start(int n) const { return start[n]; }
  // returns first word position in sentence of nth marked section

  int Stop(int n) const { return stop[n]; }
  // returns last word position in sentence of nth marked section

  const char *MarkedArea() const { return markedArea; }
  // returns a useful string of the entire marked area

  int NSuggestions() const { return nSuggestions; }
  // returns number of suggestions, may be 0

  const char *Suggestion(int n) const { return suggestion[n]; }
  // returns the nth suggestion

  const char *Info() const { return info; }
  // returns an info string about the error

  const char *LinkText() const { return ruleTerm->GetLinkText(); }
  // returns a text to describe an URL to more info

  const char *LinkURL() const { return ruleTerm->GetLinkURL(); }
  // returns an URL to more info

  const GramError *Next() const { return next; }
  // returns next gram-error of in same sentence

  void Print(std::ostream& = std::cout) const;
private:
  static Scrutinizer *scrutinizer;
  static MatchingSet *matchingSet;
  static StringBuf stringBuf;
  static void Reset() { stringBuf.Reset(); }
  GramError(Matching *m)
      : matching(m), allMarked(0),
        nMarkedSections(0), sentence(0),
	ruleTerm(0), markedArea(0), info(0),
        nSuggestions(0), next(0)
  {
      NewObj();
  }
  ~GramError() { DelObj(); }
  bool Evaluate();	    // returns false if false alarm
  void Report() const;	    // reports errors found to stats part
  bool IsError() const;	    // returns true if not @recog rule
  void Output() const;	    // output the object (most often XML)

  Matching *matching;
  bool allMarked;
  int nMarkedSections;
  int start[MAX_MARKED_SECTIONS];
  int stop[MAX_MARKED_SECTIONS];
  const AbstractSentence *sentence;
  const RuleTerm *ruleTerm;
  const char *markedArea;
  const char *info;
  int nSuggestions;
  const char *suggestion[MAX_SUGGESTIONS];
  char nErrors[MAX_SUGGESTIONS];
  GramError *next;
  DecObj();
};

inline std::ostream& operator<<(std::ostream& os, const GramError &s) {
  s.Print(os); return os;
}
inline std::ostream& operator<<(std::ostream& os, const GramError *s) {
  if (s) os << *s; else os << "(null GramError)"; return os;
}

#endif
