/* Rättstavningsprogram. Version 2.60  2004-09-05
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
/* LIBPATH ska innehålla bibliotekskatalogen med avslutande / 
   (eller avslutande \\ på PC) */
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
#define ELBITS 22            /* Antal bitar hashfunktionen för EL ska skapa */
#define FLBITS 22            /* Antal bitar hashfunktionen för FL ska skapa */
#define ILBITS 19            /* Antal bitar hashfunktionen för IL ska skapa */
#define ULBITS 19            /* Antal bitar hashfunktionen för UL ska skapa */
#define XLBITS 22            /* Antal bitar hashfunktionen för XL ska skapa */
#define ELSIZE (1L<<(ELBITS-3))        /* Antal 8-bitselement i EL-bitvektorn */
#define FLSIZE (1L<<(FLBITS-3))        /* Antal 8-bitselement i FL-bitvektorn */
#define ILSIZE (1L<<(ILBITS-3))        /* Antal 8-bitselement i IL-bitvektorn */
#define ULSIZE (1L<<(ULBITS-3))        /* Antal 8-bitselement i UL-bitvektorn */
#define XLSIZE (1L<<(XLBITS-3))        /* Antal 8-bitselement i XL-bitvektorn */
#define HASHINITVAL 0x4711   /* Startvärde i hashningen */
#define NOOFGRAMS 32         /* Antal bokstäver i fyrgramsalfabetet */
#define DELIMGRAM 31         /* Nr som används för ordbörjan/slut och skiljetecken i fyrgram */
#define DELIMP 1             /* Nr som används för ordbörjan/slut och skiljetecken i p-tabeller */
#define FGRAMSIZE 131072L    /* Antal 8-bitselement i fyrgramsbitvektorn */
#define LANGD 4096           /* Längsta tillåtna ord */
#define FILENAMELENGTH 1000  /* Längsta tillåtna filnamn+1 */
#define DELORDMIN 3          /* Minsta tillåta ordlängd i sammansättningar */
#define STARTDELORDMIN 2     /* Minsta tillåta ordlängd på första ordet i sammansättningar */
#define SLUTDELORDMIN 3      /* Minsta tillåta ordlängd på sista ordet i sammansättningar */
#define DELORDMAX 22         /* Längsta tillåtna delord */
#define MAXORDDELAR 4        /* högsta antal delar i ett sammansatt ord */
#define PREFIXMIN 0          /* Minsta tillåta ordlängd före suffix */
#define MAXSUFFIXMEMSIZE 12  /* Minnesstorlek för snabbminnet i suffix.c */
#define ORDMIN 2             /* Ord som är kortare än ORDMIN hoppas över */
#define VSHIFT 7             /* Hur mycket nästa pow2 är vänstershiftad */
#define FLNOOFHASH 7         /* Antal hashningar i FL */
#define ELNOOFHASH 7         /* Antal hashningar i EL */
#define ILNOOFHASH 4         /* Antal hashningar i IL */
#define ULNOOFHASH 4         /* Antal hashningar i UL */
#define XLNOOFHASH 6         /* Antal hashningar i XL */

#define ISOCODE 1            /* värde på variabeln x8bitar för ISO 8859-1 */
#define MACCODE 2            /* värde på variabeln x8bitar för Mackodning */
#define DOSCODE 3            /* värde på variabeln x8bitar för Doskodning */
#define UTF8CODE 4           /* värde på variabeln x8bitar för UTF-8      */

extern char isLowerCase[256]; /* är x en liten bokstav? */
extern char isUpperCase[256]; /* är x en stor bokstav? */
extern char isVowel[256]; /* är x en versal? */
extern char isDelim[256]; /* är x en icke-bokstav? */
extern unsigned char toLowerCase[256]; /* omvandla stor till liten bokstav */
extern unsigned char toUpperCase[256]; /* omvandla stor till liten bokstav */
extern unsigned char *lowerCaseLetters; /* alla små bokstäver */
extern unsigned char *upperCaseLetters; /* alla stora bokstäver */
extern unsigned char *delimiters; /* alla icke-bokstäver */

extern INLINE void VersalerGemena(register const unsigned char *ordin, 
				  register unsigned char *ord,
				  register unsigned char *Ord);
/* StavaSkrivOrd anropas vid rättstavningen för att skriva ut ett
   rättstavningsförslag. */
extern void StavaSkrivOrd(const unsigned char *s);

/* StavaSkrivSeparator anropas vid rättstavningen för att skriva ut 
   wordSeparator mellan två rättstavningsförslag. */
void StavaSkrivSeparator(void);

/* WriteISO skriver ut en ASCII-textsträng översatt till ISO Latin-1 */
extern void WriteISO(const unsigned char *s);

/* sWriteISO skriver ut en ASCII-textsträng översatt till ISO Latin-1 på sträng */
extern void sWriteISO(unsigned char *res, const unsigned char *s);

/* PrintErrorWithText prints an error containing a text argument. format is a 
  formating string containing the string %s */
void PrintErrorWithText(const char *format, const char *text);

extern INLINE int InEL(const unsigned char *ord, int len);
extern INLINE int InFL(const unsigned char *ord, int len);
extern INLINE int InIL(const unsigned char *ord, int len);
extern INLINE int InUL(const unsigned char *ord, int len);
extern INLINE int InXL(const unsigned char *ord, int len);
extern INLINE int InILorELbutnotUL(const unsigned char *ord, int len);

extern int x8bitar;
extern int xAndelser, xForkortningar, xNamn, xDatatermer;
extern int xTex;
extern int xSammansatta, xKort;
extern int xxDebug;
extern int xTillatSIFogar, xTillatSIAllaFogar;
extern int xGenerateCompounds, xIntePetig;
extern int xHtml, xEndastEtt, xRattstavningsforslag, xMaxOneError;
extern int xAcceptCapitalWords;
extern int xPrintError; /* Skriv ut felmeddelanden på stderr */
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
