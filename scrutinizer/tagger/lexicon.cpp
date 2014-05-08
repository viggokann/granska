/* lexicon.cc
 * author: Johan Carlberger
 * last change: 2000-05-08
 * comments: Lexicon class
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

#include <float.h>
#include <math.h>
#include "file.h"
#include "letter.h"
#include "message.h"
#include "morf.h"
#include "sentence.h"
#include "settings.h"
#include "lexicon.h"
#include "tagger.h"

DefObj(HashTable<WordRuleTerms>);
DefObj(RuleTermList);

Lexicon::Lexicon() {
  wordRuleTerms = new HashTable<WordRuleTerms>(KeyWordRuleTerms, CompareWordRuleTerms,
					       NULL, NULL);
}

Lexicon::~Lexicon() {
  Message(MSG_STATUS, "deleting lexicon...");
  wordRuleTerms->DeleteAndClear();
  delete wordRuleTerms;
}

const WordRuleTerms *Lexicon::FindWordRuleTerms(const Word *w) {
  if (!w->IsRuleAnchor())
    return NULL;
  WordRuleTerms wrt((Word*)w);
  return wordRuleTerms->Find(&wrt);
}

bool Lexicon::AddWordRuleTerm(Word *w, const RuleTerm *r) {
  bool check = true;
  WordRuleTerms *wrt= (WordRuleTerms*) FindWordRuleTerms(w);
  //  std::cout << "add: " << w << ' ' << r << ' ' << wrt << std::endl;
  if (wrt) {
    for (const RuleTermList *l = wrt; l; l = l->Next()) {
      if (l->GetRuleTerm() == r) {
	Message(MSG_WARNING,
		"AddWordRuleTerm() detected something Viggo promised would never happen, word = ",
		w->String());
	check = false;
	return false;
      }
      ensure(l->Next() != wrt);
    }
    wrt->AddRuleTerm(r);
    return check;
  }
  wrt = new WordRuleTerms(w, r); // new OK
  wordRuleTerms->Insert(wrt);
  return check;
}

void Lexicon::SetWordProbs(Tagger *tagger) {
  for (int i=0; i<words.Cw(); i++) {
    if (words[i].HasExtraWordTag())
      tagger->TagUnknownWord(&words[i], true, false);
    words[i].ComputeLexProbs();
  }
}

void Lexicon::SetMorfProbs() {
  static float prevAlpha[10] = {0,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  ensure(morfs.IsLoaded());
  int i;
  for (i=0; i<10; i++)
    if (prevAlpha[i] != xAlphaLastChar[i])
      break;
  if (i != 10)
    for (i=0; i<morfs.Cw(); i++) {
      uint len = morfs[i].StringLen();
      if (prevAlpha[len] != xAlphaLastChar[len]) {
	for (WordTag *w = &morfs[i]; w; w = w->Next()) {
	  w->lexProb = xAlphaLastChar[len]*w->TagFreq()/morfs[i].Freq();
	  ensure(w->lexProb > 0);
	}
      }
    }
  for (i=0; i<10; i++)
    prevAlpha[i] = xAlphaLastChar[i];
}

void Lexicon::LoadWords(const char* dir, Tagger* tagger) {
  bool fastOK = words.LoadFast(dir, &tags, &newWords, false);
  if (!fastOK) {
    words.LoadSlow(dir, &tags, &newWords);
    SetWordProbs(tagger);
  }

  if (!fastOK)
    words.Save();
}

void Lexicon::LoadMorfs(const char* dir) {
  bool fastOK = morfs.LoadFast(dir, false);
  if (!fastOK) {
    morfs.LoadSlow(dir, tags);
    SetMorfProbs();
  }
 
  if (!fastOK)
    morfs.Save();
}

bool Lexicon::LoadTags(const char* dir) {
  Message(MSG_STATUS, "loading tags...");
  ensure(!tags.IsLoaded());
  char wDir[MAX_FILE_NAME_LENGTH];
  AddFileName(wDir, dir, "tags");
  if (!tags.LoadFast(wDir, false)) {
    tags.LoadSlow(wDir);
    tags.Save();
  }
  return tags.IsLoaded();
}

bool Lexicon::LoadWordsAndMorfs(Tagger *tagger, const char *dir) {
  ensure(tags.IsLoaded());
  ensure(!morfs.IsLoaded());
  ensure(!words.IsLoaded());
  lexiconDir = dir;
  char wDir[MAX_FILE_NAME_LENGTH];
  Message(MSG_STATUS, "loading morfs...");
  LoadMorfs(AddFileName(wDir, lexiconDir, "morfs"));
  if (!morfs.IsLoaded())
    Message(MSG_ERROR, "cannot load morf lexicon");
  Message(MSG_STATUS, "loading words...");
  LoadWords(AddFileName(wDir, lexiconDir, "words"), tagger);
  if (!words.IsLoaded())
    Message(MSG_ERROR, "cannot load word lexicon");
  return true;
}

Word *Lexicon::FindMainWord(const char *s) {
  char s2[MAX_WORD_LENGTH];
  strcpy(s2, s);
  ToLower(s2);
  return words.Find(s2);
}

Word *Lexicon::FindMainOrNewWordNoCaps(const char *s) const {
  Word *w = words.Find(s);
  return w ? w : newWords.Find(s);
}

Word *Lexicon::FindMainOrNewWord(const char *s) {
  Word *w = FindMainOrNewWordNoCaps(s);
  if (w) return w;
  char s2[MAX_WORD_LENGTH];
  strcpy(s2, s);
  ToLower(s2);
  return FindMainOrNewWordNoCaps(s2);
}

Word *Lexicon::FindMainOrNewWordAndAddIfNotPresent(const char *s) {
  Word *w = FindMainOrNewWordNoCaps(s);
  if (w) return w;
  char s2[MAX_WORD_LENGTH];
  strcpy(s2, s);
  ToLower(s2);
  w = FindMainOrNewWordNoCaps(s2);
  if (w) return w;
  NewWord *nw = newWords.AddWord(s2);
  Words().AnalyzeNewWord(nw);
  return nw;
}


