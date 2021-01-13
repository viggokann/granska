/* wwwscrutinizer.c
 * author: Viggo Kann, changed 1999-11-14
 * last Johan change: 2000-01-31
 * last Viggo change: 2006-09-06
 * comments: parses cgi input from web form
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>

#include "utf2latin1.h"

extern char tempfile[100], ruletempfile[100];

#define MAXURLLEN 400
#define TEMPFILENAME "/tmp/wwwskruttget"

typedef unsigned char bool;

static char *buf; /* Buffert f�r inmatningen, inleds med avskiljningsraden */
static int dividelen; /* Antal tecken i avskiljningsraden */
static char url[MAXURLLEN + 1];
static long maxlen = 1000000; /* Maxl�ngd p� infilen i demoversionen. */
static int (*wwwscrutinizer)(char *, char *, char *, char *);

extern bool xNewlineMeansNewSentence;
extern bool xPrintAllSentences, xPrintGramErrors, xPrintSelectedTag;
static bool xPrintCorrections = 1; /* Inte inf�rt i granskakoden �nnu /Viggo */
static bool xUseStandardRules;
static int noOfRuleLines = 0;

static char *stringdup(char *s) {
  char *t = malloc(strlen(s) + 1);
  if (t == NULL) exit(1);
  strcpy(t, s);
  return t;
}

static int FindTempFileName(char *filename)
{ FILE *fp;
  int i;
  for (i = 0; i < 100; i++) {
    sprintf(filename, "%s%d.tmp", TEMPFILENAME, i);
    if (!(fp = fopen(filename, "r"))) break;
    fclose(fp);
  }
  if (!(fp = fopen(filename, "w"))) {
    fprintf(stderr, "Kan inte skriva p� %s\n", filename);
    *filename = '\0';
    return 0;
  }
  fclose(fp);
  return 1;
}


static void TextScrutinize(char *t) {
  if(looksLikeUTF(t)) {
    utf2latin1(t);
  }

  (*wwwscrutinizer)(t, tempfile, NULL, noOfRuleLines > 0 ? ruletempfile : NULL);
  return;
  /*
  FILE *fp;
  fp = fopen(tempfile, "w");
  if (!fp) {
    printf("Kan inte �ppna %s\n", tempfile);
    return;
  }
  fprintf(fp, "%s", t);
  fclose(fp);
  (*wwwscrutinizer)(NULL, tempfile, NULL, noOfRuleLines > 0 ? ruletempfile : NULL);
  */
}

static void FileScrutinize(char *filename) {
    (*wwwscrutinizer)(NULL, filename, NULL, noOfRuleLines > 0 ? ruletempfile : NULL);
}

static void DocScrutinize(char *t) {
  FILE *fp;
  char command[1000];
  int c;

  fp = fopen("/tmp/wwwskrutt.tmp", "w");
  if (!fp) {
    printf("Kan inte �ppna %s\n", "/tmp/wwwskrutt.tmp");
    return;
  }
  fprintf(fp, "%s", t);
  fclose(fp);
//  system("module add hacks");
  sprintf(command, "/afs/.nada.kth.se/public/www/cgi-bin/viggo/granska/docfix </tmp/wwwskrutt.tmp > /tmp/tmp22");
  system(command);
  sprintf(command,
	  "/afs/.nada.kth.se/public/www/cgi-bin/viggo/granska/mswordview /tmp/tmp22 -w 1 -a -e -h -n -o /tmp/tmp33");
  c = system(command);
  if (c == -1) {
    printf("kan inte konvertera doc-fil\n");
    return;
  }
  sprintf(command,
	  "/afs/.nada.kth.se/public/www/cgi-bin/viggo/granska/html2txt </tmp/tmp33 > /tmp/tmp44");
  sprintf(command,
	  "echo 'hej JOhan och Viggo' > /tmp/tmp44");
  system(command);

  (*wwwscrutinizer)(NULL, "/tmp/tmp44", NULL, noOfRuleLines > 0 ? ruletempfile : NULL);
}

/* UrlScrutinize granskar en webbsida p� angiven URL */
static int UrlScrutinize(char *url, char *tempfilename) {
  const char *lynxHead = "lynx -dump -nolist '";
  const char *lynxTail = "' | tr '[:space:]' ' ' | tr -s ' ' | sed 's/\\[INLINE\\]//g'";

  char buf[maxlen];
  
  int i = 0;
  for(i = 0; i < MAXURLLEN && url[i] > 0; i++) {
    if(url[i] == 13) {
      url[i] = 0;
    }
    if(url[i] == '"' || url[i] == '\'') {
      url[i] = 0; // we do not want the external input to terminate our qoutation and add malicious code
    }
  }

  int needed = 0;
  
  needed += strlen(url);
  needed += strlen(lynxHead);
  needed += strlen(lynxTail);
  needed++;

  char *command_buf = malloc(needed);
  sprintf(command_buf, "%s%s%s", lynxHead, url, lynxTail);

  printf("l�ser %s...<BR>\n", url);

  FILE *lynx_pipe = popen(command_buf, "r");
  //long bytes = fread(buf, sizeof(buf), 1, lynx_pipe);
  fread(buf, sizeof(buf), 1, lynx_pipe);

  free(command_buf);

  TextScrutinize(buf);

  return 0;
}

static int IsName(char *s, char *t)
{ int len = strlen(t);
  return (strncmp(s, t, len) == 0 && s[len] == '"');
}

/* SkipToNextPart hoppar till n�sta del av inmatningen och returnerar 0
   ifall inmatningen �r slut */
static int SkipToNextPart(char **p)
{ char *bufp = *p;
  while (1) {
    if (bufp[0] == '-' && (bufp[-1] == '\n' || bufp[-1] == '\0') && 
	strncmp(bufp, buf, dividelen) == 0)
      break;
    bufp++;
  }
  bufp += dividelen;
  if (*bufp == '-') return 0;
  while (*bufp == '\r' || *bufp == '\n') bufp++;
  *p = bufp;
  return 1;
}

/* GetLine ger raden som kommer h�rn�st och uppdaterar bufp */
/* Om inmatningen �r tom returneras NULL */
static char *GetLine(char **p)
{ char *bufp, *start = *p;
  if (*start == '-' && strncmp(start, buf, dividelen) == 0) return NULL;
  for (bufp = start; *bufp != '\r' && *bufp != '\n'; bufp++);
  *bufp++ = '\0';
  while (*bufp == '\r' || *bufp == '\n') bufp++;
  *p = bufp;
  return start;
}

/* PrintHead skriver ut ett HTML-huvud med angiven titel */
static void PrintHead(char *title)
{
  printf("Content-type: text/html\n\n<HTML><HEAD>\n");
  printf("<TITLE>%s</TITLE>\n", title); 
  printf("</HEAD>\n<BODY>\n");
}

/* PrintFoot skriver ut slutet p� en HTML-fil med angiven datorpostadress */
static void PrintFoot(char *email)
{
  if (*email) printf("<P><ADDRESS>&lt;%s&gt;</ADDRESS>\n", email); 
  printf("</BODY>\n</HTML>\n");
}

/* InterpretString substitutes + with space and hexadecimal codes with the
   corresponding characters until next & or end of string and returns the next
   position in the input */
static char *InterpretString(char *origstring, char *result)
{ char *tmp, *res = result;
  for (tmp = origstring; *tmp != '\0' && *tmp != '&'; tmp++) {
    if (*tmp == '+') *res++ = ' '; else
      if (*tmp == '%') { /* hexadecimal number */
	unsigned int number;
	tmp++;
	sscanf(tmp, "%2x", &number);
	tmp++;
	*res++ = number;
      }
      else *res++ = *tmp;
  }
  *res = '\0';
  return tmp;
}

/* Parse inputs data from a HTML form. */
static int Parse(void) {
  long length, i;
  char *bufp, *tmp, *res, *value;
  /* char *finalline; */
  char *name, *filename;
  long filesize;
  /*  unsigned int number; */
  char *textp; /* pekare till text som ska granskas */
  
  tmp = getenv("CONTENT_LENGTH");
  if (tmp == NULL || *tmp == '\0') { 
    /* Uppslagsord kan finnas i QUERY_STRING */
    tmp = getenv("QUERY_STRING");
    
    if (tmp == NULL || *tmp == '\0') return 1; /* error */
    buf = malloc(strlen(tmp) + 1);
    InterpretString(tmp, buf);
    textp = buf;
    TextScrutinize(textp);
    return 0;
  }
  length = atol(tmp);
  buf = malloc(length + 200);
  /* L�s in meddelandet i buf: */
  for (i = 0; i < length; i++) buf[i] = getchar();
  buf[length] = '\0';
  res = buf+length;
  
  /* Avkoda hexadecimala koder i meddelandet:
  res = buf;
  for (tmp = buf; *tmp != '\0'; tmp++) {
    if (*tmp == '%' && sscanf(tmp + 1, "%2x", &number) == 1) {
      tmp++;
      tmp++;
      *res++ = number;
    } else *res++ = *tmp;
  }
  *res = '\0';
  */
  /* Kolla att meddelandet �r i flera delar �tskilda av streckrader: */
  for (bufp = buf; *bufp != '\r' && *bufp != '\n' && *bufp != '\0'; bufp++);
  if (*bufp == '\0' || *buf != '-') {
    printf("<H1>Tyv�rr kunde inte ditt svar l�sas</H1>\n");
    printf("Du m�ste anv�nda en nyare webbl�sare.\n");
    return 1;
  }
  dividelen = bufp - buf;
  *bufp++ = '\0';
  /* Se till att inmatningen avslutas med en avslutningsrad */
  if (dividelen > 195) {
    printf("Felaktig HTML-syntax. Avskiljningsraden f�r l�ng!");
    return 1;
  }
  sprintf(res, "\n%s--\r\n", buf);
  /* finalline = res + 1; */
  /* Kolla vad n�sta del har f�r namn, dvs s�k efter name="xxx": */
  while (*bufp) {
    name = strchr(bufp, '=');
    if (name == NULL) break;
    bufp = name + 2;
    if (name[1] != '"') {
      continue;
    }
    name = bufp;
    /* Skippa fram till informationens b�rjan: */
    for (; *bufp != '\n'; bufp++);
    for (; *bufp == '\r' || *bufp == '\n'; bufp++);
    if (IsName(name, "test")) {
      printf("<PRE>\n");
      for (; *bufp; bufp++)
	;
      putchar(*bufp);
      printf("</PRE>\n");
    } else
      if (IsName(name, "regler")) {
	FILE *fp = NULL;
	noOfRuleLines = 0;
	while ((value = GetLine(&bufp))) {
	  if (noOfRuleLines == 0) {
	    fp = fopen(ruletempfile, "w");
	    if (!fp) {
	      printf("Kan inte �ppna %s\n", ruletempfile);
	      break;
	    }
	  }
	  fprintf(fp, "%s\n", value);
	  noOfRuleLines++;
	}
	if (fp) fclose(fp);
	if (SkipToNextPart(&bufp) == 0) return 0;
    } else
      if (IsName(name, "ord")) {
	value = GetLine(&bufp);
	if (value && *value) {
	  int len;
	  textp = value;
	  len = strlen(value);
	  value[len + 1] = '\0'; /* l�gg till ett extra nulltecken sist */
	  TextScrutinize(textp);
	  return 0;
	}
	if (SkipToNextPart(&bufp) == 0) return 0;
      } else if (IsName(name, "medgranskasregler")) {
	value = GetLine(&bufp);
	if (value) {
	  xUseStandardRules = 1;
	  if (noOfRuleLines > 0) {
	    FILE *rulefp = fopen(DEFAULTRULEFILE, "r");
	    FILE *fp = fopen(ruletempfile, "a");
	    int ch;
	    if (!fp) {
	      printf("Kan inte �ppna %s\n", ruletempfile);
	      break;
	    }
	    if (!rulefp) {
	      printf("Kan inte �ppna %s\n", "regeldatabasen");
	      break;
	    }
	    while ((ch = fgetc(rulefp)) != EOF) fputc(ch, fp);
	    fclose(fp);
	    fclose(rulefp);
	  }
	} else xUseStandardRules = 0;
	if (SkipToNextPart(&bufp) == 0) return 0;
      } else if (IsName(name, "nyrad")) {
	value = GetLine(&bufp);
	if (value) xNewlineMeansNewSentence = 1;
	else xNewlineMeansNewSentence = 0;
	if (SkipToNextPart(&bufp) == 0) return 0;
      } else if (IsName(name, "allameningar")) {
	value = GetLine(&bufp);
	if (value) {
	  if (strcmp(value, "ja") == 0) xPrintAllSentences = 1;
	  else xPrintAllSentences = 0;
	}
	if (SkipToNextPart(&bufp) == 0) return 0;
      } else if (IsName(name, "visaf�rslag")) {
	value = GetLine(&bufp);
	if (value) xPrintCorrections = 1;
	else xPrintCorrections = 0;
	if (SkipToNextPart(&bufp) == 0) return 0;
      } else if (IsName(name, "visataggar")) {
	value = GetLine(&bufp);
	if (value) xPrintSelectedTag = 1;
	else xPrintSelectedTag = 0;
	if (SkipToNextPart(&bufp) == 0) return 0;
      } else if (IsName(name, "demotext")) {
	textp = bufp;
	FileScrutinize(EXAMPLESENTENCESFILE);
	// free(buf);
	return 0;
      } else 
	if (IsName(name, "url")) {
	  value = GetLine(&bufp);
	  if (value) {
	    while (isspace((int)*value))
	      value++;
	    length = strlen(value);
	    while(length >= 0 && isspace((int)value[length-1]))
	      value[--length] = '\0';
	    if (length >= MAXURLLEN) value[MAXURLLEN - 1] = '\0';
	    strcpy(url, value);
	    {
	        char tempfilename[100];
		if (!FindTempFileName(tempfilename)) return 0;
		return UrlScrutinize(url, tempfilename);
	    }
	  }
	  if (SkipToNextPart(&bufp) == 0) return 0;
	} else 
	  if (IsName(name, "fil")) {
	    textp = bufp;
	    filename = strchr(name + 4, '=');

	    if (filename == NULL) continue;
	    filename += 2;
	    
	    for (tmp = filename; *tmp && *tmp != '"'; tmp++);
	    *tmp = '\0';
	    if (*filename == '\0') continue;
	    
	    for (filesize = 0; filesize < maxlen; filesize++) {
	      if (bufp[0] == '-' && bufp[-1] == '\n' &&
		  strncmp(bufp, buf, dividelen) == 0)
		break;
	      bufp++;
	    }
	    if (filesize == maxlen) {
	      printf(
		     "<H2>Denna demonstrationsversion av Granska kan inte anv�ndas p� s� l�nga filer.</H2>");
	      return 1;
	    }
	    bufp[0] = bufp[1] = '\0';
	    bufp += dividelen;
	    if (filesize == 0) {
	      printf("<H1>Filen %s verkar inte finnas!</H1>\n", filename);
	      return 1;
	    }
	    //	    DocScrutinize(textp);
	    TextScrutinize(textp);
	    free(buf);
	    return 0;
	  } else {
	    printf("<BR>Ok�nt namn: ");
	    while (*name != '"') putchar(*name++);
	    value = GetLine(&bufp);
	    if (value) printf(".\nV�rde: '%s'<P>\n", value);
	    else printf(" utan v�rde.\n");
	    if (SkipToNextPart(&bufp) == 0) return 0;
	  }
  }
  printf("<H1>Ofullst�ndigt formul�r. Du m�ste ange n�got som ska granskas!</H1>\n");
  free(buf);
  return 1;
}

int wwwscrutinize(int (*f)(char*, char *, char *, char *)) {
  /* int res; */
  wwwscrutinizer = f;
  PrintHead("Resultat av granskning");
  /* res = Parse(); */
  Parse();
  PrintFoot("");
  return 0; /* res; */
}
