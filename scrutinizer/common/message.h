/* message.hh
 * author: Johan Carlberger
 * last change: 2000-03-28
 * comments:
 */

#ifndef _message_hh
#define _message_hh

#include <iosfwd>

enum MessageType {
  MSG_ERROR,
  MSG_WARNING,
  MSG_MINOR_WARNING,
  MSG_STATUS,
  MSG_VERBOSE,
  MSG_CONTINUE,
  MSG_COUNTS,
  MSG_N_TYPES
};

#ifdef MESSAGES

extern std::ostream *m_out[MSG_N_TYPES];
inline void SetMessageStream(MessageType t, std::ostream *out) { m_out[t] = out; }
void Message(MessageType t, const char *m1=0, const char *m2=0,
	     const char *m3=0, const char *m4=0);

#else

#define SetMessageStream(t, out)
void Message(MessageType t, const char *m1=0, const char *m2=0,
	     const char *m3=0, const char *m4=0);

#endif
#endif
