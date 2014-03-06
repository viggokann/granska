/* tagger.hh
 * author: Johan Carlberger
 * last change: 2000-05-05
 * comments: Tagger class
 */

#ifndef _tagger_hh
#define _tagger_hh

#include "basics.h"
#include "lexicon.h"
#include "sentence.h"
#include "timer.h"
#include "text.h"
#include "tokenizer.h"
#include "trigramgadget.h"

class TrigramGadget;
class WordSuggestions;
class WordToken;

class Tagger : public Lexicon {
  friend class Lexicon;
public:
  Tagger();
  virtual ~Tagger();
  bool Load(const char *dir = NULL);
  void SetStream(std::istream *instream, int size_hint = -1);
  void TagText();
  void ReadText();
  void PrintNewWords() const;
  void PrintParameters() const;
  Text &GetText() { return theText; }
  const Text &GetText() const { return theText; }
  void TagSentence(AbstractSentence *s) { TagSentenceInterval(s, 0, s->NTokens()-1); }
  void TagSentenceInterval(AbstractSentence*, int startPos, int endPos);
  void GenerateInflections();

protected:
  void TagUnknownWord(Word*, bool normalize = false,
		      bool suffixCheck = false, const WordToken* = NULL);
  Tokenizer tokenizer;
  Text theText;

 protected: // jonas
  void Reset();
  void BuildSentences(WordToken*);
 private:
  void AddTagsToUnknownWord(NewWord*);
  void Rewind(AbstractSentence*, int nSteps);
  void SetLexicalProbs(Word*, const WordToken&, TrigramGadget*);
  TrigramGadget gadget[MAX_SENTENCE_LENGTH];
 protected: // jonas
  std::istream *inputStream;
 private:
  Tag* contentTags[MAX_TAGS];
  int CT_CONTENT;
  bool xSetNewWordTagFreqs;
  WordToken specialWordToken[N_TOKENS];
 protected: // jonas
  int input_size; // jonas
  Word *specialWord[N_TOKENS];
  WordToken *theTokens;
  int tokensBufSize;
  int nTokens;
  std::ostringstream theOriginalText; //Oscar
public:
  void PrintTimes();
  Timer::type loadTime;
  Timer::type tokenizeTime;
  Timer::type sentenceTime;
  Timer::type tagTime;
  Timer::type analyzeTime;

  void ReadTaggedTextQnD(); // ugly hack to read pre-tagged text

};

inline void Tagger::SetStream(std::istream *instream, int size_hint) {
  inputStream = instream;
  tokenizer.SetStream(inputStream);
  input_size = size_hint;
}

#endif
