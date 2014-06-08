#include "scrutinizer.h"
#include "prob.h"
#include "granskaapi.h"
#include <iostream>
bool xReadTaggedText = false;
static Scrutinizer scrutinizer;

void loadGranska() {	
  
  scrutinizer.Load(NULL, NULL);

#ifdef PROBCHECK 
  Prob::load(scrutinizer.Tags());
#endif
}

const char* granska(char* text) {
  int n;
  xPrintAllSentences = true;
  xPrintOneWordPerLine = false;
  xPrintSelectedTag = xPrintWordInfo = false;
  scrutinizer.ReadTextFromString(text);
  scrutinizer.Scrutinize(&n);
  const char *cstr = scrutinizer.GetResult();
  return cstr;
}
