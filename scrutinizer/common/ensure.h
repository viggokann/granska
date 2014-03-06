/* ensure.hh
 * author: Johan Carlberger
 * last change: 961008
 * comments:
 */

#ifndef __ensure_hh
#define __ensure_hh

#if defined ENSURE
#include <assert.h>
#define ensure(x) assert(x)
#else
#define ensure(x)
#endif

#endif





