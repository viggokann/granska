/* inflectrule.cc
 * author: Johan Carlberger, based on work by Johan Bertenstam
 * last change: 990909
 * comments: derives inflected forms from any Word
 * which has an applicable inflection rule.
 */

#include "letter.h"
#include "inflectrule.h"
#include "tag.h"
#include <string.h>

char *InflectRule::strings;

int max(int a, int b) { return (a<b) ? b : a;}

const char *InflectRule::Apply(const char *string, int form) const {
  if (form == 0)
    return string;
  ensure(form < nForms);
  const int q = formIndex[form-1];
  if (q == INFLECTION_FORM_NONE)
    return NULL;
  static char s[100];
  const int stemLength = max(0, strlen(string)-nCharsToRemove);
  strncpy(s, string, stemLength);
  strcpy(s+stemLength, Form(q)); 
  return s;
}

bool InflectRule::IsApplicable(const char *string) const {
  for (int i=0; i<nEndings; i++) {
    const int n = strlen(Ending(i));
    const char *ending, *stringEnd;
    if (Ending(i)[0] == '^') {
      ending = Ending(i) + 1;
      stringEnd = string;
    }
    else
    {
	ending = Ending(i);
#if 0   /* jbfix: len - n may be < 0! */
	stringEnd = string + strlen(string) - n;
#else
	int index = strlen(string) - n;
	if(index < 0)
	    index = 0;
	stringEnd = string + index;
#endif
    }
    if (!strncmp(stringEnd, ending, n) || 
	(ending[0] == 'V' &&
	 IsVowel(stringEnd[0]) &&
	 !strncmp(stringEnd+1, ending+1, n-1)) ||
	(ending[0] == 'C' &&
	 IsConsonant(stringEnd[0]) &&
	 !strncmp(stringEnd+1, ending+1, n-1)))
      return true;
  }
  return false;
}

bool InflectRule::IsApplicable(const char *string, uchar baseFormTagIndex) const {
  return (baseFormTagIndex == tagIndex[0] || baseFormTagIndex == TAG_INDEX_NONE)
    && IsApplicable(string);
}
    

