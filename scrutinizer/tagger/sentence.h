/* sentence.hh
 * author: Johan Carlberger
 * last change: 2000-04-27
 * comments: Sentence class
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

#ifndef _sentence_hh
#define _sentence_hh

#include "wordtoken.h"

// these should be in Sentence:
const int MAX_SENTENCE_LENGTH = 150;      // can be set arbitrarily

class Sentence;
class Tagger;
class GramError;

class AbstractSentence {
  friend class Scrutinizer;
  friend class Tagger;
  friend class MatchingSet;
public:
  AbstractSentence() : nWords(0), nTokens(0), tokens(NULL), prob(0),
		       endsParagraph(0), containsStyleWord(0),
		       containsRepeatedWords(0), containsNewWord(0), isHeading(0),
		       seemsForeign(0), bitsSet(0), nGramErrors(0), gramError(NULL) {}
  void Print(std::ostream& = std::cout) const;
  void Print(int from, int to, std::ostream& = std::cout) const;
  int NWords() const { return nWords; }
  int NTokens() const { return nTokens; }
  Word* GetWord(int n) const { return tokens[n]->GetWord(); }
  WordToken* GetWordToken(int n) const { return tokens[n]; }
  WordToken** GetWordTokensAt(int n) const { return tokens + n; }
  int TextPos() const { return tokens[2]->Offset(); }
  probType Prob() const { return prob; }
  void FixUp();
  void TagMe();
  bool IsEqual(const AbstractSentence*) const;
  void SetContentBits();
  bool ContainsNewWord() const { ensure(bitsSet); return containsNewWord; }
  bool ContainsStyleWord() const {  ensure(bitsSet); return containsStyleWord; }
  bool ContainsRepeatedWords() const {  ensure(bitsSet); return containsRepeatedWords; }
  bool SeemsForeign() const {  ensure(bitsSet); return seemsForeign; }
  bool IsHeading() const { return isHeading; }
  bool EndsParagraph() const { return endsParagraph; }
  const GramError *GetGramError() const { return gramError; } 
  int NGramErrors() const { return nGramErrors; }
  void setOriginalText(std::string org) {
      //if(originalText)
      //    delete originalText;
      originalText = org;
  }
  std::string getOriginalText() const {return originalText;}
 protected:
  std::string originalText; //Oscar
  static Tagger *tagger;
  short nWords;                   // including punctuation marks
  short nTokens;                  // including 4 sentence-delimiters
  // jonas, both actually count the same thing
  WordToken **tokens;
  probType prob;
  Bit endsParagraph : 1;
  Bit containsStyleWord : 1;
  Bit containsRepeatedWords : 1;
  Bit containsNewWord : 1;
  Bit isHeading : 1;
  Bit seemsForeign : 1;
  Bit bitsSet : 1;
  char nGramErrors;
  GramError *gramError;
};

enum SentenceStatus {
  SEEMS_OK,
  SAME_AS_ORIGINAL,
  SAME_AS_ANOTHER,
  FORM_NOT_FOUND,
  NOT_IMPROVING
};

class DynamicSentence : public AbstractSentence {
  friend class Matching;
  friend class GramError;
  friend class Expr;
  friend class Scrutinizer;
public:
  DynamicSentence() : next(0), size(0), actualTokens(0) { NewObj(); } 
  DynamicSentence(const AbstractSentence*);
  ~DynamicSentence();
  void Delete(int from, int to);
  bool Delete(int orgPos);
  void Insert(int pos, Word*, const char *string);
  bool Insert2(int orgPos, Word*, const char *string);
  bool Replace(Word*, const char *string, int orgPos);
  DynamicSentence *Next() const { return next; }
  DynamicSentence *SetNext(DynamicSentence *s) { return next=s; }
  SentenceStatus Status() const { return status; }
  void PrintOrgRange(int from, int to, std::ostream& = std::cout) const;
private:
  DynamicSentence *next;
  short size;
  SentenceStatus status;
  WordToken *actualTokens;
  DecObj();
};

class Sentence : public AbstractSentence {
  friend class Tagger;
  friend class Text;
public:
  Sentence *Next() const { return next; }
private:
  Sentence(int n) { ensure(n < MAX_SENTENCE_LENGTH); tokens = new WordToken*[n]; NewObj(); }
  ~Sentence();
  Sentence *next;
  DecObj();
};

inline std::ostream& operator<<(std::ostream& os, const Sentence &s) {
  os << &s; return os;
}
inline std::ostream& operator<<(std::ostream& os, const AbstractSentence *s) {
  if (s) s->Print(os); else os << "(null Sentence)"; return os;
}

#endif




