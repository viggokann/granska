/* Program som testar Stavas API. Det förväntar sej ett ord
   per rad och matar ut ord som anses vara felstavade.
   Rättstavningsförslag ges.

   Copyright (C) Viggo Kann 2002-03-22
   */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libstava.h"

int main(void)
{ char buf[100];
#if DEFAULTCODE==UTF8CODE
  utf8locale = 1;
#endif
  if (!StavaReadLexicon(LIBPATH,1,1,1,1,1,1,(const unsigned char *) ",")) {
    fprintf(stderr, "Kan inte initiera Stava\n");
    exit(1);
  }
  xGenerateCompounds = 1;
  while (gets(buf)) {
#if DEFAULTCODE==UTF8CODE
      char tmpbuf[1001];
      tmpbuf[1000] = '\0';
      utf8string2iso(tmpbuf, 1000, (unsigned char *) buf);
      strcpy(buf, tmpbuf);
#endif
    if (!StavaWord((unsigned char *) buf)) {
      unsigned char *corrections = StavaCorrectWord((unsigned char *) buf);
      if (corrections) {
	PrintLocale(stdout, buf);
	PrintLocale(stdout, ": ");
	PrintLocale(stdout, (char *) corrections);
	PrintLocale(stdout, "\n");
	free(corrections);
      }
    }
    else {
      char result[10000], *resp;
      StavaGetAllCompounds((unsigned char *) result, (unsigned char *) buf);
      PrintLocale(stdout, buf);
      PrintLocale(stdout, ":)");
      for (resp = result; (char)*resp; resp += strlen(resp) + 1) {
	PrintLocale(stdout, " ");
	PrintLocale(stdout, resp);
      }
      PrintLocale(stdout, "\n");
    }
  }
  return 0;
}
