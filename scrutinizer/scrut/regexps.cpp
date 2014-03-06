#define INIT         register unsigned char *sp = (unsigned char *) instring;
#define GETC()       (*sp++)
#define PEEKC()      (*sp)
#define UNGETC(c)    (--sp)
#define RETURN(ep)   return ep;
#define REGEXP_ERROR(c)     { fprintf(stderr, "Regexpkompileringsfel nr %d\n", c); return NULL; }

extern "C" {
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "regexp.h"
}

/* CompileRegexpHelp compiles valregexp */
char *CompileRegexpHelp(const char *valregexp) {
  char regexpbuf[1000], *valcompiled;
  if (!compile((char*)valregexp, regexpbuf, &regexpbuf[1000],'\0')) { // johan fixade bort varningen
    return NULL;
  }
  int len = strlen(regexpbuf);
  valcompiled = new char[len + 1];
  strcpy(valcompiled, regexpbuf);
  return valcompiled;
}

/* RegexpCheckHelp checks if regular expresesion in valcompiled matches s */
int RegexpCheckHelp(const char *s, const char *valcompiled)
{ 
  if (advance(s, valcompiled) && loc2 == s + strlen(s)) 
    return 1;
  return 0;
}
