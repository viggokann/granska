#include "scrutinizer.h"
#include "prob.h"
#include <iostream>
bool xReadTaggedText = false;
static Scrutinizer scrutinizer;

void loadGranska() {	
  
  scrutinizer.Load(NULL, NULL);

#ifdef PROBCHECK 
  Prob::load(scrutinizer.Tags());
#endif

}

const char* granska(const char* textFile) {
  int n;
  xPrintAllSentences = true;
  xPrintOneWordPerLine = false;
  xPrintSelectedTag = xPrintWordInfo = false;
  scrutinizer.ReadTextFromFile(textFile);
  scrutinizer.Scrutinize(&n);
  const char *cstr = scrutinizer.GetResult();
  return cstr;
}
