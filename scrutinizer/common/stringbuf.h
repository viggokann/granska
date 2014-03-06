/* stringbuf.hh
 * author: Johan Carlberger
 * last change: 2000-05-24
 * comments: StringBuf class
 */

#ifndef _stringbuf_hh
#define _stringbuf_hh

#include <string.h>
#include "basics.h"

class StringBuf {
 public:
    StringBuf(int s = 4096) : size(s), bufEnd(0), prev(NULL) {
        buf = new char[size];
        ensure(buf);
        NewObj(); ExtByt(size);
    }
    ~StringBuf() {
        if (prev) delete prev;
        delete [] buf; // jonas, delete -> delete []
        DelObj(); ExtByt(-size);
    }
    void Reset() {
        bufEnd = 0;
        if (prev) { delete prev; prev = NULL; }
    }
    char *NewString(const char *s) {
        char *s2 = NewString(strlen(s)+1);
        strcpy(s2, s); return s2;
    }
    char *NewString(int len) {
        if (len + bufEnd >= size) {
            //  std::cout << "resizing stringbuf" << std::endl;
            prev = new StringBuf(*this);
            ensure(prev);
            NewObj();
            if (len > size) size = len;
            buf = new char[size];
            ensure(buf);
            bufEnd = 0;
            ExtByt(size);
        }
        char *s = buf + bufEnd;
        bufEnd += len; return s;
    }
 private:
    int size;
    char *buf;
    int bufEnd;
    StringBuf *prev;
    DecObj();
};

#endif
