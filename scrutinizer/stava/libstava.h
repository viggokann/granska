/* Rättstavningsprogram. Version 2.63  2013-04-12
   Copyright (C) 1990-2013
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

#ifndef __STAVAAPI_H__
#define __STAVAAPI_H__

#include <stdio.h>

#if defined (__cplusplus)
extern "C" {
#endif

/* All methods except StavaReadLexicon and StavaLastErrorMessage are thread safe. */

/* StavaVersion returns a string that uniquely identifies the version. */
const char *StavaVersion(void);

/* StavaReadLexicon must be called before any other function in the API. */
/* Returns 1 if the initialization goes well and 0 otherwise. */
int StavaReadLexicon(const
                     char *libPath, /* path to library directory (ending with /) */
		     int compound, /* 1 to allow compound words */
		     int suffix,   /* 1 to apply suffix rules */
		     int abbrev,   /* 1 to add abbreviation word list */
		     int name,     /* 1 to add name list */
		     int comp,     /* 1 to add list of computer words */
		     int correct,  /* 1 to be able to correct words */
		     const unsigned char *separator); /* separator between corrections */

/* StavaAddWord adds a word to one of the word lists of Stava. This means
   that in the future the word will be accepted. There are four types of
   word lists:
   E  - (Ending) for words that may appear alone or as last part of compound
        Examples: medium, fotboll, blåare
   F  - (First) for words that may appear as first or middle part of compound
        Examples: medie, fotbolls, blå
   I  - (Individual) for words that may appear only as individual words
        Examples: hej, du
   U  - (exception) for words that should not be accepted
        Examples: parantes, mässigt
  Returns 1 if word could be stored and 0 otherwise. */
int StavaAddWord( const
		  unsigned char *word, /* the word to be entered */
                  char type);  /* word list type (E, F, I, or U) */

/* StavaWord checks if a word is correctly spelled.
   Returns 1 if the word is correctly spelled and 0 otherwise. */
int StavaWord(
	      const unsigned char *word);    /* word to be checked */

int SoundWord(
	      const unsigned char *word);    /* word to be checked for sound */

/* StavaCorrectWord checks if a word is correctly spelled and returns
   ordered proposals of replacements if not. The most likely word is
   presented first.
   Before StavaCorrectWord is called the first time StavaReadLexicon
   must have been called with the parameter correct=1.

   Returns NULL if the word is correctly spelled and a string of 
   proposed replacements otherwise. If no proposed replacement is
   found the empty string is returned. */
unsigned char *StavaCorrectWord(
	      const unsigned char *word);    /* word to be corrected */
	      

/* StavaCorrectCompound checks if a word is a correctly spelled compound
   and then returns ordered proposals of replacements. The most likely word is
   presented first.
   Before StavaCorrectCompound is called the first time StavaReadLexicon
   must have been called with the parameter correct=1.

   Returns NULL if the word is not a correctly spelled compound and a string 
   of proposed replacements otherwise. If no proposed replacement is
   found the empty string is returned. */
unsigned char *StavaCorrectCompound(
	      const unsigned char *word);     /* word to be corrected */

/* StavaGetWord reads the next word from infile and stores it in word.
   Word has to be allocated (of size at least 51) before calling.
   0 is returned if there is no more word (EOF), and 1 otherwise. */
int StavaGetWord(
		 FILE *infile,         /* file to be read */
		 unsigned char *word); /* where read word will be stored */

/* StavaAnalyzeCompound analyzes a compund. 
   Before StavaAnalyzeCompound is called the first time StavaReadLexicon
   must have been called.

   Returns 0 if the word is not a correctly spelled compound and 1 otherwise.
*/
extern int StavaAnalyzeCompound(
			 unsigned char *res, /* result will appear here */
			 const unsigned char *word); /* word to be analyzed */

extern int StavaGetAllCompounds(
			 unsigned char *res, /* result will appear here, 
						and it will be many '\0'-terminated strings, 
						ended with two consecutive '\0' */
			 const unsigned char *word); /* word to be analyzed */

/* StavaStringGetWord reads the next word from the string str and stores 
   it in word.
   Word has to be allocated (of size at least LANGD+1 (51)) before calling.
   NULL is returned if there is no more word, and a pointer to the next
   unused character in str otherwise. */
unsigned char *StavaStringGetWord(
		 unsigned char *str,   /* input string */
		 unsigned char *word); /* where read word will be stored */

  extern int utf8locale; /* decides whether PrintLocale should translate to utf-8 */

/* PrintLocale prints a string in the locale defined by utf8locale */
void PrintLocale(
                 FILE *f,        /* file to print on */
		 const char *s); /* string to print */

/* utf8string2iso converts an UTF-8 string to ISO-8859-1 */
int utf8string2iso(
		   char *dest, /* output string */
		   int destSize, /* max output size */
		   unsigned char *src); /* input string */

/* StavaLastErrorMessage returns a message describing the last error message
that occurred. Returns empty string if no error message occurred since last
call to StavaLastErrorMessage. */
const char *StavaLastErrorMessage(void);

/* Flags that may be set by application before spell checking: */
extern int xGenerateCompounds; /* 1 to enable and 0 to disable
				 compound generation when correcting words */
extern int xAcceptCapitalWords;
extern int xTillatSIFogar, xTillatSIAllaFogar;
extern int xIntePetig;
extern int xMaxOneError;
extern int xPrintError;

#if defined (__cplusplus)
}
#endif
#endif

