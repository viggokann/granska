/* lexicon.hh
 * author: Johan Carlberger
 * last change: 2000-02-15
 * comments: Lexicon class
 */

#ifndef _lexicon_hh
#define _lexicon_hh

#include "basics.h"
#include "hashtable.h"
#include "morflexicon.h"
#include "newwordlexicon.h"
#include "taglexicon.h"
#include "newword.h"
#include "wordlexicon.h"
#include "wordruleterms.h"

class Tagger;
class TrigramGadget;
class WordSuggestions;
class WordToken;
class RuleTerm;

class Lexicon {
  friend class Word;
public:
  Lexicon();
  virtual ~Lexicon();
  const char *LexiconDir() const { return lexiconDir; }
  bool IsLoaded() const { return tags.IsLoaded() && words.IsLoaded() && morfs.IsLoaded(); }
  bool LoadWordsAndMorfs(Tagger*, const char *dir = NULL);
  const MorfLexicon& Morfs() const { return morfs; }
  const NewWordLexicon& NewWords() const { return newWords; }
  const WordLexicon& Words() const { return words; }
  const TagLexicon& Tags() const { return tags; }
  TagLexicon& Tags() { return tags; }

  Word *FindMainWord(const char*);          // string can contain uppers
  Word *FindMainOrNewWord(const char*);          // string can contain uppers
  Word *FindMainOrNewWordAndAddIfNotPresent(const char*); // string can contain uppers
  Word *FindMainOrNewWordNoCaps(const char*) const;   // string must be all lowers
  NewWord *AddNewWord(const char* s) { return newWords.AddWord(s); }
  bool AddWordRuleTerm(Word*, const RuleTerm*);
  const WordRuleTerms *FindWordRuleTerms(const Word *w);
protected:
  NewWordLexicon& NewWords() { return newWords; }
  MorfLexicon& Morfs() { return morfs; }
  bool LoadTags(const char* dir);
  void SetMorfProbs();
private:
  void LoadWords(const char* dir, Tagger*);
  void LoadMorfs(const char* dir);
  void SetWordProbs(Tagger*);
  WordLexicon words;
  MorfLexicon morfs;
  TagLexicon tags;
  NewWordLexicon newWords;
  const char *lexiconDir;

  HashTable<WordRuleTerms> *wordRuleTerms;
};

#endif




