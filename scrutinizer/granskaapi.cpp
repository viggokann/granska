#include "scrutinizer.h"
#include "prob.h"
bool xReadTaggedText = false;
static Scrutinizer scrutinizer;

void loadGranska() {	
  
  scrutinizer.Load(NULL, NULL);

#ifdef PROBCHECK 
  Prob::load(scrutinizer.Tags());
#endif

}

void granska(char* textFile) {
  int n;
  xPrintAllSentences = true;
  xPrintOneWordPerLine = false;
  xPrintSelectedTag = xPrintWordInfo = false;
  scrutinizer.ReadTextFromFile(textFile);
  scrutinizer.Scrutinize(&n);
  scrutinizer.PrintResult();
}
