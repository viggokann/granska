#include "scrutinizer.h"
#include "prob.h"
#include "granskaapi.h"
#include <string.h>
#include <iostream>
#include <stdlib.h>

bool xReadTaggedText = false;
static Scrutinizer scrutinizer;

void loadGranska() {	
  
  scrutinizer.Load(NULL, NULL);

#ifdef PROBCHECK 
  Prob::load(scrutinizer.Tags());
#endif
}

char* granska(char* text) {
  int n;
  xPrintAllSentences = true;
  xPrintOneWordPerLine = false;
  xPrintSelectedTag = xPrintWordInfo = false;
  scrutinizer.ReadTextFromString(text);
  scrutinizer.Scrutinize(&n);
  char *cstr = scrutinizer.GetResult();
  return cstr;
}


