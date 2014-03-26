/* message.cc
 * author: Johan Carlberger
 * last change: 2000-04-25
 * comments:
 */

#ifdef linux
#include <cstdlib>
#endif
#include "message.h"

#ifdef MESSAGES

#include "settings.h"
#include "sentence.h"

std::ostream *m_out[MSG_N_TYPES] = {&std::cerr, &std::cerr, &std::cerr, &std::cerr, &std::cerr, &std::cerr, &std::cerr};

void Message(MessageType t, const char *m1, const char *m2,
	     const char *m3, const char *m4) {
  if (t == MSG_VERBOSE && !xVerbose)
    return;
  static MessageType prevT = MSG_STATUS;
  static int nWarnings = 0;
  static int nMinorWarnings = 0;
  static std::ostream *out = m_out[MSG_STATUS];
  if (t != MSG_CONTINUE)
    out = m_out[t];
  if (!out)
    return;
  if (t == MSG_CONTINUE && prevT == MSG_MINOR_WARNING && !xWarnAll)
    return;
  if (t == MSG_MINOR_WARNING) {
    nMinorWarnings++;
    prevT = t;
    if (!xWarnAll)
      return;
  }

  ; // (*out) << std::endl;

  prevT = t;
  switch(t) {
  case MSG_N_TYPES:
  case MSG_ERROR: (*out) << "ERROR:  "; break;
  case MSG_WARNING: nWarnings++;
  case MSG_MINOR_WARNING: (*out) << "WARNING: "; break;
  case MSG_VERBOSE: break;
  case MSG_STATUS: break;
  case MSG_CONTINUE: (*out) << "         "; break;
  case MSG_COUNTS:
    if (nWarnings || nMinorWarnings) {
      (*out) << nWarnings << " warning" << optS(nWarnings != 1) << ' ';
      if (nMinorWarnings)
	(*out) << "and " << nMinorWarnings << " minor warning"
	       << optS(nMinorWarnings != 1) << ' ';
      nWarnings = nMinorWarnings = 0;
    } else
      return;
  }
  if (m1)
    (*out) << m1;
  if (m2)
    (*out) << ' ' << m2;
  if (m3)
    (*out) << ' ' << m3;
  if (m4)
    (*out) << ' ' << m4;
  if (t == MSG_WARNING || t == MSG_ERROR) {
    if (xCurrRuleTerm) (*out) << " ruleterm: " << xCurrRuleTerm;
    if (xCurrSentence) (*out) << " sen: " << xCurrSentence;
  }
  if (t != MSG_STATUS || xVerbose)
    (*out) << xEndl;
  //if (t == MSG_STATUS)
  //  (*out) << '\r';
  if (t == MSG_STATUS)
    (*out) << '\n';
  if (t == MSG_ERROR) {
    Message(MSG_COUNTS);
    exit(1);
  }
}

#else

void Message(MessageType t, const char*, const char*, const char*, const char*) {}

#endif
