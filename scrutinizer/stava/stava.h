/* R�ttstavningsprogram. Version 2.60  2004-09-05
   Copyright (C) 1990-2004
   Joachim Hollman och Viggo Kann
   joachim@algoritmica.se viggo@nada.kth.se
*/

/******************************************************************************

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

******************************************************************************/

#define HUGEVAR
#ifdef __GNUC__
#define INLINE inline
#define VOLATILE volatile
#else
#define INLINE
#define VOLATILE
#endif

#define PROGRAMNAME    "stava"
#ifndef LIBPATH
/* LIBPATH ska inneh�lla bibliotekskatalogen med avslutande / 
   (eller avslutande \\ p� PC) */
#define LIBPATH "/misc/tcs/language/lib/"
#endif
#define ENVIRONMENTVARIABLE "STAVA"
#define LIBPATHVARIABLE     "STAVALIB" /* environment variable containing lib path */

/* Datafilsnamn om inget annat anges */
#define ELFILENAME        "ELlist"
#define KELFILENAME       "kELlist"
#define FLFILENAME        "FLlist"
#define KFLFILENAME       "kFLlist"
#define ILFILENAME        "ILlist"
#define KILFILENAME       "kILlist"
#define ULFILENAME        "ULlist"
#define KULFILENAME       "kULlist"
#define XLFILENAME	  "XLlist"
#define SLFILENAME        "SLlist"
#define XFORKORTNINGAR    "forkortningar"
#define XNAMN             "namn"
#define XDATATERMEREL     "datatermer.E"
#define XDATATERMERFL     "datatermer.F"
#define XDATATERMERIL     "datatermer.I"
#define XTEX              "tex"
#define XFYRGRAM          "fyrgraf"
#define DOKUMENTORDLISTEEFTERNAMN ".ord"
#define DOKUMENTELEFTERNAMN       ".ord.E"
#define DOKUMENTFLEFTERNAMN       ".ord.F"
#define DOKUMENTILEFTERNAMN       ".ord.I"
#define DOKUMENTULEFTERNAMN       ".ord.U"
#define ELBITS 22            /* Antal bitar hashfunktionen f�r EL ska skapa */
#define FLBITS 22            /* Antal bitar hashfunktionen f�r FL ska skapa */
#define ILBITS 19            /* Antal bitar hashfunktionen f�r IL ska skapa */
#define ULBITS 19            /* Antal bitar hashfunktionen f�r UL ska skapa */
#define XLBITS 22            /* Antal bitar hashfunktionen f�r XL ska skapa */
#define ELSIZE (1L<<(ELBITS-3))        /* Antal 8-bitselement i EL-bitvektorn */
#define FLSIZE (1L<<(FLBITS-3))        /* Antal 8-bitselement i FL-bitvektorn */
#define ILSIZE (1L<<(ILBITS-3))        /* Antal 8-bitselement i IL-bitvektorn */
#define ULSIZE (1L<<(ULBITS-3))        /* Antal 8-bitselement i UL-bitvektorn */
#define XLSIZE (1L<<(XLBITS-3))        /* Antal 8-bitselement i XL-bitvektorn */
#define HASHINITVAL 0x4711   /* Startv�rde i hashningen */
#define NOOFGRAMS 32         /* Antal bokst�ver i fyrgramsalfabetet */
#define DELIMGRAM 31         /* Nr som anv�nds f�r ordb�rjan/slut och skiljetecken i fyrgram */
#define DELIMP 1             /* Nr som anv�nds f�r ordb�rjan/slut och skiljetecken i p-tabeller */
#define FGRAMSIZE 131072L    /* Antal 8-bitselement i fyrgramsbitvektorn */
#define LANGD 4096           /* L�ngsta till�tna ord */
#define FILENAMELENGTH 1000  /* L�ngsta till�tna filnamn+1 */
#define DELORDMIN 3          /* Minsta till�ta ordl�ngd i sammans�ttningar */
#define STARTDELORDMIN 2     /* Minsta till�ta ordl�ngd p� f�rsta ordet i sammans�ttningar */
#define SLUTDELORDMIN 3      /* Minsta till�ta ordl�ngd p� sista ordet i sammans�ttningar */
#define DELORDMAX 22         /* L�ngsta till�tna delord */
#define MAXORDDELAR 4        /* h�gsta antal delar i ett sammansatt ord */
#define PREFIXMIN 0          /* Minsta till�ta ordl�ngd f�re suffix */
#define MAXSUFFIXMEMSIZE 12  /* Minnesstorlek f�r snabbminnet i suffix.c */
#define ORDMIN 2             /* Ord som �r kortare �n ORDMIN hoppas �ver */
#define VSHIFT 7             /* Hur mycket n�sta pow2 �r v�nstershiftad */
#define FLNOOFHASH 7         /* Antal hashningar i FL */
#define ELNOOFHASH 7         /* Antal hashningar i EL */
#define ILNOOFHASH 4         /* Antal hashningar i IL */
#define ULNOOFHASH 4         /* Antal hashningar i UL */
#define XLNOOFHASH 6         /* Antal hashningar i XL */

#define ISOCODE 1            /* v�rde p� variabeln x8bitar f�r ISO 8859-1 */
#define MACCODE 2            /* v�rde p� variabeln x8bitar f�r Mackodning */
#define DOSCODE 3            /* v�rde p� variabeln x8bitar f�r Doskodning */
#define UTF8CODE 4           /* v�rde p� variabeln x8bitar f�r UTF-8      */

extern char isLowerCase[256]; /* �r x en liten bokstav? */
extern char isUpperCase[256]; /* �r x en stor bokstav? */
extern char isVowel[256]; /* �r x en versal? */
extern char isDelim[256]; /* �r x en icke-bokstav? */
extern unsigned char toLowerCase[256]; /* omvandla stor till liten bokstav */
extern unsigned char toUpperCase[256]; /* omvandla stor till liten bokstav */
extern unsigned char *lowerCaseLetters; /* alla sm� bokst�ver */
extern unsigned char *upperCaseLetters; /* alla stora bokst�ver */
extern unsigned char *delimiters; /* alla icke-bokst�ver */

void VersalerGemena(register const unsigned char *ordin, 
				  register unsigned char *ord,
				  register unsigned char *Ord);
/* StavaSkrivOrd anropas vid r�ttstavningen f�r att skriva ut ett
   r�ttstavningsf�rslag. */
extern void StavaSkrivOrd(const unsigned char *s);

/* StavaSkrivSeparator anropas vid r�ttstavningen f�r att skriva ut 
   wordSeparator mellan tv� r�ttstavningsf�rslag. */
void StavaSkrivSeparator(void);

/* WriteISO skriver ut en ASCII-textstr�ng �versatt till ISO Latin-1 */
extern void WriteISO(const unsigned char *s);

/* sWriteISO skriver ut en ASCII-textstr�ng �versatt till ISO Latin-1 p� str�ng */
extern void sWriteISO(unsigned char *res, const unsigned char *s);

/* PrintErrorWithText prints an error containing a text argument. format is a 
  formating string containing the string %s */
void PrintErrorWithText(const char *format, const char *text);

int InEL(const unsigned char *ord, int len);
int InFL(const unsigned char *ord, int len);
int InIL(const unsigned char *ord, int len);
int InUL(const unsigned char *ord, int len);
int InXL(const unsigned char *ord, int len);
int InILorELbutnotUL(const unsigned char *ord, int len);

extern int x8bitar;
extern int xAndelser, xForkortningar, xNamn, xDatatermer;
extern int xTex;
extern int xSammansatta, xKort;
extern int xxDebug;
extern int xTillatSIFogar, xTillatSIAllaFogar;
extern int xGenerateCompounds, xIntePetig;
extern int xHtml, xEndastEtt, xRattstavningsforslag, xMaxOneError;
extern int xAcceptCapitalWords;
extern int xPrintError; /* Skriv ut felmeddelanden p� stderr */
extern char stavaerrorbuf[400];

/* Definieras i stavaconstants.h: */
extern unsigned char ISO_intern[256];
extern unsigned char MAC_intern[256];
extern unsigned char DOS_intern[256];
extern unsigned char intern_ISO[256];
extern unsigned char MAC_ISO[256];
extern unsigned char DOS_ISO[256];
extern unsigned char ISO_ISO[256];
extern unsigned char ASCII_intern[256];
extern unsigned char intern_gram[256];
extern unsigned char intern_p[256];
extern unsigned char dubbelBokstavsTabell[256];
extern unsigned char transform_diacritics[256];
extern unsigned char bindebokstav[];
extern unsigned char isConsonant[256];
