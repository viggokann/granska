/* inflectlexicon.cc
 * author: Johan Carlberger, based on work by Johan Bertenstam
 * last change: 990906
 * InflectLexicon class
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

#include "file.h"
#include "inflectlexicon.h"
#include "letter.h"
#include "message.h"
#include "settings.h"
#include "taglexicon.h"
#include <limits.h>

uint InflectLexicon::FindRuleIndex(const char *string, const char *rule, uchar baseFormTagIndex) {
  for (uint i=0; i<nRules; i++)
    if (!strcmp(rule, rules[i].Name()) && rules[i].IsApplicable(string, baseFormTagIndex)) {
      rules[i].used = true;
      return i;
    }
  return INFLECT_NO_RULE;
}

const char *InflectLexicon::GetInflectedForm(const char *string, uint ruleIndex, uchar tagIndex) const {
  const InflectRule &rule = rules[ruleIndex];
  for (int k=0; k<rule.NForms(); k++)
    if (rule.tagIndex[k] == tagIndex)
      return rule.Apply(string, k);
  return NULL;
}

bool InflectLexicon::Save(std::ofstream &out) const {
  Message(MSG_STATUS, "saving fast inflect lexicon...");
  WriteData(out, (char*)this, sizeof(InflectLexicon), "inflects");
  WriteData(out, (char*)InflectRule::strings, ruleStringLen, "ruleString");
  for (uint i=0; i<nRules; i++) {
    WriteData(out, (char*) &rules[i], sizeof(InflectRule));
    if (!rules[i].used)
      Message(MSG_MINOR_WARNING, "inflection rule", rules[i].Name(), rules[i].Ending(0), "is not used");
  }
  return true;
}

bool InflectLexicon::LoadFast(std::ifstream &in) {
  Message(MSG_STATUS, "loading inflect lexicon fast...");
  ReadData(in, (char*)this, sizeof(InflectLexicon), "inflects");
  InflectRule::strings = new char[ruleStringLen];
  ReadData(in, (char*)InflectRule::strings, ruleStringLen, "ruleString");
  for (uint i=0; i<nRules; i++)
    ReadData(in, (char*) &rules[i], sizeof(InflectRule));
  return isLoaded = true;
}

short InflectLexicon::AddString(const char *s) {
  for (short i=0; i<ruleStringLen; i++)
    if (!strcmp(InflectRule::strings+i, s))
      return i;
  int len = strlen(s)+1;
  ensure(ruleStringLen+len < SHRT_MAX);
  strcpy(InflectRule::strings+ruleStringLen, s);
  ruleStringLen += len;
  return (short) (ruleStringLen - len);
}

void InflectLexicon::LoadSlow(const char *dir, const TagLexicon *tags) {
  lexiconDir = dir;
  Message(MSG_STATUS, "loading inflect lexicon slow...");
  std::ifstream in;
  if (!FixIfstream(in, dir, "inflection.rules", true))
    return;
  InflectRule::strings = new char[MAX_INFLECTION_RULES*40];
  for (nRules=0; nRules<MAX_INFLECTION_RULES; nRules++) {
    while (isspace(in.peek())) in.get();
    if (in.peek() == EOF)
      break;
    InflectRule &r = rules[nRules];
    if (in.peek() == '$') {
      in.get();
      Tag t;
      int j=0;
      while (in.peek() != '\n') {
	ensure(j < MAX_INFLECTION_FORMS);
	in >> t.string;
	if (strcmp(t.string, "-")) {
	  Tag *t2 = tags->Find(t);
	  if (!t2)
	    Message(MSG_WARNING, "unknown tag in inflection.rules:", t.String());//jonas 	    Message(MSG_ERROR, "unknown tag in inflection.rules:", t.String());
	  else {
	    r.tagIndex[j] = t2->Index();
	    if (j == 0)
	      t2->ruleBase = true;
	  }
	} else
	  r.tagIndex[j] = TAG_INDEX_NONE;
	j++;
	SkipSpaceButNotNewLine(in);
      }
      for (; j<MAX_INFLECTION_FORMS; j++)
	r.tagIndex[j] = TAG_INDEX_NONE;
    } else {
      if (nRules == 0)
	Message(MSG_ERROR, "inflection.rules must start with $ followed by some tags");
      for (int j=0; j<MAX_INFLECTION_FORMS; j++)
	r.tagIndex[j] = rules[nRules-1].tagIndex[j];
    }
    char s[100];
    in >> s;
    r.nameIndex = AddString(s);
    in >> s;
    r.nEndings=0;
    for (char* ss = strtok(s, ","); ss; ss = strtok(NULL, ",")) {
      if (r.nEndings >= MAX_INFLECTION_ENDINGS)
	Message(MSG_ERROR, "too many ending-alternatives for rule", r.Name());
      r.endingIndex[r.nEndings++] = AddString(ss);
    }
    in >> s;
    if (s[0] == '=')
      r.nCharsToRemove = 0;
    else      
      r.nCharsToRemove = strlen(s);
    r.nForms = 0;
    for (r.nForms=0; in.peek() != '\n' && in.peek() != EOF; r.nForms++) {
      ensure(r.nForms < MAX_INFLECTION_FORMS);
      in >> s;
      if (*s == '-')
	r.formIndex[r.nForms] = INFLECTION_FORM_NONE;
      else {
	if (*s == '=')
	  *s = '\0';
	r.formIndex[r.nForms] = AddString(s);
      }
      SkipSpaceButNotNewLine(in);
    }    
    for (int k=r.nForms; k<MAX_INFLECTION_FORMS; k++)
      r.formIndex[k] = INFLECTION_FORM_NONE;
    r.nForms++;
  }
  while (isspace(in.peek())) in.get();
  if (in.peek() != EOF)
    Message(MSG_WARNING, "number of rules in inflection.rules exceeds", int2str(MAX_INFLECTION_RULES));
  isLoaded = true;
}
