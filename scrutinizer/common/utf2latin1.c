#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utf2latin1.h"

/* Assuming the text is Swedish text, this should return 1 if the
   encoding is UTF-8, and it should return 0 if we find something that
   looks like latin-1 */
int looksLikeUTF(const char *s) {
  if(s == NULL) {
    return 1;
  }

  int last = 1;
  
  int i = 0;
  for(; s[i] != '\0'; i++) {
    if(s[i] < 0 && last > 0) {
      if(s[i] == -27
	 || s[i] == -28
	 || s[i] == -10
	 || s[i] == -59
	 || s[i] == -60
	 || s[i] == -42) {
	return 0;
      }
    }
    if(s[i] < 0 && last == -61) {
      return 1;
    }
    last = s[i];
  }
  
  return 1;
}

/* Converts UTF-8 to Latin-1 correctly if we assume the string
   contents are Swedish text. Based on code example from Mark Ransom */
void utf2latin1(char *s) {
  
  int n = strlen(s);
  /*char *tempBuf = new char[n+1];*/
  char *tempBuf = (char *) malloc(sizeof(char) * (n+1));

  unsigned int codepoint = 0;

  int j = 0;
  int i = 0;
  for(; s[i] != 0; ) {
    /* unsigned char c = static_cast<unsigned char>(s[i]); */
    unsigned char c = (unsigned char)(s[i]);
    if (c <= 0x7f)
      codepoint = c;
    else if (c <= 0xbf)
      codepoint = (codepoint << 6) | (c & 0x3f);
    else if (c <= 0xdf)
      codepoint = c & 0x1f;
    else if (c <= 0xef)
      codepoint = c & 0x0f;
    else
      codepoint = c & 0x07;

    i++;
    
    if (((s[i] & 0xc0) != 0x80) && (codepoint <= 0x10ffff)) {
      if (codepoint <= 255) {
	/* tempBuf[j++] = static_cast<char>(codepoint); */
	tempBuf[j++] = (char)(codepoint);
      } else {
	// do whatever you want for out-of-bounds characters
	tempBuf[j++] = 'X'; // Store something away so we can convert back later? TODO: Jonas
      }
    }
  }

  int k = 0;
  for(; k < j; k++) {
    s[k] = tempBuf[k];
  }
  s[j] = '\0';

  /* delete [] tempBuf; */
  free(tempBuf);
}




/* Converts UTF-8 to Latin-1 correctly if we assume the string
   contents are Swedish text. Based on code example from Mark Ransom

   Same as above, except a copy is created and returned, and that copy
   needs to be free()d by the caller.
 */
char *utf2latin1Copy(const char *s) {
  
  int n = strlen(s);
  char *tempBuf = (char *) malloc(sizeof(char) * (n+1));

  unsigned int codepoint = 0;

  int j = 0;
  int i = 0;
  for(; s[i] != 0; ) {
    /* unsigned char c = static_cast<unsigned char>(s[i]); */
    unsigned char c = (unsigned char)(s[i]);
    if (c <= 0x7f)
      codepoint = c;
    else if (c <= 0xbf)
      codepoint = (codepoint << 6) | (c & 0x3f);
    else if (c <= 0xdf)
      codepoint = c & 0x1f;
    else if (c <= 0xef)
      codepoint = c & 0x0f;
    else
      codepoint = c & 0x07;

    i++;
    
    if (((s[i] & 0xc0) != 0x80) && (codepoint <= 0x10ffff)) {
      if (codepoint <= 255) {
	/* tempBuf[j++] = static_cast<char>(codepoint); */
	tempBuf[j++] = (char)(codepoint);
      } else {
	// do whatever you want for out-of-bounds characters
	tempBuf[j++] = 'X'; // Store something away so we can convert back later? TODO: Jonas
      }
    }
  }
  tempBuf[j] = '\0';
  return tempBuf;
}

 // returns true if this string seems to contain no text
int looksEmpty(const char *s) {
  if(s) {
    int i = 0;
    for(; s[i]; i++) {
      if(isprint(s[i]) && !isspace(s[i])) {
	return 0;
      }
    }
  }
  
  return 1;
}
