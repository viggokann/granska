/* scrutinizer.hh
 * author: Johan Carlberger
 * last change: 2000-03-23
 * comments: Scrutinizer class
 *           A Scrutinizer scrutinizes a text and creates a linked list of GramErrors
 *           containing information to get suggestions for spelling and grammar
 *           errors found in the text.
 *           Only public methods should be used by user interface.
 *           Don't cast returned const objects into non-consts unless
 *           you know what you're doing.
 *           Private methods are used by scrutinizer internally.
 */

#ifndef _scrutinizer_hh
#define _scrutinizer_hh

#include "matching.h"
#include "matchingset.h"
#include "rule.h"
#include "rules.h"
#include "rulesettings.h"
#include "gramerror.h"
#include "tagger.h"
#include "prob.h"

class RuleSet;
class Output;

class Scrutinizer : public Tagger {
public:
  // methods for user interface:
  Scrutinizer(OutputMode = OUTPUT_MODE_XML);
  // see settings.hh and *.cc for OutputMode options

  ~Scrutinizer();

  bool Load(const char* taggerLexDir = NULL, const char *ruleFile = NULL);
  // if dirs == NULL, default lexicons and rules are used
  // returns true when everything was loaded OK

  const Text *ReadTextFromFile(const char *fileName);
  // reads and tags a text from file

  const Text *ReadTextFromString(char *string);
  // reads and tags a text from string
  // the string may be changed, depends on how istringstream works

  const Text *ReadTextFromStream(std::istream*);
  // reads and tags a text from stream

  GramError **Scrutinize(int *n);
  // scrutinizes the current text and returns an array of n GramErrors
  
  std::map<std::string, int> ruleCount;
  //stores occurences of rules used.
  
  // methods not for user interface:
  void Analyze();
  RuleSet *GetRuleSet() const { return ruleSet; }
  bool IsSpellOK(const char *s, Token token);        // used by rules
  char *SpellOK(const char *s, Token token);   // used by rules
  bool IsSoundSpellOK(const char *s, Token token); // used by rules
  void PrintResult(std::ostream& = std::cout);
  char* GetResult();
  void Scrutinize(AbstractSentence*);
  void CheckAcceptAndDetect();
  MatchingSet &GetMatchingSet() const { return RuleTerm::GetMatchingSet(); }
  
private:
  void Scrutinize(char*);
  //std::string fixXML(const char*); //Oscar
  //std::string fixXML(std::string); //Oscar
  RuleSet *ruleSet;
  GramError **gramErrors;
  int nGramErrors;
  int gramErrorBufSize;
  DecObj();
};

void PrintCorrForPos(Scrutinizer &scrutinizer);
// use to test GramError functionality

#endif
