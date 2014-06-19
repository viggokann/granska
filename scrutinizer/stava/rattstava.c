/* Rättstavningsprogram. Version 2.63  2013-04-12
   Copyright (C) 1990-2013
   Joachim Hollman och Viggo Kann
   joachim@algoritmica.se viggo@nada.kth.se

   Mikael Tillenius har utvecklat rangordningen 1995-1996, se
   exjobbsrapporten TRITA-NA-E9621
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

/* rattstava.c - modul för rättstavningsfunktioner */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SVENSKA
#include "stava.h"

#include "rattstava.h"
#include "suffix.h"

struct ljudpost {
  unsigned char *ljud;
  unsigned char **ljudklass;
  struct ljudpost *next;
};

static struct ljudpost *ljudpostbuf;
static int ljudpostn = 0;

static struct ljudpost *ljudposter[128][128];
#define LJUDPOST(a, b)  ljudposter[intern_p[(uchar)(a)]][intern_p[(uchar)(b)]]

/* Alla sätt att stava sje-ljudet på. Sist måste en tom sträng komma. */
static unsigned char *sjeljud[] = {
  (unsigned char *) "sj",
  (unsigned char *)  "skj",
  (unsigned char *)  "stj",
  (unsigned char *)  "sk",
  (unsigned char *)  "sch",
  (unsigned char *)  "sh",
  (unsigned char *)  "ssi",
  (unsigned char *)  "ssj",
  (unsigned char *)  "g",
  (unsigned char *)  "ge",
  (unsigned char *)  "ch",
  (unsigned char *)  "j",
  (unsigned char *)  ""};
/* Motsvarande för tje-ljudet: */
static unsigned char *tjeljud[] = {
  (unsigned char *) "tj",
  (unsigned char *)  "k",
  (unsigned char *)  ""};

typedef unsigned char uchar;

extern int xAndelser, xSammansatta, xContext, xMaxOneError;
extern int CheckWord(const unsigned char *word, int checkLevel);
extern int SimpleIsCompound(const unsigned char *word, int len);

#define CAPITALDIFF 32
#define MAXSUGGESTIONS  20 /* Max antal utskrivna rätteseförslag */
const static int truncSuggestionsOffset = 27;
 /* Hur stor kostnad utöver det bästa
               rättelseförslagets kostnad som accepteras i utskriften.
	       Värdet minskas med två för varje förslag som ges. */

/* standardvärden för kostnadsfunktionerna: */
#define DELPVAL 20 /* ta bort en bokstav */
#define SWAPVAL 20 /* kasta om två bokstäver */
#define REPPVAL 20 /* ersätt en bokstav */
#define INSPVAL 17  /* stoppa in en bokstav */
#define FIRSTP  10 /* ändra på första bokstaven i ett ord */
#define PARTCOST 25 /* kostnad per sammansättningsled */

/* kostnadsfunktioner: */
#define DELP(a, b)      delap[intern_p[(uchar)(a)]][intern_p[(uchar)(b)]] /* ta bort a före b */
#define INSP(a, b)      insap[intern_p[(uchar)(a)]][intern_p[(uchar)(b)]] /* sätt in a före b */
#define SWAP(a, b)      swapp[intern_p[(uchar)(a)]][intern_p[(uchar)(b)]] /* kasta om a och b */
#define REPP(a, b)      replp[intern_p[(uchar)(a)]][intern_p[(uchar)(b)]] /* ersätt a med b */

#define CHECK_EL    1
#define CHECK_FL    2
#define CHECK_IL    4

static signed char delap[128][128];
static signed char insap[128][128];
static signed char swapp[128][128];
static signed char replp[128][128];

static unsigned char fyrtabell[FGRAMSIZE];
static FILE *fyrf;

/* Data som används vid rättstavning */
struct corrData {
  unsigned char **fyrOrd;
  int fyrAntalOrd, fyrMaxAntalOrd;
  unsigned char **delOrd[2*MAXORDDELAR];
  int delAntalOrd[2*MAXORDDELAR], delMaxAntalOrd[2*MAXORDDELAR];
  int addedWord;
};


/* wordSeparator är den sträng som skrivs ut mellan två rättelseförslag. */
const unsigned char *wordSeparator = (unsigned char *) " ";


static INLINE void *xmalloc(size_t s)
{
  void  *p = malloc(s);
  if (!p) {
    fprintf(stderr, "Out of memory\n");
    exit(1);
  }
  return p;
}

static INLINE void *xrealloc(void *p, size_t s)
{
  p = realloc(p, s);
  if (!p) {
    fprintf(stderr, "Out of memory!\n");
    exit(1);
  }
  return p;
}

/* Calculate points for word frequencies */
static INLINE int WordFreq(uchar *word)
{
  int       len, i;
  uchar  buf[LANGD+1];
  static int p[] = { 0, 2, 4, 6, 8, 10, 12, 14, };

  strcpy((char *)buf, (char *)word);
  len = strlen((char *)buf);

  buf[len+1] = '\0';
  for (i=0; i<8; i++) {
    buf[len] = (uchar) ('A' + i);
    if (InXL(buf, len + 1)) return p[i];
  }
  return 25;
}

/* AddSound lägger till en ljudpost till ljudpostmatrisen */
static void AddSound(unsigned char *sound, unsigned char **class)
{ int len = strlen((char *)sound);
  struct ljudpost *p, *ptmp;
  unsigned char *s;
  p = &ljudpostbuf[ljudpostn++];   
  p->ljud = sound;
  p->ljudklass = class;
  if (len >= 2) {
    p->next = LJUDPOST(sound[0],sound[1]);
    LJUDPOST(sound[0],sound[1]) = p;
  } else {
    p->next = NULL;
    for (s = lowerCaseLetters; 1; s++) {
      if (!LJUDPOST(sound[0], *s)) LJUDPOST(sound[0], *s) = p;
      else {
	for (ptmp = LJUDPOST(sound[0], *s); 
	     ptmp->ljudklass != p->ljudklass && ptmp->next;
	     ptmp = ptmp->next);
	if (ptmp->ljudklass != p->ljudklass) ptmp->next = p;
      }
      if (!*s) break;
    }
  }
}

/* AddSoundClass lägger till en ljudklass till ljudpostmatrisen */
static void AddSoundClass(unsigned char **class)
{ unsigned char **p;
  for (p = class; **p; p++) AddSound(*p, class);
}

/* CountSoundClass talar om hur många ljud som finns i ljudklassen */
static int CountSoundClass(unsigned char **class)
{ unsigned char **p;
  int no = 0;
  for (p = class; **p; p++) no++;
  return no;
}

/* FyrKollaHela kollar om ett ords alla fyrgram är tillåtna */
int FyrKollaHela(const unsigned char *ord)
{ static unsigned char buf[LANGD+4];
  unsigned char *s;
  long l;
  int plats;
  sprintf((char *) buf, "-%s-", ord);
  for (s = buf; *s; s++) *s = intern_gram[*s];
  *s = 255;
  for (plats = 3; buf[plats] != 255; plats++) {
    l = (((long) buf[plats - 3] * NOOFGRAMS + buf[plats - 2]) * NOOFGRAMS
	 + buf[plats - 1]) * NOOFGRAMS + buf[plats];
    if (!(fyrtabell[l >> 3] & (1 << ((int) l & 7)))) return plats;
  }
  return 0;
}

/* LagraFyrgram ser till att ett ords alla fyrgram är tillåtna */
void LagraFyrgram(const unsigned char *ord)
{ static unsigned char buf[LANGD+4];
  unsigned char *s;
  long l;
  int plats;
  sprintf((char *) buf, "-%s-", ord);
  for (s = buf; *s; s++) *s = intern_gram[*s];
  *s = 255;
  for (plats = 3; buf[plats] != 255; plats++) {
    l = (((long) buf[plats - 3] * NOOFGRAMS + buf[plats - 2]) * NOOFGRAMS
	 + buf[plats - 1]) * NOOFGRAMS + buf[plats];
    fyrtabell[l >> 3] |= (1 << ((int) l & 7));
  }
}

/* FyrKoll kollar om ett ords fyrgram är tillåtna, plats anger index i
ord för en ändring. Om plats är negativt kollas hela ordet.
extra är antalet extra positioner som är ändrade */
static INLINE int FyrKoll(unsigned char *ord, int plats, int extra)
{ unsigned char buf[LANGD+4];
  unsigned char *s;
  long l;

  s = buf;
  if (plats < 3) *s++ = DELIMGRAM;
  else {
    ord += plats - 3;
    s += plats - 2;
  }
  for (; *ord; s++, ord++) *s = intern_gram[*ord];
  *s++ = DELIMGRAM;
  *s = 255;
  if (plats >= 2) {
    plats++;
    buf[plats + 4 + extra] = 255;
  } else {
    if (plats >= 0) buf[plats + 5 + extra] = 255;
    plats = 3;
  }
  while (buf[plats] != 255) {
    l = (((long) buf[plats - 3] * NOOFGRAMS + buf[plats - 2]) * NOOFGRAMS
	 + buf[plats - 1]) * NOOFGRAMS + buf[plats];
    if (!(fyrtabell[l >> 3] & (1 << ((int) l & 7)))) return 0;
    plats++;
  }
  return 1;
}

static void AddSuggestion(struct corrData *cd, uchar *ord, int point)
{ int i, r;
  int len = strlen((char *)ord);
  if (len <= 1) return; /* Strunta i enbokstavsord */
  if (InUL(ord, len)) return; /* Föreslå aldrig förbjudna ord */
  if (cd->fyrMaxAntalOrd == 0) {
    cd->fyrMaxAntalOrd = 20;
    cd->fyrOrd = xmalloc(sizeof(char *) * cd->fyrMaxAntalOrd);
  }
    
  for (i = 0; i < cd->fyrAntalOrd; i++) {
    if (!strcmp((char *) ord + 1, (char *) cd->fyrOrd[i] + 2)) {
      r = abs(*ord - *(cd->fyrOrd[i]+1));
      if (r == 0
	  || r == CAPITALDIFF
	  ) {
	if ((int) cd->fyrOrd[i][0] > point)
	  goto overwrite;
	else
	  return;
      }
    }
  }
  if (cd->fyrAntalOrd >= cd->fyrMaxAntalOrd) {
    cd->fyrMaxAntalOrd += 20;
    cd->fyrOrd = realloc(cd->fyrOrd, sizeof(char *) * cd->fyrMaxAntalOrd);
  }
  cd->fyrAntalOrd++;
  cd->fyrOrd[i] = xmalloc(len + 2);

overwrite:
  cd->addedWord = 1;
  cd->fyrOrd[i][0] = (point >= 256 ? 255 : (unsigned char) point);
  strcpy((char *) cd->fyrOrd[i]+1, (char *) ord);
}

static void Concat(struct corrData *cd, uchar *to, uchar *word, int point, int part, int lastpart)
{ int len, i;

  len = strlen((char *)to);
  if (part == lastpart) {
    if (len > 2 && *word == to[len-1] && to[len-2] == to[len-1]) word++;
    strcpy((char *)to + len, (char *)word);
    AddSuggestion(cd, to, point+WordFreq(to)+PARTCOST*lastpart);
  } else {
    for (i=0; i<cd->delAntalOrd[part]; i++) {
      if (len > 2 && cd->delOrd[part][i][1] == to[len-1] &&
	  to[len-2] == to[len-1]) 
	strcpy((char *) to + len, (char *) cd->delOrd[part][i]+2);
      else strcpy((char *) to + len, (char *) cd->delOrd[part][i]+1);
      Concat(cd, to, word, point+cd->delOrd[part][i][0], part+1, lastpart);
    }
    to[len] = 0;
  }
}

/* Sätt ihop de olika orddelarna i alla kombinationer och stoppa in i
   listan över ordförslag */
static void ConcatParts(struct corrData *cd, uchar *word, int point, int part)
{
  uchar buf[LANGD];

  buf[0] = 0;
  Concat(cd, buf, word, point, 0, part);
}

/* Lägg till en ny orddel */
static void AddPart(struct corrData *cd, uchar *ord, int point, int part)
{ int i, r;

  if (cd->delMaxAntalOrd[part] == 0) {
    cd->delMaxAntalOrd[part] = 20;
    cd->delOrd[part] = xmalloc(sizeof(char *) * cd->delMaxAntalOrd[part]);
  }

  for (i = 0; i < cd->delAntalOrd[part]; i++) {
    if (!strcmp((char *)ord + 1, (char *)cd->delOrd[part][i] + 2)) {
      r = abs(*ord - *(cd->delOrd[part][i]+1));
      if (r == 0
	  || r == CAPITALDIFF
	  ) {
        if ((int) cd->delOrd[part][i][0] > point)
          goto overwrite;
        else
          return;
      }
    }
  }
  if (cd->delAntalOrd[part] >= cd->delMaxAntalOrd[part]) {
    cd->delMaxAntalOrd[part] += 20;
    cd->delOrd[part] = xrealloc(cd->delOrd[part], sizeof(char *) * cd->delMaxAntalOrd[part]);
  }
  cd->delAntalOrd[part]++;
  cd->addedWord = 1;
  cd->delOrd[part][i] = xmalloc(strlen((char *)ord)+2);

  overwrite:
  cd->delOrd[part][i][0] = (point >= 256 ? 255 : (unsigned char) point);
  strcpy((char *)cd->delOrd[part][i]+1, (char *)ord);
}

/* rensa listan med orddelar */
static void PartClear(struct corrData *cd, int part)
{
  int i;
  for (i = 0; i < cd->delAntalOrd[part]; i++) free(cd->delOrd[part][i]);
  cd->delAntalOrd[part] = 0;
}

/* Kolla om word finns i ordlistan. Parametern check styr vilka av
   ordlistorna EL, IL och FL som ska användas */
INLINE void Check(struct corrData *cd, uchar *word, int point, int part, int len, int check)
{
  if (check & CHECK_IL) {
    if (InUL(word, len)) return;
    if (InIL(word, len)) {
      AddSuggestion(cd, word, point+WordFreq(word));
      return;
    }
  }
  if (check & CHECK_EL) {
    if (InEL(word, len) || (xAndelser && CheckSuffix(word, 0, 0))) {
      ConcatParts(cd, word, point, part);
      return;
    }
  }
  if (check & CHECK_FL) {
    if (InFL(word, len)) {
      AddPart(cd, word, point+WordFreq(word), part);
      return;
    }
  }
  return;
}


static void GenereraLjudbyten(struct corrData *cd, unsigned char *ord)
{
  int i, soundlen, noofparts;
  unsigned char *s, *sound, **newsound, word[LANGD + 4];
  struct ljudpost *p;
  for (s = ord; *s; s++) {
    for (p = LJUDPOST(toLowerCase[*s], s[1]); p; p = p->next) {
      for (i = 1; p->ljud[i]; i++) 
	if (p->ljud[i] != s[i]) goto notsame;
      /* ljud hittat, byt det mot varje annat ljud i klassen */
      sound = p->ljud;
      soundlen = strlen((char *)sound);
      for (newsound = p->ljudklass; **newsound; newsound++) 
	if (*newsound != sound) {
	  strcpy((char *)word, (char *)ord);
	  strcpy((char *)word + (s-ord), (char *)*newsound);
	  if (isUpperCase[*s]) word[s-ord] = toUpperCase[**newsound];
	  strcat((char *)word, (char *)s + soundlen);
	  if (FyrKoll(word, s-ord, strlen((char *)*newsound)) > 0) {
	    if (CheckWord(word, 2))
	      AddSuggestion(cd, word, REPPVAL-1 + WordFreq(word));
	    else if ((noofparts = SimpleIsCompound(word, 
						   strlen((char *)word))))
	      AddSuggestion(cd, word, 
			    REPPVAL-1 + WordFreq(word) + PARTCOST*(noofparts-1));
	  }
	}
    notsame: continue;
    }
  }
}

static void Generera1(struct corrData *cd, unsigned char *word, int from, int errors, int point, int part, 
	       int len, int check)
{
  int i, left, right, first, last, onlyswap;
  unsigned char *lett;
  long l;
  unsigned char fyrbuf[LANGD+2], word2[LANGD];
  unsigned char *s;

  if (len < 2) return;

  sprintf((char *)fyrbuf, "-%s-", word);
  for (s = fyrbuf; *s; s++) *s = intern_gram[*s];

  last = -1;
  first = -1;
  for (i = 0; i < len - 1; i++) {
    l = (((long) fyrbuf[i] * NOOFGRAMS + fyrbuf[i+1]) * NOOFGRAMS
	 + fyrbuf[i+2]) * NOOFGRAMS + fyrbuf[i+3];
    if (!(fyrtabell[l>>3] & (1 << (l&7)))) {
      last = i;
      if (first == -1)
        first = i;
    }
  }
  onlyswap = 0;
  if (errors == 1) {
    if (last == -1) {   /* inget felaktigt fyrgram */
      left = from;
      right = len;
    } else if (last-first > 4) { /* minst 2 fel */
      return;
    } else if (last-first == 4) {
      onlyswap = 1;
      left = last;
      right = last-1;
    } else {    /* möjliga positioner för felet */
      left = last-1;
      right = first+3;
      if (right > len) right = len;
    }
  } else { /* se till att alltid fixa första felaktiga fyrgrammet */
    left = from;
    if (first == -1 || first+3 > len)
      right = len;
    else
      right = first+3;
  }
  if (left < from)
    left = from;

  /*fprintf(stderr, "[%s %d %d %d %d]", word, first, last, left, right);*/

  if (last == -1)
    Check(cd, word, point, part, len, check);

  if (errors == 0) {
    PrintErrorWithText("0 fel - borde inte inträffa. Ord: %s.\n", (const char *)word);
    return;
  }
  /* swap two chars */
  for (i=left-1; i<right; i++) {
    unsigned char tmp1;
    if (i<0 || word[i+1] == '\0')
      continue;
    tmp1 = word[i];
    word[i] = word[i+1];
    word[i+1] = tmp1;
    if (errors == 1) {
      if (FyrKoll(word, i, 1))
        Check(cd, word, point+SWAP(word[i+1], word[i]), part, len, check);
    } else {
      Generera1(cd, word, i+1, errors-1,
                point+SWAP(word[i+1], word[i]), part, len, check);
    }
    tmp1 = word[i];
    word[i] = word[i+1];
    word[i+1] = tmp1;
  }

  if (onlyswap) return;
    
  if (len > DELORDMIN || (check & CHECK_IL)) {
    /* remove char */
    strncpy((char *)word2, (char *)word, left);
    for (i=left; i<right; i++) {
      strcpy((char *)word2+i, (char *)word+i+1);
      if (errors == 1) {
	if (FyrKoll(word2, i, 0))
	  Check(cd, word2, point+DELP(word[i], word2[i])+((i==0)?FIRSTP:0),
		part, len-1, check);
      } else {
	Generera1(cd, word2, i, errors-1,
		  point+DELP(word[i], word2[i])+((i==0)?FIRSTP:0), part,
		  len-1, check);
      }
      word2[i] = word[i];
    }
  }

  /* replace char */
  for (i = left; i < right; i++) {
    int firstpoint = (i == 0) ? FIRSTP : 0;
    unsigned char tmp1 = word[i];
    unsigned char low = toLowerCase[tmp1];
    if ((i == 0 && part == 0) || 
	(i > 0 && (isUpperCase[word[i-1]] || isDelim[word[i-1]]))
	|| isUpperCase[word[i]]) {
      unsigned char UP = toUpperCase[tmp1];
      for (lett = upperCaseLetters; *lett; lett++) {
	if (*lett != tmp1) {
	  word[i] = *lett;
	  if (errors == 1) {
	    if (FyrKoll(word, i, 0))
	      Check(cd, word, 
		    point+REPP(tmp1,*lett)+((*lett==UP)?0:firstpoint),
		    part, len, check);
	  } else {
	    Generera1(cd, word, i+1, errors-1,
		      point+REPP(tmp1,*lett)+((*lett==UP)?0:firstpoint),
		      part, len, check);
	  }
	}
      }
    }
    for (lett = lowerCaseLetters; *lett; lett++) {
      if (*lett != tmp1) {
        word[i] = *lett;
        if (errors == 1) {
          if (FyrKoll(word, i, 0))
            Check(cd, word, point+REPP(tmp1,*lett)+((*lett==low)?0:firstpoint),
                  part, len, check);
        } else {
          Generera1(cd, word, i+1, errors-1,
                    point+REPP(tmp1,*lett)+((*lett==low)?0:firstpoint),
		    part, len, check);
        }
      }
    }
    word[i] = tmp1;
  }

  /* insert char */
  strncpy((char *)word2, (char *)word, left);
  strcpy((char *)word2+left+1, (char *)word+left);
  for (i = left; i < right+1; i++) {
    if ((i == 0 && part == 0) || 
	(i > 0 && (isUpperCase[word[i-1]] || isDelim[word[i-1]]))) {
      for (lett = upperCaseLetters; *lett; lett++) {
	word2[i] = *lett;
	if (errors == 1) {
	  if (FyrKoll(word2, i, 0))
	    Check(cd, word2, INSP(word2[i], word2[i+1])+((i==0)?FIRSTP:0),
		  part, len+1, check);
	} else {
	  Generera1(cd, word2, i+1, errors-1,
		    INSP(word2[i], word2[i+1])+((i==0)?FIRSTP:0), part,
		    len+1, check);
	}
      }
    }
    for (lett = lowerCaseLetters; *lett; lett++) {
      word2[i] = *lett;
      if (errors == 1) {
        if (FyrKoll(word2, i, 0))
          Check(cd, word2, INSP(word2[i], word2[i+1])+((i==0)?FIRSTP:0),
                part, len+1, check);
      } else {
        Generera1(cd, word2, i+1, errors-1,
                  INSP(word2[i], word2[i+1])+((i==0)?FIRSTP:0), part,
                  len+1, check);
      }
    }
    word2[i] = word2[i+1];
  }
  word2[len] = '\0';
}

/* Insert1 prövar att stoppa in exakt en bokstav i ordet */
static int Insert1(struct corrData *cd, unsigned char *word, int point, int part, int len, int check)
{ int i;
  unsigned char *lett;
  unsigned char word2[LANGD];
  cd->addedWord = 0;
  strcpy((char *)word2+1, (char *)word);
  for (i = 0; i < len+1; i++) {
    if ((i == 0 && part == 0) || 
	(i > 0 && (isUpperCase[word[i-1]] || isDelim[word[i-1]]))) {
      for (lett = upperCaseLetters; *lett; lett++) {
	word2[i] = *lett;
	if (FyrKoll(word2, i, 0))
	  Check(cd, word2, INSP(word2[i], word2[i+1])+((i==0)?FIRSTP:0),
		part, len+1, check);
      }
    }
    for (lett = lowerCaseLetters; *lett; lett++) {
      word2[i] = *lett;
      if (FyrKoll(word2, i, 0))
	Check(cd, word2, INSP(word2[i], word2[i+1])+((i==0)?FIRSTP:0),
	      part, len+1, check);
    }
    word2[i] = word2[i+1];
  }
  /* word2[len] = '\0'; */
  return (cd->addedWord ? 1 : -1);
}


/* InsertSpace tries to insert a space inside the word */
void InsertSpace(struct corrData *cd, unsigned char *wordin, int len)
{
  unsigned char word[LANGD], buf[LANGD];
  int i, point;
  strcpy((char *)word, (char *)wordin);
  for (i = len - 2; i >= 2; i--) {
    if (InILorELbutnotUL(word + i, len - i)) {
      word[i] = '\0';
      if (InILorELbutnotUL(word, i)) {
	point = WordFreq(word);
	sprintf((char *)buf, "%s %s", word, wordin + i);
	AddSuggestion(cd, buf, INSPVAL + 5 + PARTCOST*2 + point + WordFreq(wordin + i));
      }
      word[i] = wordin[i];
      if (i > 2) {
	word[i-1] = '\0';
	if (InILorELbutnotUL(word, i-1)) {
	  point = WordFreq(word);
	  sprintf((char *)buf, "%s %s", word, wordin + i);
	  AddSuggestion(cd, buf, DELPVAL + 5 + PARTCOST*2 + point + WordFreq(wordin + i));
	}
	word[i-1] = wordin[i-1];
      }
    }
  }
}

/* Generera rättstavningsförslag för word. Högst errors fel får förekomma */
static int Generera(struct corrData *cd, unsigned char *word, int errors, int point, int part,
		    int len, int check)
{ int i;

  if (len > DELORDMAX) return -1;

  if (errors > 1) {
    if (len <= 6 || xMaxOneError || part || point) errors = 1;
    else if (errors > 2) errors = 2;
  }
  cd->addedWord = 0;
  Check(cd, word, 0, part, len, check);
  if (cd->addedWord && check == CHECK_FL) return 0; /* Viggo 1999-11-25, återinfört 2002-02-09 */
  for (i=1; i<=errors; i++) {
    /*fprintf(stderr, "{Gen %s %d %d}", word, i, errors);*/
    Generera1(cd, word, 0, i, point, part, len, check);
    if (cd->addedWord) return i;
  }
  return -1;
}

/* Kolla om word är sammansatt som FL* EL. Om xTillatSIFogar
   så tillåt 's' i all fogar utom mellan 1:a och 2:a delen.
   Om xTillatSIAllaFogar så tillåt 's' i alla fogar. */
/* offset är platsen i ordet där word börjar */
static INLINE void CompoundEdit(struct corrData *cd, uchar *word, int offset, int part, int errors,
				int len)
{ int end, tmp, res, startpos, finalpos;

  /* printf("CompoundEdit(%s,offset=%d,part=%d,errors=%d,len=%d\n", word, offset, part, errors, len); */
  if (errors == 0) {
    cd->addedWord = 0;
    Check(cd, word, 0, part, len, CHECK_EL);
    if (cd->addedWord) return; /* negation borttagen 2003-04-17 */
    if (!xGenerateCompounds || part+1 >= MAXORDDELAR) return;
    startpos = offset ? DELORDMIN : STARTDELORDMIN;
    finalpos = len - DELORDMIN;
    for (end=startpos; end<=finalpos; end++) {
      tmp = word[end];
      word[end] = 0;
      PartClear(cd, part);
      cd->addedWord = 0;
      Check(cd, word, 0, part, end, CHECK_FL);
      if (cd->addedWord) {
	word[end] = tmp;
	/* Hantera t ex toppolitiker som topp|politiker */
	if (word[end-1] == word[end-2]) {
	  if (word[end-1] == tmp) {
	    continue;
	  }
	  CompoundEdit(cd, word+end-1, offset+end-1, part+1, 0, len-end+1);
	}
	CompoundEdit(cd, word+end, offset+end, part+1, 0, len-end);
	if (((xTillatSIFogar && offset) || xTillatSIAllaFogar) &&
	    word[end] == 's' &&
	    bindebokstav[(unsigned char)word[end-1]] == 's') {
	  PartClear(cd, part+1);
	  AddPart(cd, (uchar *)"s", 0, part+1);
	  CompoundEdit(cd, word+end+1, offset+end+1, part+2, 0, len-end-1);
	}
      }
      else word[end] = tmp;
    }
    return;
  } 
  if (offset == 0) {
    res = Generera(cd, word, errors, 0, part, len, CHECK_EL | CHECK_IL);
    if (res == 0 || res == 1) return;
  } else {
    if (Generera(cd, word, errors, 0, part, len, CHECK_EL) != -1) return;
  }

  if (!xGenerateCompounds || part+1 >= MAXORDDELAR) return;

  startpos = offset ? DELORDMIN : STARTDELORDMIN;
  finalpos = len - DELORDMIN;
  for (end=startpos; end<=finalpos; end++) {
    tmp = word[end];
    word[end] = 0;
    PartClear(cd, part);
    
    if (end == startpos) res = Insert1(cd, word, 0, part, end, CHECK_FL);
    else res = Generera(cd, word, errors, 0, part, end, CHECK_FL);

    if (res != -1) {
      errors -= res;
      word[end] = tmp;
      /* Hantera t ex toppolitiker som topp|politiker */
      if (word[end-1] == word[end-2]) {
	if (word[end-1] == tmp) {
	  errors += res;
	  continue;
	}
        CompoundEdit(cd, word+end-1, offset+end-1, part+1, errors, len-end+1);
      }
      CompoundEdit(cd, word+end, offset+end, part+1, errors, len-end);

      if (((xTillatSIFogar && offset) || xTillatSIAllaFogar) &&
          word[end] == 's' &&
          bindebokstav[(unsigned char)word[end-1]] == 's') {
        PartClear(cd, part+1);
        AddPart(cd, (uchar *)"s", 0, part+1);
        CompoundEdit(cd, word+end+1, offset+end+1, part+2, errors, len-end-1);
      }
      errors += res;
    }
    else word[end] = tmp;
  }
}

static void GenereraAlternativaOrd(struct corrData *cd, uchar *wordin)
{ int len = strlen((char *)wordin);
  if (len < 2)
    return;
  if (len < 7 || xMaxOneError)
    CompoundEdit(cd, wordin, 0, 0, 1, len);
  else
    CompoundEdit(cd, wordin, 0, 0, 2, len);
  if (len >= 4) InsertSpace(cd, wordin, len);
}

/* InitRattstava öppnar fyrgramsfilen och initierar hjälpstrukturer.
   separator är den sträng som skrivs ut mellan två rättelseförslag. */
int InitRattstava(const char *fyrgramfilename, const unsigned char *separator)
{ int i, j;
  unsigned char *lett, *lett2;
  char slask;
  uchar dub[] = "bdfgjlmnprstv"; /* dubbeltecknande konsonanter */
  uchar vow[] = "aeiouy{|}~AEIOUY[\\]^"; /* vokaler i p-tabeller */

  wordSeparator = separator;
  if (!(fyrf = fopen(fyrgramfilename, "rb"))) {
    PrintErrorWithText("Kan inte öppna filen %s\n", (const char *) fyrgramfilename);
    return 0;
  }
  if (fread(fyrtabell, sizeof(unsigned char), FGRAMSIZE, fyrf) != 
      FGRAMSIZE || fread(&slask, sizeof(char), 1, fyrf) == 1) {
    PrintErrorWithText("%s har fel format för att vara en fyrgramsfil\n",
            (const char *) fyrgramfilename);
    fclose(fyrf);
    return 0;
  }
  fclose(fyrf);

  for (i = 0; i < 128; i++) {
    for (j = 0; j < 128; j++) {
      delap[i][j] = DELPVAL;
      insap[i][j] = INSPVAL;
      swapp[i][j] = SWAPVAL;
      replp[i][j] = REPPVAL;
    }
    replp[i][i] = REPPVAL - 9;
  }

  for (lett = dub; *lett; lett++) {
    insap[*lett][*lett] -= 4;
    insap[toUpperCase[*lett]][toUpperCase[*lett]] -= 4;
    delap[*lett][*lett] -= 4;
    delap[toUpperCase[*lett]][toUpperCase[*lett]] -= 4;
  }
  insap[(int)'c'][(int)'k']--;
  insap[(int)'C'][(int)'K']--;

  for (lett = vow; *lett; lett++)
    for (lett2 = vow; *lett2; lett2++)
      if (lett != lett2) {
	replp[*lett][*lett2]--;
      }

  replp[(int)'e'][126 /* é */] = REPPVAL-9;
  replp[126 /* é */][(int)'e'] = REPPVAL-9;
  replp[(int)'e'][(int)'a'] = REPPVAL-4;
  replp[(int)'a'][(int)'e'] = REPPVAL-4;
  replp[(int)'e'][(int)'i'] = REPPVAL-4;
  replp[(int)'i'][(int)'e'] = REPPVAL-4;
  replp[(int)'n'][(int)'m'] = REPPVAL-4;
  replp[(int)'m'][(int)'n'] = REPPVAL-4;
  replp[(int)'{'][(int)'e'] = REPPVAL-4;
  replp[(int)'e'][(int)'{'] = REPPVAL-4;
  replp[(int)'}'][(int)'o'] = REPPVAL-4;
  replp[(int)'o'][(int)'}'] = REPPVAL-4;
  replp[(int)'s'][(int)'c'] = REPPVAL-4;
  replp[(int)'g'][(int)'j'] = REPPVAL-4;
  replp[(int)'j'][(int)'g'] = REPPVAL-4;
  replp[(int)'v'][(int)'w'] = REPPVAL-2;
  replp[(int)'w'][(int)'v'] = REPPVAL-2;
  replp[(int)'c'][(int)'k'] = REPPVAL-2;
  replp[(int)'s'][(int)'z'] = REPPVAL-4;
  replp[(int)'z'][(int)'s'] = REPPVAL-4;

  replp[(int)'E'][94 /* É */] = REPPVAL-9;
  replp[94 /* É */][(int)'E'] = REPPVAL-9;
  replp[(int)'E'][(int)'A'] = REPPVAL-4;
  replp[(int)'A'][(int)'E'] = REPPVAL-4;
  replp[(int)'E'][(int)'I'] = REPPVAL-4;
  replp[(int)'I'][(int)'E'] = REPPVAL-4;
  replp[(int)'N'][(int)'M'] = REPPVAL-4;
  replp[(int)'M'][(int)'N'] = REPPVAL-4;
  replp[(int)'['][(int)'E'] = REPPVAL-4;
  replp[(int)'E'][(int)'['] = REPPVAL-4;
  replp[(int)']'][(int)'O'] = REPPVAL-4;
  replp[(int)'O'][(int)']'] = REPPVAL-4;
  replp[(int)'S'][(int)'C'] = REPPVAL-4;
  replp[(int)'G'][(int)'J'] = REPPVAL-4;
  replp[(int)'J'][(int)'G'] = REPPVAL-4;
  replp[(int)'V'][(int)'W'] = REPPVAL-2;
  replp[(int)'W'][(int)'V'] = REPPVAL-2;
  replp[(int)'C'][(int)'K'] = REPPVAL-2;
  replp[(int)'S'][(int)'Z'] = REPPVAL-4;
  replp[(int)'Z'][(int)'S'] = REPPVAL-4;

  for (lett = lowerCaseLetters; *lett; lett++) {
    REPP(*lett, toUpperCase[*lett]) = 1; /* -FIRSTP+2 */
    REPP(toUpperCase[*lett], *lett) = 1; /* REPPVAL-6 */
  }

  i = CountSoundClass(sjeljud);
  i += CountSoundClass(tjeljud);
  ljudpostbuf = malloc(sizeof(*ljudpostbuf) * i);
  AddSoundClass(sjeljud);
  AddSoundClass(tjeljud);
  return 1;
}


            
/* SorteraGenereradeOrd sorterar dom genererade orden efter trolighetsvärdet och
lägger dom i cset. Eventuella icke-presenterade ord frigörs. */
static int SorteraGenereradeOrd(struct correctionSet *cset, struct corrData *cd, int Capitalized)
{  int i, j, truncSuggestions;
  unsigned char *tmp;

  cset->corrections = NULL;
  cset->noOfCorrections = 0;

  if (cd->fyrAntalOrd > 0) {
    /* Sortera fyrOrd med insättningssortering */
    for (i = 1; i < cd->fyrAntalOrd; i++) {
      tmp = cd->fyrOrd[i];
      for (j = i; j > 0 && cd->fyrOrd[j-1][0] > tmp[0]; j--)
	cd->fyrOrd[j] = cd->fyrOrd[j-1];
      cd->fyrOrd[j] = tmp;
    }

    truncSuggestions = cd->fyrOrd[0][0] + truncSuggestionsOffset;

    for (i = 0; i < cd->fyrAntalOrd && i < MAXSUGGESTIONS && 
	   cd->fyrOrd[i][0] <= truncSuggestions - 2 * i; i++) {
      if (Capitalized && isLowerCase[cd->fyrOrd[i][1]]) 
	cd->fyrOrd[i][1] = toUpperCase[cd->fyrOrd[i][1]];
    }
    cset->noOfCorrections = i;
    cset->corrections = cd->fyrOrd;
    for (; i < cd->fyrAntalOrd; i++) free(cd->fyrOrd[i]);
    return 1;
  } else return 0;
}

static int IsCapitalized(unsigned char *ordin)
{ int i;
  if (!isUpperCase[ordin[0]]) return 0;
  for (i = 1; ordin[i] != '\0'; i++)
    if (isUpperCase[ordin[i]]) return 0;
  return 1;
}

static INLINE void FreeCorrData(struct corrData *cd)
{
  int i;
  for (i = 0; i < 2*MAXORDDELAR && cd->delMaxAntalOrd[i] > 0; i++) {
    PartClear(cd, i);
    free(cd->delOrd[i]);
  }
}

/* GenerateSimpleCorrections genererar rangordnade rättelseförslag på avstånd 1 i EL och IL från 
   ett potentiellt riktigt stavat ord word. Rättelseförslagen läggs i cset.
   Returnerar 0 om inget förslag kunde genereras och 1 annars. */
int GenerateSimpleCorrections(struct correctionSet *cset, unsigned char *word)
{
  struct corrData cd;
  memset(&cd, 0, sizeof(cd));
  Generera1(&cd, word, 0, 1, 0, 0, strlen((char *)word), CHECK_EL | CHECK_IL);
  FreeCorrData(&cd);
  return SorteraGenereradeOrd(cset, &cd, IsCapitalized(word));
}

/* GenerateCorrections genererar rangordnade rättelseförslag till word.
   Rättelseförslagen läggs i cset.
   Returnerar 0 om inget förslag kunde genereras och 1 annars. */
int GenerateCorrections(struct correctionSet *cset, unsigned char *word)
{ unsigned char ord2[LANGD + 3], Ord[LANGD + 3];
  int Capitalized = IsCapitalized(word);
  struct corrData cd;
  memset(&cd, 0, sizeof(cd));

  GenereraAlternativaOrd(&cd, word);
  GenereraLjudbyten(&cd, word);

  if (cd.fyrAntalOrd == 0) {
    VersalerGemena(word, ord2, Ord);
    if (*ord2) GenereraAlternativaOrd(&cd, ord2);
  } else if (Capitalized) {
    word[0] = toLowerCase[word[0]];
    GenereraAlternativaOrd(&cd, word);
  }
  FreeCorrData(&cd);
  return SorteraGenereradeOrd(cset, &cd, Capitalized);
}
