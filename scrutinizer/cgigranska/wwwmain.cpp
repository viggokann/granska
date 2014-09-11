/* wwwmain.cpp
 * author: Johan Carlberger and Viggo Kann
 * last Johan change: 2000-03-24
 * last Viggo change: 2006-09-06
 * comments: main for www-scrutinizer
 */

#include <sys/types.h>
#include <sys/wait.h>
#include "wwwscrutinizer.h"
#include "scrutinizer.h"
#include "ruleset.h"
#include <unistd.h>

bool xReadTaggedText = false; // jonas, intended for use only for evaluation study 030120 - 030228

#define TEMPFILE "/tmp/wwwscrutinizer"
#define RULETEMPFILE "/tmp/wwwrulescrutinizer"

char tempfile[100];
char ruletempfile[100];

int wwwscrutinizer(char *text, char *filename, char *URLName, char *rulefile) {
  //  char *taggerlexdir = "/afs/nada.kth.se/misc/tcs/granska/lib/www/";
 // char *taggerlexdir = "/afs/nada.kth.se/misc/tcs/granska/lib/lexicons/suc/";
  char *taggerlexdir = getenv("TAGGER_LEXICON");
  //if (!rulefile || !*rulefile) rulefile = "/afs/nada.kth.se/misc/tcs/granska/lib/www/default-swedish-rules";
  if (!rulefile || !*rulefile) rulefile = getenv("SCRUTINIZER_RULE_FILE");
  else xOptimizeMatchings = false; // inserted 2006-09-03 by Viggo
  xPrintWordInfo = false;
  xPrintGramErrors = true;
  SetMessageStream(MSG_ERROR, &std::cout);
  SetMessageStream(MSG_WARNING, &std::cout);
  std::cout << "laddar lexikon... <BR>" << std::endl;
  Scrutinizer scrutinizer(OUTPUT_MODE_HTML);
  scrutinizer.Load(taggerlexdir, rulefile);
  if (!scrutinizer.IsLoaded()) {
    std::cout << "Granska kunde inte laddas <P>" << std::endl;
    return 1;
  }
  //  scrutinizer.GetRuleSet()->InActivate("analyze");
  std::cout << "granskar texten... <BR>" << std::endl;
#if 0
  //    std::cout << " i " << filename;
  if (URLName || !strcmp(filename, tempfile)) {
    std::ofstream out("/afs/nada.kth.se/home/theory/jfc/Public/skrutt-log/log99", ios::app);
    out << std::endl << std::endl << "LOGLOG" << std::endl << std::endl;
    if (URLName)
      out << URLName << std::endl;
    else {
      ;
      /*
      std::ifstream in(filename);
      char c;
      while((c = in.get()) != EOF)
	out.put(c);
      in.close();
      */
    }
    out.close();
  } else
#endif
  if (text)
    scrutinizer.ReadTextFromString(text);
  else
    scrutinizer.ReadTextFromFile(filename);
  int n;
  scrutinizer.Scrutinize(&n);
  std::cout << "<HR WIDTH=\"100%\">" << std::endl;
  scrutinizer.PrintResult();
  std::cout << "<HR WIDTH=\"100%\">" << std::endl;
  return 0;
}

int main(int argc, char **argv) {
  FILE *fp;
  int i, res;
  for (i = 0; i < 100; i++) {
    sprintf(tempfile, "%s%d.tmp", TEMPFILE, i);
    if (!(fp = fopen(tempfile, "r"))) break;
    fclose(fp);
  }
  if (!(fp = fopen(tempfile, "w"))) {
    fprintf(stderr, "Kan inte skriva på %s\n", tempfile);
    return 1;
  }
  fclose(fp);
  for (i = 0; i < 100; i++) {
    sprintf(ruletempfile, "%s%d.tmp", RULETEMPFILE, i);
    if (!(fp = fopen(ruletempfile, "r"))) break;
    fclose(fp);
  }
  if (!(fp = fopen(ruletempfile, "w"))) {
    fprintf(stderr, "Kan inte skriva på %s\n", ruletempfile);
    unlink(tempfile);
    return 1;
  }
  fclose(fp);
  if (argc == 2) {
    res = wwwscrutinizer(NULL, argv[1], NULL, NULL);
  } else
    res = wwwscrutinize(&wwwscrutinizer);
  unlink(tempfile);
  unlink(ruletempfile);
  return res;
}
