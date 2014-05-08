/* wordlexicon.hh
 * author: Johan Carlberger
 * last change: 2000-03-23
 * comments: WordLexicon class
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

#ifndef _wordlexicon_hh
#define _wordlexicon_hh

#include "word.h"
#include "newword.h"
#include "hasharray.h"
#include "inflectlexicon.h"
#include "taglexicon.h"
#include "styleword.h"

const int MAX_N_EXTRA_RULES = 300;
const int MAX_N_EXTRA_LEMMAS = 10000;

class NewWordLexicon;

class ExtraRules {
public:
  ExtraRules() : wt(NULL) {
    for (int i=0; i<MAX_INFLECTION_RULES_PER_WORDTAG-1; i++)
      rule[i] = INFLECT_NO_RULE;
  }
  WordTag *wt;
  ushort rule[MAX_INFLECTION_RULES_PER_WORDTAG-1];
};

class ExtraLemma {
public:
  WordTag *wt;
  WordTag *lemma;
};

class WordLexicon : public HashArray<Word> {
  friend class DevelopersTagger;
public:
  WordLexicon() : more(NULL), CW(0), nExtraRules(0), newWords(NULL) { NewObj(); }
  ~WordLexicon();
  bool LoadFast(const char *dir, const TagLexicon*, NewWordLexicon*, bool warn = true);
  void LoadSlow(const char *dir, const TagLexicon*, NewWordLexicon*);
  bool Save();
  //  Word* Find(const char *s) const { Word w; w.string = (char*) s; return words.Find(w); }
  //  Word* Find2(const char *s) const { Word w; w.string = (char*) s; return Find(w); }
  Word* FindAbbreviation(const char*) const;
  const StyleWord* GetStyleWord(const Word* w) const { return stylewords.Find(StyleWord(w)); }
  int Cwt() const { return CWT; }
  int Cw() const { return CW; }
  uint ExtraInflectRule(const WordTag*, int) const;
  void Print() const;
  void PrintStatistics() const;
  bool IsLoaded() const { return CW > 0; }
  int CompoundAnalyze(NewWord*) const;
  void AnalyzeNewWord(NewWord*, bool tryHard = false) const;
  void GuessWordTagRule(WordTag*) const;
  WordTag* GetInflectedForm(WordTag*, uint ruleIndex, const Tag*) const;
  const InflectLexicon &Inflects() const { return inflects; }
  WordTag* GetExtraLemma(const WordTag*) const;
  Word** WordsAlpha() const { return wordsAlpha; }
  Word** GetWordsInRange(const char *s, int *n) const;
  void AnalyzeNewWords() const; //int from = 0, int to = -1) const; // -1 means analyze all
  void TestInflections() const;
  void AnalyzeWordAndPrintInflections(const char*, const char *newline = "\n") const;
  char *GetInflectionList(const char *string, char *result) const;

  void ServerAnalyzeWordAndPrintInflections(std::iostream &socket, const char *string) const;

#ifdef DEVELOPERS
  void GenerateExtraWordTags() const; // jonas, from obsolete code
#endif
  void GenerateInflections(bool onlyUnknownWordTags = false) const;
private:
  void LoadCompoundLists();
  void LoadForeign();
  void LoadVerbtypes();
  void LoadStyleWords();
  void LoadWordRules();
  void LoadLemmas();
  void CompressStrings();
  void LoadInfo();
  void AllocateMemory();
  void SetPointersFromIndices();
  void AddExtraRule(WordTag*, ushort ruleIndex);
  bool AddTagsDerivedFromLemma(NewWord *w, WordTag *lemma) const;
  const TagLexicon *tags;
  const char *lexiconDir;
  WordTag *more;
  char *strings;
  int CL, CW, CWT, CMW;
  int N_STYLEWORDS;
  HashArray<StyleWord> stylewords;
  StringBuf commentBuf;
  InflectLexicon inflects;
  ExtraRules extraRules[MAX_N_EXTRA_RULES];
  int nExtraRules;
  ExtraLemma extraLemmas[MAX_N_EXTRA_LEMMAS];
  int nExtraLemmas;
  NewWordLexicon *newWords;
  Word **wordsAlpha;

  DecObj();
};

#endif
