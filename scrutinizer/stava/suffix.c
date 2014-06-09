/* Rättstavningsprogram. Version 2.61  2004-10-31
   Copyright (C) 1990-2004
   Joachim Hollman och Viggo Kann
   joachim@nada.kth.se viggo@nada.kth.se
*/

/* #define ENGELSKA */ /* Engelsk stavningskontroll */
#define SVENSKA /* Svensk stavningskontroll */

#include "suffix.h"
#include "stava.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct suffixchecklist {
  unsigned char *checksuffix;
  int suffixlen;
  struct suffixchecklist *next;
  int accept;
} suffixchecklist;

#ifndef MAXNOOFFORBIDDENENDINGS
#define MAXNOOFFORBIDDENENDINGS 6
#endif

typedef struct suffixset {
  /* Ingångssuffix (framlänges och baklänges) och dess längd: */
  unsigned char *suffix, *reversesuffix;
  int len;
  /* Lista med uppslagssuffix: */
  struct suffixchecklist *check;
  /* Binärt sökträd sorterat efter reversesuffix: */
  struct suffixset *left, *right; 
  /* Positiv eller negativ regel: */
  int accept;
  /* Tillåtna och otillåtna tecken i pos precis före ändelsen: */
  unsigned char *allowedlastcharacters; 
  unsigned char *forbiddenlastcharacters;
  /* Otillåtna strängar precis före ändelsen: */
  unsigned char *forbiddenending[MAXNOOFFORBIDDENENDINGS + 1];
  int forbiddenendinglength[MAXNOOFFORBIDDENENDINGS];
  /* Ska sökningen sluta efter första stavelsen eller vid ordets början: */
  int stopsearchatbeginning;
} suffixset;

static suffixset *root = NULL;
static suffixset **set;
static int SuffixStart[257];
static int noOfElements, setIndex, setIndexLeft, setIndexRight, charNumber;

/* Reverse skapar ett nytt ord som är word läst baklänges. */
static unsigned char *Reverse(unsigned char *word)
{ unsigned char *s = malloc(strlen((char *)word) + 1), *t;
  t = s + strlen((char *)word);
  *t-- = '\0';
  while (*word) *t-- = *word++;
  return s;
}

/* InsertSuffix stoppar in suffixet suf i det binära sökträdet. */
static void InsertSuffix(suffixset *suf)
{ suffixset *p = root;
  int comp;
  noOfElements++;
  if (p == NULL) {
    root = suf;
    return;
  }
  while (1) {
    comp = strcmp((char *)suf->reversesuffix, (char *)p->reversesuffix);
    if (comp == 0 && !suf->accept) comp = -1;
    if (comp >= 0) {
      if (p->right == NULL) {
	p->right = suf;
	return;
      }
      p = p->right;
    } else {
      if (p->left == NULL) {
	p->left = suf;
	return;
      }
      p = p->left;
    }
  }
}

/* NextWord letar efter nästa ord i s och sätter start att peka på början av
 * det. Efter ordet sätts '\0' in och efterföljande position returneras.
 * Om inget ord finns i s returneras NULL.
 * @ betyder tomma strängen. # gör resten av raden till kommentar */
static unsigned char *NextWord(unsigned char *s, unsigned char **start)
{
  while (*s && *s < '@' && *s != '#' && *s != '<' && *s != '-') s++;
  if (*s == '<') {
    *start = s;
    return NULL;
  }
  if (*s == '\0' || *s == '#') return NULL;
  *start = s;
  if (*s != '@')
    while (*s >= 'A' || *s == '-') {
      if (*s == '~') *s = '_'; else *s = ISO_intern[*s];
      s++;
    }
  if (*s == '\0') return s;
  if (*s == '#') s[1] = '#';
  *s = '\0';
  return s + 1;
}

static void TreeToArray(suffixset *suf)
{
  if (suf->left) TreeToArray(suf->left);
  set[setIndex++] = suf;
  if (suf->right) TreeToArray(suf->right);
}

static void WriteSuffix(suffixset *suf)
{ suffixchecklist *p;
  unsigned char **fep = suf->forbiddenending;
  if (suf->stopsearchatbeginning) WriteISO((const unsigned char *) "<B>");
  if (*fep) {
    printf("(");
    WriteISO(*fep);
    for (fep++; *fep; fep++) {
      printf("|");
      WriteISO(*fep);
    }
    printf(")");
  }
  if (suf->allowedlastcharacters && *suf->allowedlastcharacters) {
    printf("[");
    WriteISO(suf->allowedlastcharacters);
    printf("]");
  }
  if (suf->forbiddenlastcharacters && *suf->forbiddenlastcharacters) {
    printf("[^");
    WriteISO(suf->forbiddenlastcharacters);
    printf("]");
  }

  if (!suf->accept) /* negerad regel */
    printf("~");
  if (*suf->suffix) WriteISO(suf->suffix);
  else printf("@");
  printf(" \t");
  for (p = suf->check; p; p = p->next) {
    if (!p->accept) putchar('~');
    if (*p->checksuffix == '\0') printf("@ ");
    else {
      WriteISO(p->checksuffix);
      putchar(' ');
    }
  }
  printf("\n");
}

static void WriteSuffixTree(suffixset *suf)
{ 
  if (suf->left) WriteSuffixTree(suf->left);
  WriteSuffix(suf);
  if (suf->right) WriteSuffixTree(suf->right);
}
 
/* WriteSuffixList skriver ut suffixtabellen (i avlusningssyfte) */
void WriteSuffixList(void)
{ if (root) WriteSuffixTree(root); }

/* ParseSufLine hanterar en regel och returnerar 1 om allt gick bra */
static int ParseSufLine(unsigned char *line, int len)
{ suffixset *newsuf;
  suffixchecklist *p;
  unsigned char *s, *start, *freeS;
    s = malloc(len + 1);
    freeS = s;
    strcpy((char *)s, (char *)line);
    newsuf = malloc(sizeof(*newsuf));
    newsuf->allowedlastcharacters = newsuf->forbiddenlastcharacters = NULL;
    newsuf->forbiddenending[0] = NULL;
    newsuf->stopsearchatbeginning = 0;
    if (*s == '<') {
      s++;
      switch (*s) {
      case 'v':
      case 'V': /* Stop search at first vowel (default) */
	newsuf->stopsearchatbeginning = 0;
	break;
      case 'b':
      case 'B': /* Stop search at beginning of word */
	newsuf->stopsearchatbeginning = 1;
	break;
      default:
	sprintf(stavaerrorbuf, "Fel i suffixtabellen. Felaktigt <*>-uttryck.\n");
	if (xPrintError) fprintf(stderr, "%s", stavaerrorbuf);
	return 0;
      }
      while (*s && *s++ != '>');
    }
    if (*s == '(') {
      int last = 0, ending = 0;
      unsigned char *t;
      unsigned char **p = newsuf->forbiddenending;
      s++;
      t = s; 
      do {
	for (; *t && *t != ')' && *t != '|'; t++)
	  *t = ISO_intern[*t];
	if (*t) {
	  if (*t == ')') last = 1;
	  *t++ = '\0';
	  newsuf->forbiddenendinglength[ending] = strlen((char *)s);
	  *p++ = s;
	  s = t;
	} else {
	  sprintf(stavaerrorbuf, "Fel i suffixtabellen. Oavslutat (*)-uttryck.\n");
	  if (xPrintError) fprintf(stderr, "%s", stavaerrorbuf);
	  break;
	}
      } while (!last && ++ending < MAXNOOFFORBIDDENENDINGS);
      if (!last) {
	if (ending >= MAXNOOFFORBIDDENENDINGS) {
	  sprintf(stavaerrorbuf, 
		  "Fel i suffixtabellen. För många förbjudna ändelser.\n");
	  if (xPrintError) fprintf(stderr, "%s", stavaerrorbuf);
	}
	return 0; /* oavslutat (*)-uttryck */
      }
      *p = NULL;
    }      
    if (*s == '[') {
      int dis = 0;
      unsigned char *t;
      s++;
      if (*s == '^' || *s == '~') { dis = 1; s++; }
      for (t = s; *t && *t != ']'; t++)
	*t = ISO_intern[*t];
      if (*t) {
	*t++ = '\0';
	if (dis) newsuf->forbiddenlastcharacters = s;
	else newsuf->allowedlastcharacters = s;
	s = t;
      } else {
	sprintf(stavaerrorbuf, "Fel i suffixtabellen. Oavslutat [*]-uttryck.\n");
	if (xPrintError) fprintf(stderr, "%s", stavaerrorbuf);
	return 0;
      }
    }      
    if ((s = NextWord(s, &start)) == NULL) free(newsuf); 
    else {
      if (*start == '_') { /* negerad regel */
	newsuf->accept = 0;
	start++;
      } else newsuf->accept = 1;
      newsuf->suffix = start;
      newsuf->reversesuffix = Reverse(start);
      newsuf->len = strlen((char *)start);
      newsuf->left = newsuf->right = NULL;
      InsertSuffix(newsuf);
      if ((s = NextWord(s, &start)) == NULL) {
	sprintf(stavaerrorbuf, "Fel i suffixtabellen. Suffixet %s saknar regler.\n",
		newsuf->suffix);
	if (xPrintError) fprintf(stderr, "%s", stavaerrorbuf);
	newsuf->check = NULL;
	return 0;
      }
      newsuf->check = p = malloc(sizeof(*p));
      if (*start == '_') { /* negerat suffix */
	p->accept = 0;
	start++;
      } else p->accept = 1;
      p->checksuffix = start;
      
      while ((s = NextWord(s, &start)) != NULL) {
	p->suffixlen = strlen((char *)p->checksuffix);
	p->next = malloc(sizeof(*p->next));
	p = p->next;
	if (*start == '_') { /* negerat suffix */
	  p->accept = 0;
	  start++;
	} else p->accept = 1;
	p->checksuffix = start;
      }
      p->suffixlen = strlen((char *)p->checksuffix);
      p->next = NULL;
    }
    free(freeS); //wille, pekar på s början av s som aldrig frigörs;
    return 1;
}

/* InitSuf initierar suffixtabellen och returnerar 0 om det gick bra. */
int InitSuf(const char *SLfilename)
{ FILE *fp;
  unsigned char line[200];
  int len, i, j;
  fp = fopen(SLfilename, "r");
  if (!fp) return 1;
  while (fgets((char *)line, 200, fp)) {
    len = strlen((char *)line);
    if (line[len - 1] == '\n') line[--len] = '\0';
    if (len == 0) continue;
    ParseSufLine(line, len);
  }
  fclose(fp);
  set = malloc(sizeof(*set) * (noOfElements + 1));
  setIndex = 0;
  TreeToArray(root);
  set[noOfElements] = NULL;
  if (setIndex != noOfElements) {
    sprintf(stavaerrorbuf, "InitSuf: antalet suffix stämmer inte.\n");
    if (xPrintError) fprintf(stderr, "%s", stavaerrorbuf);
  }
  for (i = 0; i < 256; i++) SuffixStart[i] = -1;
  for (j = 0; j < noOfElements; j++) {
    i = set[j]->reversesuffix[0];
    if (SuffixStart[i] == -1) SuffixStart[i] = j;
  }
  SuffixStart[256] = noOfElements;
  for (i = 256; i > 0; i--)
    if (SuffixStart[i - 1] == -1) SuffixStart[i - 1] = SuffixStart[i];
  return 0;
}

/* ReverseCompare jämför s baklänges med rev (som redan är bakvänd) */
static INLINE int ReverseCompare(unsigned char *s, int len, unsigned char *rev)
{ unsigned char *t = s + len - 1;
  while (t >= s && *rev) {
    if (*t < *rev) return -1;
    if (*t > *rev) return 1;
    t--;
    rev++;
  }
  if (*rev == '\0') return 0;
  return -1;
}

static INLINE int BinarySearch(unsigned char c)
{ int mid, left, right = setIndexRight;
  while (setIndexLeft < right) {
    mid = (setIndexLeft + right) / 2;
    if (set[mid]->len <= charNumber ||
	set[mid]->reversesuffix[charNumber] < c) setIndexLeft = mid + 1;
    else right = mid;
  }
  if (set[right]->len > charNumber &&
      set[right]->reversesuffix[charNumber] == c) {
    /* Räkna ut var det tänkbara intervallet slutar */
    left = right;
    while (left < setIndexRight) {
      mid = (left + setIndexRight + 1) / 2;
      if (set[mid]->len <= charNumber ||
	  set[mid]->reversesuffix[charNumber] <= c) left = mid;
      else setIndexRight = mid - 1;
    }
    return 1;
  }
  return 0;
}

/* FindNextSuffix letar rätt på nästa förekomst (efter set[setIndexLeft]) i
suffixlistan av ett suffix som matchar slutet av word.
Stammen (dvs delen före suffixet) kopieras till stem. */
static suffixset *FindNextSuffix(const unsigned char *word, int len, 
				 int firstvowel, unsigned char *stem,
				 int *outstemlen)
{ int stemlen, i, ending;
  int maxsuffixlen = len - PREFIXMIN;
  unsigned char **p;
 beginFindNextSuffix:
  if (++setIndexLeft <= setIndexRight) {
    if (strcmp((char *)set[setIndexLeft - 1]->suffix, 
	       (char *)set[setIndexLeft]->suffix) == 0) {
      stemlen = len - set[setIndexLeft]->len;
      if (stemlen < firstvowel && !set[setIndexLeft]->stopsearchatbeginning)
	goto beginFindNextSuffix;
      for (p = set[setIndexLeft]->forbiddenending, ending = 0; *p; 
	   p++, ending++) {
	i = set[setIndexLeft]->forbiddenendinglength[ending];
	if (strncmp((char *)word + stemlen - i, (char *)*p, i) == 0)
	  goto beginFindNextSuffix;
      }
      if (set[setIndexLeft]->forbiddenlastcharacters) {
	if (strchr((char *)set[setIndexLeft]->forbiddenlastcharacters,
		   word[stemlen - 1]) != NULL)
	  goto beginFindNextSuffix;
      } else if (set[setIndexLeft]->allowedlastcharacters) {
	if (strchr((char *)set[setIndexLeft]->allowedlastcharacters, 
		   word[stemlen - 1]) == NULL)
	  goto beginFindNextSuffix;
      }
      strcpy((char *)stem, (char *)word);
      stem[stemlen] = '\0';
      *outstemlen = stemlen;
      return set[setIndexLeft];
    }
    for (charNumber++; charNumber < maxsuffixlen; charNumber++) {
      if (!BinarySearch(word[len - charNumber - 1])) return NULL;
      if (set[setIndexLeft]->len == charNumber + 1) {
	stemlen = len - set[setIndexLeft]->len;
	if (stemlen < firstvowel && !set[setIndexLeft]->stopsearchatbeginning)
	  goto beginFindNextSuffix;
	for (p = set[setIndexLeft]->forbiddenending, ending = 0; *p; 
	     p++, ending++) {
	  i = set[setIndexLeft]->forbiddenendinglength[ending];
	  if (strncmp((char *)word + stemlen - i, (char *)*p, i) == 0)
	    goto beginFindNextSuffix;
	}
	if (set[setIndexLeft]->forbiddenlastcharacters) {
	  if (strchr((char *)set[setIndexLeft]->forbiddenlastcharacters, 
		     word[stemlen - 1]) != NULL)
	    goto beginFindNextSuffix;
	} else if (set[setIndexLeft]->allowedlastcharacters) {
	  if (strchr((char *)set[setIndexLeft]->allowedlastcharacters, 
		     word[stemlen - 1]) == NULL)
	    goto beginFindNextSuffix;
	}
	strcpy((char *)stem, (char *)word);
	stem[stemlen] = '\0';
        *outstemlen = stemlen;
	return set[setIndexLeft];
      }
    }
  }
  return NULL;
}

/* FindSuffix letar rätt på första förekomsten i suffixlistan av ett suffix
som matchar slutet av word. 
Stammen (dvs delen före suffixet) kopieras till stem. */
static suffixset *FindSuffix(const unsigned char *word, int len, 
			     int firstvowel, unsigned char *stem,
			     int *outstemlen)
{ int stemlen, i, ending;
  int maxsuffixlen = len - PREFIXMIN;
  unsigned char **p;
  setIndexLeft = SuffixStart[word[len - 1]];
  setIndexRight = SuffixStart[word[len - 1] + 1] - 1;
  if (setIndexLeft > setIndexRight) return NULL;
  charNumber = 0;
  if (set[setIndexLeft]->len == 1) {
    stemlen = len - 1;
    for (p = set[setIndexLeft]->forbiddenending, ending = 0; *p; 
	 p++, ending++) {
      i = set[setIndexLeft]->forbiddenendinglength[ending];
      if (strncmp((char *)word + stemlen - i, (char *)*p, i) == 0)
	return FindNextSuffix(word, len, firstvowel, stem, outstemlen);
    }
    if (set[setIndexLeft]->forbiddenlastcharacters) {
      if (strchr((char *)set[setIndexLeft]->forbiddenlastcharacters, 
		 word[stemlen - 1]) != NULL)
	return FindNextSuffix(word, len, firstvowel, stem, outstemlen);
    } else if (set[setIndexLeft]->allowedlastcharacters) {
      if (strchr((char *)set[setIndexLeft]->allowedlastcharacters, 
		 word[stemlen - 1]) == NULL)
	return FindNextSuffix(word, len, firstvowel, stem, outstemlen);
    }
    strcpy((char *)stem, (char *)word);
    stem[stemlen] = '\0';
    *outstemlen = stemlen;
    return set[setIndexLeft];
  }
  for (charNumber = 1; charNumber < maxsuffixlen; charNumber++) {
    if (!BinarySearch(word[len - charNumber - 1])) return NULL;
    if (set[setIndexLeft]->len == charNumber + 1) {
      stemlen = len - set[setIndexLeft]->len;
      if (stemlen < firstvowel && !set[setIndexLeft]->stopsearchatbeginning)
	return FindNextSuffix(word, len, firstvowel, stem, outstemlen);
      for (p = set[setIndexLeft]->forbiddenending, ending = 0; *p; 
	   p++, ending++) {
	i = set[setIndexLeft]->forbiddenendinglength[ending];
	if (strncmp((char *)word + stemlen - i, (char *)*p, i) == 0)
	  return FindNextSuffix(word, len, firstvowel, stem, outstemlen);
      }
      if (set[setIndexLeft]->forbiddenlastcharacters) {
	if (strchr((char *)set[setIndexLeft]->forbiddenlastcharacters, 
		   word[stemlen - 1]) != NULL)
	  return FindNextSuffix(word, len, firstvowel, stem, outstemlen);
      } else if (set[setIndexLeft]->allowedlastcharacters) {
	if (strchr((char *)set[setIndexLeft]->allowedlastcharacters, 
		   word[stemlen - 1]) == NULL)
	  return FindNextSuffix(word, len, firstvowel, stem, outstemlen);
      }
      strcpy((char *)stem, (char *)word);
      stem[stemlen] = '\0';
      *outstemlen = stemlen;
      return set[setIndexLeft];
    }
  }
  return NULL;
}
    
/* CheckEmptySuffix kollar om ett tomt suffix matchar */
static int CheckEmptySuffix(suffixset *suf, const unsigned char *word, int len)
{ int i, ending;
  unsigned char **p;

  for (p = suf->forbiddenending, ending = 0; *p; 
       p++, ending++) {
    i = suf->forbiddenendinglength[ending];
    if (strncmp((char *)word + len - i, (char *)*p, i) == 0)
      return 0;
  }
  if (suf->forbiddenlastcharacters) {
    if (strchr((char *)suf->forbiddenlastcharacters, word[len - 1])
	!= NULL)
      return 0;
  } else if (suf->allowedlastcharacters) {
    if (strchr((char *)suf->allowedlastcharacters, word[len - 1])
	== NULL)
      return 0;
  }
  return 1;
}

/* CacheCheckWord kollar ord i InEL med cacheminne */
static INLINE int CacheCheckWord(unsigned char *word, int len)
{ int i, j;
  static int noOfCheckedWords;
  static unsigned char checkedWord[MAXSUFFIXMEMSIZE][LANGD];
  static int checkedWlen[MAXSUFFIXMEMSIZE];
  static int checkedWres[MAXSUFFIXMEMSIZE];
  if (word == NULL) {
    noOfCheckedWords = 0; /* initiera */
    return -1;
  }
  for (i = 0; i < noOfCheckedWords; i++)
    if (len == checkedWlen[i]) {
      for (j = len - 1; j >= 0; j--) /* jämför orden bakifrån */
	if (word[j] != checkedWord[i][j]) break;
      if (j < 0) return checkedWres[i];
    }
  i = (InEL(word, len) != 0);
  if (noOfCheckedWords < MAXSUFFIXMEMSIZE) {
    strcpy((char *)checkedWord[noOfCheckedWords], (char *)word);
    checkedWlen[noOfCheckedWords] = len;
    checkedWres[noOfCheckedWords++] = i;
  }
  return i;
}

/* CheckSuffix kollar om word innehåller suffix i suffixtabellen. För varje
rad i suffixtabellen som stämmer överens kollas att ordet finns i ordlistan
om suffixet byts ut mot alla kollsuffix i så fall returneras 1. Annars
returneras 0. 
Om tryallrules=1 (och TRYALLRULES är definierat) så gås alla regler som
matchar igenom och matchningarna lagras i arrayerna lemma och tag. Antalet
olika matchiningar lagras i nooftags. */
int CheckSuffix(const unsigned char *word, int tryallrules)
{ unsigned char word2[LANGD], *word2end;
  int len = strlen((char *)word), minprefixlen = 0, firstvowel, word2len;
  int i;
  suffixset *suf;
  suffixchecklist *p;
  if (len < PREFIXMIN) return 0;
  for (; isConsonant[word[minprefixlen]]; minprefixlen++);
  minprefixlen++;
  if (minprefixlen < PREFIXMIN) firstvowel = PREFIXMIN; else 
    firstvowel = minprefixlen;
  CacheCheckWord(NULL, 0);
  if (len > PREFIXMIN) {
    for (suf = FindSuffix(word, len, firstvowel, word2,
			&word2len);
	 suf; 
	 suf = FindNextSuffix(word, len, firstvowel, word2, &word2len)) {
      word2end = word2 + word2len;
      for (p = suf->check; p; p = p->next) {
	strcpy((char *)word2end, (char *)p->checksuffix);
	if (CacheCheckWord(word2, word2len + p->suffixlen) != p->accept) 
	  goto nextSuffix;
      }
      if (xDebug) {
	WriteISO(word);
	printf(" matchar suffixregeln ");
	*word2end = '\0';
	WriteISO(word2);
	putchar('|');
	WriteSuffix(suf);
      }
      return suf->accept; /* Alla villkor OK */
      nextSuffix: continue;
    }
  }
  for (i = SuffixStart[0]; i < SuffixStart[1]; i++) {
    suf = set[i];
    if (CheckEmptySuffix(suf, word, len)) {
      strcpy((char *)word2, (char *)word);
      word2end = word2 + len;
      for (p = suf->check; p; p = p->next) {
	strcpy((char *)word2end, (char *)p->checksuffix);
        if (CacheCheckWord(word2, len + p->suffixlen) != p->accept) 
	  goto nextSuffix2;
      }
      if (xDebug) {
	WriteISO(word);
	printf(" matchar suffixregeln ");
	WriteISO(word);
	putchar('|');
	WriteSuffix(suf);
      }
      return suf->accept; /* Alla villkor OK */
     nextSuffix2: continue;
    }
  }
  return 0;
}
