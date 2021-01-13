/* rule.hh
 * authors: Viggo Kann and Johan Carlberger
 * last Viggo change: 2000-03-30
 * last Johan change: 2000-04-25
 * comments: Rule and RuleTerm classes
 */

#ifndef _rule_hh
#define _rule_hh

#include "timer.h"
#include "sentence.h"
#include "element.h"

class Rule;
class GbgRule;
class HelpRule;
class MatchingSet;
class Matching;
class IdEntry;
class GotoEntry;
class Expr;

#include "sentence.h"
#include "taglexicon.h"

#define MAXNOOFELEMENTS 15 /* max allowed number of elements in a rule */

class Category {
  friend class RuleSet;
public:
  const char *Name() const { return name; }
  const char *Info() const { return info; }
  const char *LinkURL() const { return linkURL; }
  const char *LinkText() const { return linkText; }
private:
  Category() : active(true), name(0), info(0), linkURL(0), linkText(0) {}
  bool active;
  const char *name;
  const char *info;
  const char *linkURL;
  const char *linkText;
};

class RuleTerm {
  friend class RuleSet;
  friend class Rule;
  friend class GbgRule;
  friend class HelpRule;
public:
  RuleTerm(Element *elements, int noOfElements, int leftContext = -1, int rightContext = -1);
  void SetMark(Expr *);
  void SetCorr(Expr *);
  void SetJump(GotoEntry *);
  void SetInfo(Expr *);
  void SetAction(Expr *);
  void SetDetect(const char *);
  void SetAccept(const char *);
  void SetLink(const char *, const char *);
  void ResolveHelpRules();
  int MinScope() const { return minScope; }
  int MinScopeWithContext() const { return minScopeWithContext; }
  int ComputeScope();
  int ComputeLeftContextLength(); // computes leftContextLength
  void Print(std::ostream &out = std::cout) const;
  const char *Header() const;
  bool IsJumped(int pos) const;
  void TryMatching(AbstractSentence *s, int wordi, int wordLeft);
  RuleTerm *Next() const { return next; }
  void SetNext(RuleTerm *r) { next = r; }
  Category *GetCategory() const { return category; }
  Rule *GetRule() const { return rule; }
  int Number() const { return number; }
  int GetIndex() const { return index; }
  bool IsHelp() const { return isHelp; }
  bool IsAccepting() const { return isAccepting; }
  bool IsScrutinizing() const { return isScrutinizing; }
  int NElements() const { return nElements; }
  int EndLeftContext() const { return endLeftContext; }
  int BeginRightContext() const { return beginRightContext; }
  bool HasRightContext() const { return (beginRightContext != nElements); }
  int LeftContextLength() const { return leftContextLength; }
  bool EvaluateCorr(Matching*, Expr*) const;
  bool EvaluateAction(Matching*) const;
  const char *EvaluateInfo(Matching*) const;
  bool EvaluateMark(Matching*) const;
  static MatchingSet &GetMatchingSet() { return matchingSet; }
  static bool SaveMatchingOptimization(FILE *fp);
  static bool ReadMatchingOptimization(FILE *fp);
  Element &GetElement(int n) const { return elements[n]; }
  GotoEntry *GetJump() const { return jump; }
  Expr *GetCorr() const { return corr; }
  Expr *GetMark() const { return mark; }
  Expr *GetInfo() const { return info; }
  Expr *GetAction() const { return action; }
  const char *GetDetect() const { return detect; }
  const char *GetAccept() const { return accept; }
  const char *GetLinkURL() const {
    if (!linkURL && GetCategory())
      return GetCategory()->LinkURL();
    return linkURL; }
  const char *GetLinkText() const {
    if (!linkText && GetCategory())
      return GetCategory()->LinkText();
    return linkText; }
  bool ExactScope() const { return exactScope; } // ComputeScope has to be run first
  int TokensBefore(int index); // counts the (minimal) number of tokens before Element index
  void OptimizeRuleMatching();
  bool OptimizeRuleMatchingExtra(double probLevel, 
				 bool skipExactScope,
				 double &bestProb,
				 bool &minOnlyWords,
				 bool possibleTagPair[MAX_TAGS][MAX_TAGS]);
  int anchorTokens; // where the anchor of the ruleterm is placed (in tokens)
  static int nTags; // Number of tags, set by scrutinizer.cc
  static const TagLexicon *tagLexicon; // Current TagLexicon, set by scrutinizer.cc
  static RuleTerm **matchingCheck[MAX_TAGS][MAX_TAGS];
  static int matchingCheckN[MAX_TAGS][MAX_TAGS];
  static void CreateMatchingCheck();
  static void FindMatchingsOptimized(AbstractSentence *s);
private:
  static int matchingCheckAllocatedN[MAX_TAGS][MAX_TAGS];
  static MatchingSet matchingSet;
  static RuleTerm **ruleterms;
  static int ruletermN;
  static int ruletermAllocated; // allocated size of ruleterms[]
  void TryMatchingHelp(Matching &m, const int wordi, const int wordLeft, const int ei);
  void AddMatchingCheck(int tag1, int tag2); // Check this rule term on tags (tag1,tag2)
  void AddMatchingCheck(int tag); // Check this rule term on tag
  void NewRuleTerm(); // Add this RuleTerm to ruleterms[]
  Category *category;
  Expr *mark;
  Expr *corr;
  Expr *info;
  Expr *action;
  const char *detect;
  const char *accept;
  const char *linkURL;
  const char *linkText;
  Element *elements;
  int nElements;
  int minScope; /* minimum no of words in a matching of the rule */
  int minScopeWithContext;
  bool exactScope;
  GotoEntry *jump;
  Rule *rule;
  RuleTerm *next;
  bool isHelp;
  bool isAccepting;
  bool isScrutinizing;
  int number; // number of the RuleTerm in the corresponding Rule
  int index; // index in ruleterms[]
  int endLeftContext; // no of first element after left context
  int beginRightContext; // no of first element in right context
  int leftContextLength; // word tokens in left context
  const Word **savedWords;
  const Word **bestWords;
  Expr **savedExpr;
  Expr ***savedExprPtr;
  int savedWordsN;
  int bestWordsN;
  bool ComputePossibleTagPairs(bool possibleTagPair[MAX_TAGS][MAX_TAGS],
			       int *occurrences, int anchor, 
			       int wordsToMatch, int elementIndex,
			       bool *firstTags);
  bool ComputePossibleSingleTags(bool *possibleTags,
				 int *occurrences, int elementIndex);
  void ComputePossibleTag(bool possibleTagPair[MAX_TAGS][MAX_TAGS],
			  int *occurrences, int anchor, 
			  bool *firstTags, bool *secondTags,
			  bool **secondTagMatrix);
  void CopyToBestWords();
  Word **BestWords(int &n);
  void AddSavedToBestWords(HelpRule *r);
  void AddBestToBestWords(HelpRule *r);
  void PrintBestWords();
  void AddBestWordsAndRuleTerm();
  void ZeroSavedWords();
  Word **CopySavedWords(int &N);
  bool AddWord(Expr **p, const Word *s);
  bool AddHelpRuleWords(HelpRule *r);
  bool CopyHelpRuleBestWords(HelpRule *r);
  void RestoreExpressions();
  bool ExtractWords(Expr **p);
public:
  static Timer::type prepTime;
};

class Rule {
  friend class RuleSet;
  friend class Scrutinizer;
public:
  virtual ~Rule() {};
  Rule(IdEntry *id_, RuleTerm *first);
  const IdEntry *Id() const { return id; }
  int Number() const { return number; }
  const char* Name() const;
  const char *CategoryName() const {
    if (Name() && strchr(Name(), '@'))
      return strchr(Name(), '@')+1;
    return NULL;}
  virtual void ResolveHelpRules();
  int MinScope() const { return minScope; }
  int MinScopeWithContext() const { return minScopeWithContext; }
  bool ExactScope() const { return exactScope; } // ComputeScope has to be run first
  virtual int ComputeScope();
  virtual void Print(std::ostream& = std::cout) const;
  virtual void FindMatchings(AbstractSentence *s);
  virtual void OptimizeRuleMatching();
  void TryMatching(AbstractSentence *s, int wordi, int wordLeft);
  const char *Header() const;
  virtual const char *Type() const { return "§"; }
  RuleTerm *FirstRuleTerm() const { return firstRuleTerm; }
  virtual HelpRule *ThisIfHelpRule() { return NULL; }
  virtual bool IsHelpRule() const { return false; }
  virtual void ResetTried(int) {}
  int NTerms() const { return nTerms; }
  bool IsIgnored() const { return ignored; }
protected:
  int number;
  int nTerms;
  IdEntry *id;
  int minScope; /* minimum no of all rule terms */
  int minScopeWithContext;
  bool exactScope;
  RuleTerm *firstRuleTerm;
  bool ignored;
public:
  Timer::type evaluationTime;
};

class HelpRule : public Rule {
  friend class RuleTerm;
public:
  HelpRule(IdEntry *id_, RuleTerm *first) :
    Rule(id_, first),
    firstWordsOnly(false),
    firstWords(0),
    firstWordsN(0),
    bestWordsOnly(false),
    bestProb(0),
    bestWords(0),
    bestWordsN(0),
    firstTags(0),
    firstTagsMatrix(0),
    bestTagsMatrix(0)
  { }
  HelpRule *ThisIfHelpRule() { return this; }
  bool IsHelpRule() const { return true; }
  bool HasTried(int pos) const { return ms[pos] != NULL; }
  void SetTried(int pos) { if (!ms[pos]) ms[pos] = &noMatch; }
  void ResetTried(int len) { for(int i=1; i<=len; i++) ms[i] = NULL; }
  Matching *FirstMatching(int pos) const {
    if (ms[pos] && ms[pos] != &noMatch)
      return ms[pos];
    return NULL;
  }
  const char *Type() const { return "§h"; }
  void SaveMatching(Matching*);
  bool *FirstTags();
  bool **FirstTagsMatrix();
  bool **BestTagsMatrix();
private:
  bool firstWordsOnly;
  Word **firstWords;
  int firstWordsN;
  bool bestWordsOnly;
  double bestProb; // probability of bestWords and bestTagsMatrix
  Word **bestWords;
  int bestWordsN;
  bool *firstTags;
  bool **firstTagsMatrix;
  bool **bestTagsMatrix;
  void ComputeFirstTagsMatrix();
  void ComputeBestTagsMatrix(bool possibleTagPair[MAX_TAGS][MAX_TAGS]);
  void ComputeBestProb();
  void AllocateBestWords();
  static Matching noMatch;
  Matching *ms[MAX_SENTENCE_LENGTH]; // matchings for current sentence
};

class GbgRule : public Rule {
public:
  GbgRule(IdEntry *id, RuleTerm *posTerm, RuleTerm *negTerm, Expr *info_);
  void ResolveHelpRules();
  void OptimizeRuleMatching();
  int ComputeScope();
  const char *Type() const { return "GbgRule"; }
  void FindMatchings(AbstractSentence *s);
  void Print(std::ostream &out) const;
  const RuleTerm *NegRuleTerm() const { return negRuleTerm; }
private:
  RuleTerm *negRuleTerm;
  Expr *info;
};

class RegExpRule : public Rule {
protected:
  Element *element;
  const char *regexp;
  Expr *mark;
  const char *corr;
  GotoEntry *jump;
  Expr *info, *action;
  const char *detect, *accept;
public:
  RegExpRule(IdEntry *id, RuleTerm *term) :
    Rule(id, term) {
  }
  RegExpRule(Element *el, const char *regexp_, IdEntry *id, Expr *mark_, 
	     const char *corr_, GotoEntry *jump_, Expr *info_, Expr *action_,
	     const char *detect_, const char *accept_);
  int ComputeScope();
  const char *Type() const { return "RegExpRule"; }
};

inline std::ostream& operator<<(std::ostream& os, const Rule &r) {
  os << r.Name(); return os;
}
inline std::ostream& operator<<(std::ostream& os, const Rule *r) {
  if (r) os << *r; else os << "(null Rule)"; return os;
}
inline std::ostream& operator<<(std::ostream& os, const RuleTerm &r) {
  os << r.Header(); return os;
}
inline std::ostream& operator<<(std::ostream& os, const RuleTerm *r) {
  if (r) os << *r; else os << "(null RuleTerm)"; return os;
}

#endif
