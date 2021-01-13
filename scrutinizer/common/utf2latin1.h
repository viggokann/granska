#ifndef UTF2LATIN1
#define UTF2LATIN1

// Assuming the text is Swedish text, this should return 1 if the
// encoding is UTF-8, and it should return 0 if we find something that
// looks like latin-1
int looksLikeUTF(const char *s);

// Converts UTF-8 to Latin-1 correctly if we assume the string
// contents are Swedish text. Based on code example from Mark Ransom
void utf2latin1(char *s);

char *utf2latin1Copy(const char *s); // creates a new string with the result, that the caller needs to free()

int looksEmpty(const char *s); // returns true if this string seems to contain no text

#endif
