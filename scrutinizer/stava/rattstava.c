/* Rättstavningsprogram. Version 2.60 2004-09-05
   Copyright (C) 1990-2004
   Joachim Hollman och Viggo Kann
   joachim@algoritmica.se viggo@nada.kth.se

   Mikael Tillenius har utvecklat rangordningen 1995-1996, se
   exjobbsrapporten TRITA-NA-E9621
*/

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
static unsigned char **fyrOrd;
static int fyrAntalOrd = 0, fyrMaxAntalOrd = 0;
static unsigned char **delOrd[2*MAXORDDELAR];
static int delAntalOrd[2*MAXORDDELAR], delMaxAntalOrd[2*MAXORDDELAR];
static int addedWord;
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
    fprintf(stderr, "Out of memory\n");
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

static void AddSuggestion(uchar *ord, int point)
{ int i, r;
  int len = strlen((char *)ord);
  if (len <= 1) return; /* Strunta i enbokstavsord */
  if (InUL(ord, len)) return; /* Föreslå aldrig förbjudna ord */
  if (fyrMaxAntalOrd == 0) {
    fyrMaxAntalOrd = 20;
    fyrOrd = xmalloc(sizeof(char *) * fyrMaxAntalOrd);
  }
    
  for (i = 0; i < fyrAntalOrd; i++) {
    if (!strcmp((char *) ord + 1, (char *) fyrOrd[i] + 2)) {
      r = abs(*ord - *(fyrOrd[i]+1));
      if (r == 0
	  || r == CAPITALDIFF
	  ) {
	if ((int) fyrOrd[i][0] > point)
	  goto overwrite;
	else
	  return;
      }
    }
  }
  if (fyrAntalOrd >= fyrMaxAntalOrd) {
    fyrMaxAntalOrd += 20;
    fyrOrd = realloc(fyrOrd, sizeof(char *) * fyrMaxAntalOrd);
  }
  fyrAntalOrd++;
  fyrOrd[i] = xmalloc(len + 2);

overwrite:
  addedWord = 1;
  fyrOrd[i][0] = (point >= 256 ? 255 : (unsigned char) point);
  strcpy((char *) fyrOrd[i]+1, (char *) ord);
}

static void ClearSuggestions(void)
{ int i;
  for (i = 0; i < fyrAntalOrd; i++) free(fyrOrd[i]);
  fyrAntalOrd = 0;
}

static void Concat(uchar *to, uchar *word, int point, int part, int lastpart)
{ int len, i;

  len = strlen((char *)to);
  if (part == lastpart) {
    if (len > 2 && *word == to[len-1] && to[len-2] == to[len-1]) word++;
    strcpy((char *)to + len, (char *)word);
    AddSuggestion(to, point+WordFreq(to)+PARTCOST*lastpart);
  } else {
    for (i=0; i<delAntalOrd[part]; i++) {
      if (len > 2 && delOrd[part][i][1] == to[len-1] &&
	  to[len-2] == to[len-1]) 
	strcpy((char *) to + len, (char *) delOrd[part][i]+2);
      else strcpy((char *) to + len, (char *) delOrd[part][i]+1);
      Concat(to, word, point+delOrd[part][i][0], part+1, lastpart);
    }
    to[len] = 0;
  }
}

/* Sätt ihop de olika orddelarna i alla kombinationer och stoppa in i
   listan över ordförslag */
static void ConcatParts(uchar *word, int point, int part)
{
  uchar buf[LANGD];

  buf[0] = 0;
  Concat(buf, word, point, 0, part);
}

/* Lägg till en ny orddel */
static void AddPart(uchar *ord, int point, int part)
{ int i, r;

  if (delMaxAntalOrd[part] == 0) {
    delMaxAntalOrd[part] = 20;
    delOrd[part] = xmalloc(sizeof(char *) * delMaxAntalOrd[part]);
  }

  for (i = 0; i < delAntalOrd[part]; i++) {
    if (!strcmp((char *)ord + 1, (char *)delOrd[part][i] + 2)) {
      r = abs(*ord - *(delOrd[part][i]+1));
      if (r == 0
	  || r == CAPITALDIFF
	  ) {
        if ((int) delOrd[part][i][0] > point)
          goto overwrite;
        else
          return;
      }
    }
  }
  if (delAntalOrd[part] >= delMaxAntalOrd[part]) {
    delMaxAntalOrd[part] += 20;
    delOrd[part] = xrealloc(delOrd[part], sizeof(char *) * delMaxAntalOrd[part]);
  }
  delAntalOrd[part]++;
  addedWord = 1;
  delOrd[part][i] = xmalloc(strlen((char *)ord)+2);

  overwrite:
  delOrd[part][i][0] = (point >= 256 ? 255 : (unsigned char) point);
  strcpy((char *)delOrd[part][i]+1, (char *)ord);
}

/* rensa listan med orddelar */
static void PartClear(int part)
{
  int i;
  for (i = 0; i < delAntalOrd[part]; i++) free(delOrd[part][i]);
  delAntalOrd[part] = 0;
}

/* Kolla om word finns i ordlistan. Parametern check styr vilka av
   ordlistorna EL, IL och FL som ska användas */
INLINE void Check(uchar *word, int point, int part, int len, int check)
{
  if (check & CHECK_IL) {
    if (InUL(word, len)) return;
    if (InIL(word, len)) {
      AddSuggestion(word, point+WordFreq(word));
      return;
    }
  }
  if (check & CHECK_EL) {
    if (InEL(word, len) || (xAndelser && CheckSuffix(word, 0))) {
      ConcatParts(word, point, part);
      return;
    }
  }
  if (check & CHECK_FL) {
    if (InFL(word, len)) {
      AddPart(word, point+WordFreq(word), part);
      return;
    }
  }
  return;
}


static void GenereraLjudbyten(unsigned char *ord)
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
	      AddSuggestion(word, REPPVAL-1 + WordFreq(word));
	    else if ((noofparts = SimpleIsCompound(word, 
						   strlen((char *)word))))
	      AddSuggestion(word, 
			    REPPVAL-1 + WordFreq(word) + PARTCOST*(noofparts-1));
	  }
	}
    notsame: continue;
    }
  }
}

static void Generera1(unsigned char *word, int from, int errors, int point, int part, 
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
    Check(word, point, part, len, check);

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
        Check(word, point+SWAP(word[i+1], word[i]), part, len, check);
    } else {
      Generera1(word, i+1, errors-1,
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
	  Check(word2, point+DELP(word[i], word2[i])+((i==0)?FIRSTP:0),
		part, len-1, check);
      } else {
	Generera1(word2, i, errors-1,
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
	      Check(word, 
		    point+REPP(tmp1,*lett)+((*lett==UP)?0:firstpoint),
		    part, len, check);
	  } else {
	    Generera1(word, i+1, errors-1,
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
            Check(word, point+REPP(tmp1,*lett)+((*lett==low)?0:firstpoint),
                  part, len, check);
        } else {
          Generera1(word, i+1, errors-1,
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
	    Check(word2, INSP(word2[i], word2[i+1])+((i==0)?FIRSTP:0),
		  part, len+1, check);
	} else {
	  Generera1(word2, i+1, errors-1,
		    INSP(word2[i], word2[i+1])+((i==0)?FIRSTP:0), part,
		    len+1, check);
	}
      }
    }
    for (lett = lowerCaseLetters; *lett; lett++) {
      word2[i] = *lett;
      if (errors == 1) {
        if (FyrKoll(word2, i, 0))
          Check(word2, INSP(word2[i], word2[i+1])+((i==0)?FIRSTP:0),
                part, len+1, check);
      } else {
        Generera1(word2, i+1, errors-1,
                  INSP(word2[i], word2[i+1])+((i==0)?FIRSTP:0), part,
                  len+1, check);
      }
    }
    word2[i] = word2[i+1];
  }
  word2[len] = '\0';
}

/* Insert1 prövar att stoppa in exakt en bokstav i ordet */
static int Insert1(unsigned char *word, int point, int part, int len, int check)
{ int i;
  unsigned char *lett;
  unsigned char word2[LANGD];
  addedWord = 0;
  strcpy((char *)word2+1, (char *)word);
  for (i = 0; i < len+1; i++) {
    if ((i == 0 && part == 0) || 
	(i > 0 && (isUpperCase[word[i-1]] || isDelim[word[i-1]]))) {
      for (lett = upperCaseLetters; *lett; lett++) {
	word2[i] = *lett;
	if (FyrKoll(word2, i, 0))
	  Check(word2, INSP(word2[i], word2[i+1])+((i==0)?FIRSTP:0),
		part, len+1, check);
      }
    }
    for (lett = lowerCaseLetters; *lett; lett++) {
      word2[i] = *lett;
      if (FyrKoll(word2, i, 0))
	Check(word2, INSP(word2[i], word2[i+1])+((i==0)?FIRSTP:0),
	      part, len+1, check);
    }
    word2[i] = word2[i+1];
  }
  /* word2[len] = '\0'; */
  return (addedWord ? 1 : -1);
}


/* InsertSpace tries to insert a space inside the word */
void InsertSpace(unsigned char *wordin, int len)
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
	AddSuggestion(buf, INSPVAL + 5 + PARTCOST*2 + point + WordFreq(wordin + i));
      }
      word[i] = wordin[i];
      if (i > 2) {
	word[i-1] = '\0';
	if (InILorELbutnotUL(word, i-1)) {
	  point = WordFreq(word);
	  sprintf((char *)buf, "%s %s", word, wordin + i);
	  AddSuggestion(buf, DELPVAL + 5 + PARTCOST*2 + point + WordFreq(wordin + i));
	}
	word[i-1] = wordin[i-1];
      }
    }
  }
}

/* Generera rättstavningsförslag för word. Högst errors fel får förekomma */
static int Generera(unsigned char *word, int errors, int point, int part,
		    int len, int check)
{ int i;

  if (len > DELORDMAX) return -1;

  if (errors > 1) {
    if (len <= 6 || xMaxOneError || part || point) errors = 1;
    else if (errors > 2) errors = 2;
  }
  addedWord = 0;
  Check(word, 0, part, len, check);
  if (addedWord && check == CHECK_FL) return 0; /* Viggo 1999-11-25, återinfört 2002-02-09 */
  for (i=1; i<=errors; i++) {
    /*fprintf(stderr, "{Gen %s %d %d}", word, i, errors);*/
    Generera1(word, 0, i, point, part, len, check);
    if (addedWord) return i;
  }
  return -1;
}

/* Kolla om word är sammansatt som FL* EL. Om xTillatSIFogar
   så tillåt 's' i all fogar utom mellan 1:a och 2:a delen.
   Om xTillatSIAllaFogar så tillåt 's' i alla fogar. */
/* offset är platsen i ordet där word börjar */
static INLINE void CompoundEdit(uchar *word, int offset, int part, int errors,
				int len)
{ int end, tmp, before, res, startpos, finalpos;

  /* printf("CompoundEdit(%s,offset=%d,part=%d,errors=%d,len=%d\n", word, offset, part, errors, len); */
  if (errors == 0) {
    addedWord = 0;
    Check(word, 0, part, len, CHECK_EL);
    if (addedWord) return; /* negation borttagen 2003-04-17 */
    if (!xGenerateCompounds || part+1 >= MAXORDDELAR) return;
    startpos = offset ? DELORDMIN : STARTDELORDMIN;
    finalpos = len - DELORDMIN;
    for (end=startpos; end<=finalpos; end++) {
      tmp = word[end];
      word[end] = 0;
      before = fyrAntalOrd;
      PartClear(part);
      addedWord = 0;
      Check(word, 0, part, end, CHECK_FL);
      if (addedWord) {
	word[end] = tmp;
	/* Hantera t ex toppolitiker som topp|politiker */
	if (word[end-1] == word[end-2]) {
	  if (word[end-1] == tmp) {
	    continue;
	  }
	  CompoundEdit(word+end-1, offset+end-1, part+1, 0, len-end+1);
	}
	CompoundEdit(word+end, offset+end, part+1, 0, len-end);
	if (((xTillatSIFogar && offset) || xTillatSIAllaFogar) &&
	    word[end] == 's' &&
	    bindebokstav[(unsigned char)word[end-1]] == 's') {
	  PartClear(part+1);
	  AddPart((uchar *)"s", 0, part+1);
	  CompoundEdit(word+end+1, offset+end+1, part+2, 0, len-end-1);
	}
      }
      else word[end] = tmp;
    }
    return;
  } 
  if (offset == 0) {
    res = Generera(word, errors, 0, part, len, CHECK_EL | CHECK_IL);
    if (res == 0 || res == 1) return;
  } else {
    if (Generera(word, errors, 0, part, len, CHECK_EL) != -1) return;
  }

  if (!xGenerateCompounds || part+1 >= MAXORDDELAR) return;

  startpos = offset ? DELORDMIN : STARTDELORDMIN;
  finalpos = len - DELORDMIN;
  for (end=startpos; end<=finalpos; end++) {
    tmp = word[end];
    word[end] = 0;
    before = fyrAntalOrd;
    PartClear(part);
    
    if (end == startpos) res = Insert1(word, 0, part, end, CHECK_FL);
    else res = Generera(word, errors, 0, part, end, CHECK_FL);

    if (res != -1) {
      errors -= res;
      word[end] = tmp;
      /* Hantera t ex toppolitiker som topp|politiker */
      if (word[end-1] == word[end-2]) {
	if (word[end-1] == tmp) {
	  errors += res;
	  continue;
	}
        CompoundEdit(word+end-1, offset+end-1, part+1, errors, len-end+1);
      }
      CompoundEdit(word+end, offset+end, part+1, errors, len-end);

      if (((xTillatSIFogar && offset) || xTillatSIAllaFogar) &&
          word[end] == 's' &&
          bindebokstav[(unsigned char)word[end-1]] == 's') {
        PartClear(part+1);
        AddPart((uchar *)"s", 0, part+1);
        CompoundEdit(word+end+1, offset+end+1, part+2, errors, len-end-1);
      }
      errors += res;
    }
    else word[end] = tmp;
  }
}

static void GenereraAlternativaOrd(uchar *wordin)
{ int len = strlen((char *)wordin);
  if (len < 2)
    return;
  if (len < 7 || xMaxOneError)
    CompoundEdit(wordin, 0, 0, 1, len);
  else
    CompoundEdit(wordin, 0, 0, 2, len);
  if (len >= 4) InsertSpace(wordin, len);
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
            
/* SkrivGenereradeOrd sorterar dom genererade orden efter trolighetsvärdet och
skriver ut dom med StavaSkrivOrd */
static int SkrivGenereradeOrd(int Capitalized)
{  int i, swapped, truncSuggestions;
  unsigned char *tmp;
  int printed = 0;

  if (fyrAntalOrd > 0) {
    /* bubblesort the list */
    do {
      swapped = 0;
      for (i=0; i<fyrAntalOrd-1; i++) {
        if (fyrOrd[i][0] > fyrOrd[i+1][0]) {
          tmp = fyrOrd[i];
          fyrOrd[i] = fyrOrd[i+1];
          fyrOrd[i+1] = tmp;
          swapped = 1;
        }
      }
    } while (swapped);

    truncSuggestions = fyrOrd[0][0] + truncSuggestionsOffset;
    /* display the list */
    for (i = 0; i < fyrAntalOrd; i++) {
      /*printf("%d ", fyrOrd[i][0]);*/
      if (fyrOrd[i][0] > truncSuggestions - 2 * i)
	if (!xDebug)
	  break;
      if (Capitalized && isLowerCase[fyrOrd[i][1]]) 
	fyrOrd[i][1] = toUpperCase[fyrOrd[i][1]];
      if (printed++) StavaSkrivSeparator();
      StavaSkrivOrd(fyrOrd[i]+1);
      if (xDebug) {
	if (fyrOrd[i][0] > truncSuggestions - 2 * i)
	  printf("((%d))", (int) fyrOrd[i][0]);
	else printf("(%d)", (int) fyrOrd[i][0]);
      }
      if (i > MAXSUGGESTIONS)
        break;
    }
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

/* SimpleCorrections genererar rättelser på avstånd 1 i EL och IL från 
   ett potentiellt riktigt stavat ord. Returnerar 0 om inget förslag kunde
   genereras och 1 annars. */
int SimpleCorrections(unsigned char *word)
{
  ClearSuggestions();
  Generera1(word, 0, 1, 0, 0, strlen((char *)word), CHECK_EL | CHECK_IL);
  return SkrivGenereradeOrd(IsCapitalized(word));
}

/* SkrivForslag genererar rättstavningsförslag för ett ord och skriver 
   ut dom med StavaSkrivOrd(). Returnerar 0 om inget förslag kunde
   genereras och 1 annars. */
int SkrivForslag(unsigned char *ordin)
{ unsigned char ord2[LANGD + 3], Ord[LANGD + 3];
  int Capitalized = IsCapitalized(ordin);

  ClearSuggestions();
  GenereraAlternativaOrd(ordin);
  GenereraLjudbyten(ordin);

  if (fyrAntalOrd == 0) {
    VersalerGemena(ordin, ord2, Ord);
    if (*ord2) GenereraAlternativaOrd(ord2);
  } else if (Capitalized) {
    ordin[0] = toLowerCase[ordin[0]];
    GenereraAlternativaOrd(ordin);
  }
  return SkrivGenereradeOrd(Capitalized);
}

