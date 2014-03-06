/* rule.cc
 * author: Viggo Kann and Johan Carlberger
 * last Viggo change: 2000-09-15
 * last Johan change: 2000-05-02
 * comments: Rule class
 */

#include "scrutinizer.h"
#include "rules.h"
#include "ruleset.h"
#include "rule.h"
#include "matching.h"
#include "rulesettings.h"
#include "matchingset.h"
#include "timer.h"
#include "wordtoken.h"

extern Scrutinizer *scrutinizer;       // defined in rules.y

static const int MAX_RULES_TO_CHECK = 100;
static const int MATCHING_CHECK_START_SIZE = 10;
static const int MAX_WORDS_IN_EXPRESSION = 100;
const int NEW_ELEMENTS_BUF_SIZE = 2100;
const static double INFINITY_PROBABILITY = 1000.0;
const static double MUST_CHECK_ALL_FACTOR = 6.0; // how much worse is checking every place in a sentence for matchings

static RuleTerm *helpRuleTermsToCheck[MAX_SENTENCE_LENGTH][MAX_RULES_TO_CHECK];
static int nHelpRuleTermsToCheck[MAX_SENTENCE_LENGTH];
static RuleTerm *helpRuleTermsToCheckEverywhere[MAX_RULES_TO_CHECK];
static int nHelpRuleTermsToCheckEverywhere;

extern RuleSet ruleSet;
Matching HelpRule::noMatch;
MatchingSet RuleTerm::matchingSet;
RuleTerm **RuleTerm::ruleterms = 0;
int RuleTerm::ruletermN = 0;
int RuleTerm::ruletermAllocated = 0;
const TagLexicon *RuleTerm::tagLexicon;
int RuleTerm::nTags;
Timer::type RuleTerm::prepTime = 0;
const int MethodOrFunctionEntry::variablenoofargs = -1; /* possible value of noofargs */

bool *allTags = NULL;

// SavedWordProb gives the probability of savedWords[0..savedWordsN-1]
static double SavedWordProb(const Word *savedWords[], int savedWordsN) {
  long freq = 0;
  if (savedWordsN == 0) return 0.0;
  for (int i = 0; i < savedWordsN; i++) {
    freq += savedWords[i]->Freq();
  }
  return (double) freq / scrutinizer->Words().Cw();
}

// CopyToBestWords copies savedWords to bestWords
void RuleTerm::CopyToBestWords() {
  for (int i = 0; i < savedWordsN; i++) {
    bestWords[i] = savedWords[i];
  }
  bestWordsN = savedWordsN;
}

Word **RuleTerm::BestWords(int &n)
{ n = bestWordsN;
  return (Word **) bestWords;
}

// AddSavedToBestWords adds savedWords to the bestWords in the HelpRule.
void RuleTerm::AddSavedToBestWords(HelpRule *r) {
  int j;
  for (int i = 0; i < savedWordsN; i++) {
    for (j = 0; j < r->bestWordsN; j++)
      if (r->bestWords[j] == savedWords[i]) break;
    if (j == r->bestWordsN) {
      if (bestWordsN+1 == MAX_WORDS_IN_EXPRESSION) {
	Message(MSG_WARNING, "Too many words in the same help rule. The word ", 
		savedWords[i]->String(), "did not fit.");
	return;
      }
      r->bestWords[r->bestWordsN++] = (Word *) savedWords[i];
    }
  }
}

// AddBestToBestWords adds bestWords to the bestWords in the HelpRule.
void RuleTerm::AddBestToBestWords(HelpRule *r) {
  int j;
  for (int i = 0; i < bestWordsN; i++) {
    for (j = 0; j < r->bestWordsN; j++)
      if (r->bestWords[j] == bestWords[i]) break;
    if (j == r->bestWordsN) {
      if (bestWordsN+1 == MAX_WORDS_IN_EXPRESSION) {
	Message(MSG_WARNING, "Too many words in the same help rule. The word ", 
		bestWords[i]->String(), "did not fit.");
	return;
      }
      r->bestWords[r->bestWordsN++] = (Word *) bestWords[i];
    }
  }
}

void RuleTerm::PrintBestWords() {
  if (bestWordsN == 0) return;
  std::cout << tab << "matching word(s): ";
  for (int i = 0; i < bestWordsN; i++)
    std::cout << bestWords[i]->String() << " ";
  std::cout << std::endl;
}

// AddBestWordsAndRuleTerm adds bestWords and the current RuleTerm to the 
// lexicon.
void RuleTerm::AddBestWordsAndRuleTerm() {
  for (int i = 0; i < bestWordsN; i++)
    if (!scrutinizer->AddWordRuleTerm((Word *)(bestWords[i]), 
				      (const RuleTerm *) this))
      if (xPrintOptimization) std::cout << tab << bestWords[i] << " added twice" << std::endl; 
}

// ZeroSavedWords must be called before using savedWords or bestWords.
void RuleTerm::ZeroSavedWords()
{ savedWordsN = 0;
 if (savedWords == NULL) {
   savedWords = new const Word *[MAX_WORDS_IN_EXPRESSION];
   savedExpr = new Expr *[MAX_WORDS_IN_EXPRESSION];
   savedExprPtr = new Expr **[MAX_WORDS_IN_EXPRESSION];
   bestWords = new const Word *[MAX_WORDS_IN_EXPRESSION];
   bestWordsN = 0;
 }
}

// CopySavedWords creates and returns a copy of the savedWords
Word **RuleTerm::CopySavedWords(int &N)
{
  N = savedWordsN;
  Word **wp = new Word *[N];
  for (int i = 0; i < N; i++)
    wp[i] = (Word *) savedWords[i];
  return wp;
}

// AddWord adds the word s to savedWords
bool RuleTerm::AddWord(Expr **p, const Word *s) {
  extern Expr *constantFalse; // defined in rules.y
  if (savedWordsN == MAX_WORDS_IN_EXPRESSION) {
    Message(MSG_WARNING, "Too many words in the same expression. The word ", 
	    s->String(), "did not fit.");
    return false;
  }
  savedExprPtr[savedWordsN] = p;
  if (p) {
    savedExpr[savedWordsN] = *p;
    *p = constantFalse;
  }
  savedWords[savedWordsN] = s;
  savedWordsN++;
  return true;
}

// AddHelpRuleWords adds all firstWords of the rule r to savedWords
bool RuleTerm::AddHelpRuleWords(HelpRule *r) {
  for (int i = 0; i < r->firstWordsN; i++)
    AddWord(NULL, r->firstWords[i]);
  return r->firstWordsOnly;
}

// CopyHelpRuleBestWords copies all bestWords of the help rule to bestWords
bool RuleTerm::CopyHelpRuleBestWords(HelpRule *r) {
  for (int i = 0; i < r->bestWordsN; i++)
    bestWords[i] = r->bestWords[i];
  bestWordsN = r->bestWordsN;
  return r->bestWordsOnly;
}

// RestoreExpressions restores the saved words and their expressions.
// It must be called after using savedWords.
void RuleTerm::RestoreExpressions() {
  while (savedWordsN > 0) {
    savedWordsN--;
    if (savedExprPtr[savedWordsN])
      *savedExprPtr[savedWordsN] = savedExpr[savedWordsN];
  }
}

static int MustBeWord(Expr *e, const char **result) {
  extern IdEntry *constantText; // defined in rules.y
  extern IdEntry *constantLemma; // defined in rules.y
  if (e->type != Expr::Operation) return 0;
  if (e->c.op.Op() == '&') {
    int left = MustBeWord(e->c.op.Left(), result);
    if (left == 0) return MustBeWord(e->c.op.Right(), result);
    return left;
  }
  if (e->c.op.Op() == '|') {
    const int wl = MustBeWord(e->c.op.Left(), result);
    if (wl == 0) return 0;
    const int wr = MustBeWord(e->c.op.Right(), result+wl);
    if (wr == 0) return 0;
    return wl+wr;
  }
  if (e->c.op.Op() != '=') return 0;

  Expr *expr = e->c.op.Left(), *strExpr = e->c.op.Right();
  
  if (!(expr->type == Expr::Attribute && expr->semtype == String &&
	(expr->c.id == constantText || expr->c.id == constantLemma))) {
    if (!(strExpr->type == Expr::Attribute && strExpr->semtype == String &&
	  (strExpr->c.id == constantText || strExpr->c.id == constantLemma)))
      return 0;
    Swap(strExpr, expr);
  }
  if (expr->c.id == constantLemma) {
    Word *w = scrutinizer->FindMainWord(strExpr->c.string);
    if (w) {
      int n = 0;
      for (int k=0; k<w->NLemmas(); k++)
	for (int j=0; j<w->Lemma(k)->NInflectRules(); j++)
	  for (int i=0; i<scrutinizer->Tags().Ct(); i++) {
	    WordTag *wt2 = w->GetForm(&(scrutinizer->Tags())[i], k, j);
	    if (wt2) {
	      for (int b=0; b<n; b++)
		if (!strcmp(result[b], wt2->String())) {
		  wt2 = NULL; break;
		}
	      if (wt2)
		result[n++] = wt2->String();
	    }
	  }
      return n;
    } else {
      Message(MSG_WARNING, "add lemma", strExpr->c.string,
	      "to main lexicon for better optimization");
      return 0;
    }
  }
  if (strExpr->type == Expr::Leaf && strExpr->semtype == String) {
    *result = strExpr->c.string;
    return 1;
  }
  return 0;
}

bool RuleTerm::ExtractWords(Expr **p) {
  if ((*p)->type != Expr::Operation)
    return false;
  const char *result[1000];
  switch ((*p)->c.op.Op()) {
  case '&':
  case '=': {
    const int ws = MustBeWord(*p, result);
    //std::cout << "MustBeWord() " << *p << " = " << ws << std::endl;
    for (int i=0; i<ws; i++) {
      Word *w = scrutinizer->FindMainWord(result[i]);
      if (w)
	AddWord(p, w); 
      else {
	//	Message(MSG_WARNING, "add", result[i], "to main lexicon for better optimization"); // tillfälligt borttaget 2000-11-24 av Viggo!!!!!!!!!
	return false;
      }
    }
    return ws > 0;
  }
  case '|':
    return (ExtractWords((*p)->c.op.LeftToChange()) && ExtractWords((*p)->c.op.RightToChange()));
  }
  return false;
}

RuleTerm::RuleTerm(Element *elements_, int nElements_, int leftContext, int rightContext) :
  category(NULL),
  mark(NULL),
  corr(NULL),
  info(NULL),
  action(NULL),
  detect(NULL),
  accept(NULL),
  linkURL(NULL),
  linkText(NULL),
  elements(elements_),
  nElements(nElements_),
  minScope(-1),
  minScopeWithContext(-1),
  exactScope(true),
  jump(NULL),
  rule(NULL),
  next(NULL),
  isHelp(false),
  number(-1),
  index(0),
  leftContextLength(0),
  savedWords(NULL),
  bestWords(NULL),
  savedExpr(NULL),
  savedExprPtr(NULL),
  savedWordsN(0),
  bestWordsN(0)
{
    NewRuleTerm();
    endLeftContext = leftContext > 0 ? leftContext : 0;
    beginRightContext = rightContext >= 0 ? rightContext : nElements_;
}

void RuleTerm::NewRuleTerm() {
  if (ruletermN == ruletermAllocated) {
    int oldno = ruletermAllocated;
    ruletermAllocated += 1000;
    RuleTerm **tmp = new RuleTerm *[ruletermAllocated];
    if (oldno > 0) {
      memcpy(tmp, ruleterms, oldno * sizeof(RuleTerm *));
      delete [] ruleterms; // jonas delete -> delete []
    }
    ruleterms = tmp;
  }
  index = ruletermN;
  ruleterms[ruletermN++] = this;
}

void RuleTerm::SetMark(Expr *mark_) {
  if (mark)
    ParseError("Mark-fältet får inte sättas flera gånger i samma högerled.");
  mark = mark_;
}

void RuleTerm::SetCorr(Expr *corr_) {
  if (!corr) {
    corr = corr_;
    return;
  }
  Expr *c;
  for (c = corr; c->Next(); c = c->Next());
  c->SetNext(corr_);
}

void RuleTerm::SetJump(GotoEntry *jump_) {
  if (jump)
    ParseError("Jump-fältet får inte sättas flera gånger i samma högerled.");
  jump = jump_;
}

void RuleTerm::SetInfo(Expr *info_) {
  if (info)
    ParseError("Info-fältet får inte sättas flera gånger i samma högerled.");
  info = info_;
}

void RuleTerm::SetAction(Expr *action_) {
  if (action)
    ParseError("Action-fältet får inte sättas flera gånger i samma högerled.");
  action = action_;
  if (action) printf("Actionregel med op %d (HELP=%d)\n", (int) action->c.op.Op(), HELPIDENTSYM); // Viggotest
  isHelp = (action && action->c.op.Op() == HELPIDENTSYM);
  isAccepting = (action && action->c.op.Op() == ACCEPTIDENTSYM);
  isScrutinizing = (action && action->c.op.Op() == CHECKIDENTSYM);
}

void RuleTerm::SetDetect(const char *detect_) {
  if (detect)
    ParseError("Detect-fältet får inte sättas flera gånger i samma högerled.");
  detect = detect_;
}

void RuleTerm::SetAccept(const char *accept_) {
  if (accept)
    ParseError("Accept-fältet får inte sättas flera gånger i samma högerled.");
  accept = accept_;
}

void RuleTerm::SetLink(const char *linkURL_, const char *linkText_) {
  if (linkURL)
    ParseError("Link-fältet får inte sättas flera gånger i samma högerled.");
  linkURL = linkURL_;
  linkText = linkText_;
}

void RuleTerm::ResolveHelpRules() {
  for (int i=0; i < nElements; i++)
    elements[i].ResolveHelpRule(GetRule());
}

void RuleTerm::TryMatching(AbstractSentence *s, int wordi, int wordsLeft) {
  const int startwordi = wordi-LeftContextLength();
  if (startwordi > 0) {
    xCurrRuleTerm = this;
    ElementMatching elementmatchings[MAXNOOFELEMENTS];
    Matching m(s, this, elementmatchings, startwordi); 
    ensure(startwordi <= s->NWords()+2);
    // Message(MSG_WARNING, "bad startwordi", int2str(startwordi));
    TryMatchingHelp(m, startwordi, wordsLeft+LeftContextLength(), 0);
    xCurrRuleTerm = NULL;
  }
}

bool RuleTerm::IsJumped(int pos) const {
  return (!IsHelp() && ruleSet.GetJumpIndex(pos) > GetRule()->Number());
}

void RuleTerm::TryMatchingHelp(Matching &m, const int wordi, const int wordsLeft, const int ei) {
  if (ei == BeginRightContext())
    m.rightContextStart = wordi;
  if (ei >= NElements()) { // matching successful
    if (IsJumped(wordi-1))
      return; // matches too late
    m.nTokens = wordi - m.Start();
    m.nTokensWithoutContext = m.rightContextStart - m.StartWithoutContext();
    HelpRule *hr = GetRule()->ThisIfHelpRule();
    if (hr) {
      union value res;
      m.GetElementMatching(ei).Init(m.GetSentence()->GetWordTokensAt(m.StartWithoutContext()),
				    m.nTokensWithoutContext,
				    matchingSet.GetChangeableTag());
      m.GetRuleTerm()->GetAction()->Eval(res, &m, ei, 0, Expr::HelpruleEnvironment);
    }
    if (GetJump()) {
      int jumpX = GetJump()->GetId()->GetRuleIndex();
      Expr *e = GetJump()->GetNoOfTokens();
      int jumpTokens = 0;
      if (e) {
	union value res;
	if (e->Eval(res, &m, ei, 0, Expr::LHSEnvironment) != 0)
	  jumpTokens = res.integer;
      }
      if (m.Start() + jumpTokens >= m.GetSentence()->NTokens()-1)
	Message(MSG_WARNING, GetRule()->Name(), ": jump scope covers too many tokens");
      ruleSet.SetJumpIndex(m.Start(), m.Start()+jumpTokens, jumpX);
      if (xPrintMatchings && strcmp(m.GetRule()->Name(), "stavOK@stavning")
	  ) { // && strcmp(m.GetRule()->Name(), "stilOK@stil")) {
	if (matchingSet.CheckMode()) std::cout << tab;
	std::cout << "jump: " << m << " (" << m.GetSentence()->GetWordToken(m.Start());
	if (jumpTokens > 2) std::cout << " ... ";
	if (jumpTokens > 1) std::cout << m.GetSentence()->GetWordToken(m.Start()+jumpTokens-1);
	if (jumpTokens > 0) std::cout << m.GetSentence()->GetWordToken(m.Start()+jumpTokens);
	std::cout << ')' << std::endl;
      }
    }
    if (hr) {
      Matching *nm = matchingSet.SaveHelp(&m);
      hr->SaveMatching(nm);
      nm->GetElementMatching(ei).matching = nm;
    } else if (IsScrutinizing())
      matchingSet.Found(&m);
    return;
  }
  if (IsJumped(wordi)) return;
  const Element &e = GetElement(ei);
  if (wordsLeft < e.MinRemainingWords())
    return; // not enough tokens for rule to be matched
  ElementMatching &em = m.GetElementMatching(ei);
  em.element = &e;
  em.matching = NULL;
  em.helpRuleTag = NULL;
  if (e.IsHelpRule()) { // must match help rule 
    bool triedZero = false;
    HelpRule *hr = e.GetHelpRule();
    if (!hr->HasTried(wordi)) {
      if (xOptimizeMatchings) {
	const int n = nHelpRuleTermsToCheck[wordi];
	int i;
	for (i = 0; i < n; i++)
	  if (helpRuleTermsToCheck[wordi][i]->GetRule() == hr)
	    helpRuleTermsToCheck[wordi][i]->TryMatching(m.GetSentence(), wordi, wordsLeft);
	for (i = 0; i < nHelpRuleTermsToCheckEverywhere; i++)
	  if (helpRuleTermsToCheckEverywhere[i]->GetRule() == hr)
	    helpRuleTermsToCheckEverywhere[i]->TryMatching(m.GetSentence(), wordi, wordsLeft);
      } else hr->TryMatching(m.GetSentence(), wordi, wordsLeft);
      hr->SetTried(wordi);
    }
    for (const Matching *hm = hr->FirstMatching(wordi); hm;
	 hm=hm->NextHelpRuleMatching()) {
      //      std::cout << "try help " << hr << std::endl;
      ensure(hm != &HelpRule::noMatch);
      int occ = hm->NTokens();
      const ElementMatching &hrem = hm->GetElementMatching(hm->GetRuleTerm()->NElements());
      em.tokenX = hrem.GetWordTokensAt(0);
      em.occurrences = hrem.Occurrences();
      em.helpRuleTag = hrem.GetHelpRuleTag();
      em.matching = hrem.GetMatching();
      union value res;
      if (e.GetExpr()->Eval(res, &m, ei, occ, Expr::LHSEnvironment) && res.boolean) {
	if (em.Occurrences() == 0) triedZero = true;
	TryMatchingHelp(m, wordi + em.Occurrences(), wordsLeft - em.Occurrences(), ei + 1);
      }
    }
    if (!triedZero && e.Occurrences() == ZeroOrOne) { // try 0 occurrences
      em.occurrences = 0;
      TryMatchingHelp(m, wordi, wordsLeft, ei + 1); 
    }
    return;
  }
  em.tokenX = m.GetSentence()->GetWordTokensAt(wordi);
  if (e.Occurrences() != ExactlyOne &&
      e.Occurrences() != OneOrMore) { // try 0 occurrences
    em.occurrences = 0;
    TryMatchingHelp(m, wordi, wordsLeft, ei + 1); 
  }
  int max;
  switch (e.Occurrences()) {
  case ZeroOrOne:
  case ExactlyOne: max = 1; break;
  case ZeroOrMore: 
  case OneOrMore: max = wordsLeft; break;
  default: max = e.Occurrences();
  }
  //  std::cout << "e: "; e.Print(); std::cout << " ei:" << ei << " max: " << max << std::endl;
  for (int occ = 0; occ < max; occ++) {
    em.occurrences = occ + 1;
    union value res;
    if (e.GetExpr()->Eval(res, &m, ei, occ, Expr::LHSEnvironment) && res.boolean) {
      //std::cout << GetRule() << " matched " << occ+1 << " tokens at pos " << wordi
      //	   << " (" << m.GetSentence()->GetWord(wordi) << ')' << std::endl;
      TryMatchingHelp(m, wordi + occ + 1, wordsLeft - occ - 1, ei + 1);
    } else break;
  }
}

void RuleTerm::Print(std::ostream &out) const { 
  for (int i = 0; i < nElements; i++) {
    elements[i].Print();
    out << std::endl;
  }
  out << (ExactScope() ? "Exakt " : "Minst ") << MinScope() 
      << " matchade ord." << std::endl;
  if (mark) out << "    mark: " << mark << std::endl;
  if (corr) out << "    corr: " << corr << std::endl;
  if (jump) out << "    jump: " << jump << std::endl;
  if (info) out << "    info: " << info << std::endl;
  if (detect) out << "  detect: " << detect << std::endl;
  if (accept) out << "  accept: " << accept << std::endl;
  if (linkURL) out << "    URL link: " << linkURL << std::endl;
  if (linkText) out << "    link text: " << linkText << std::endl;
  out << "    action: " << action << std::endl;
}

int RuleTerm::ComputeScope() {
  int remainingWords = 0;
  exactScope = true;
  leftContextLength = 0;
  minScope = 0;
  for (int i = nElements-1; i >= 0; i--) {
    int escope = elements[i].MinScope();
    remainingWords = elements[i].minRemainingWords = remainingWords + escope;
    if (i < beginRightContext) {
      if (!elements[i].ExactScope()) exactScope = false;
      if (i < endLeftContext) {
	if (!elements[i].ExactScope()) 
	  Message(MSG_WARNING, "Vänsterkontexten har inte fix längd i", GetRule()->Name());
	leftContextLength += escope;
      } else minScope += escope;
    }
  }
  minScopeWithContext = remainingWords;
  return minScope;
}

const char *RuleTerm::EvaluateInfo(Matching *m) const {
  if (!info) {
    if (GetCategory())
      return GetCategory()->Info();
    return NULL;
  }
  value s;
  xCurrRuleTerm = this;
  infoStringBuf.Reset();
  if (!info->Eval(s, m, 0, 0, Expr::LHSEnvironment)) {
    xCurrRuleTerm = NULL;
    return NULL;
  }
  xCurrRuleTerm = NULL;
  return s.string;
}

bool RuleTerm::EvaluateCorr(Matching *m, Expr* c) const {
  ensure(c);
  xCurrRuleTerm = this;
  value s;
  evalStringBuf.Reset();
  if (!c->Eval(s, m, 0, 0, Expr::CorrectionEnvironment)) {
    xCurrRuleTerm = NULL;
    return false;
  }
  xCurrRuleTerm = NULL;
  return true;
}

bool RuleTerm::EvaluateAction(Matching *m) const {
  if (!action) return false;
  xCurrRuleTerm = this;
  value s;
  if (!action->Eval(s, m, 0, 0, Expr::ActionEnvironment)) {
    xCurrRuleTerm = NULL;
    return false;
  }
  xCurrRuleTerm = NULL;
  return s.boolean;
}

bool RuleTerm::EvaluateMark(Matching *m) const {
  if (!mark) return false;
  xCurrRuleTerm = this;
  value s;
  if (!mark->Eval(s, m, 0, 0, Expr::MarkEnvironment)) {
    xCurrRuleTerm = NULL;
    return false;
  }
  xCurrRuleTerm = NULL;
  return s.boolean;
}

void HelpRule::SaveMatching(Matching *m) {
  int pos = m->StartWithoutContext();
  ensure(pos >= 0);
  ensure(pos < MAX_SENTENCE_LENGTH);
  ensure(ms[pos] != &noMatch);
  m->nextHelpRuleMatching = ms[pos];
  ms[pos] = m;
}

Rule::Rule(IdEntry *id_, RuleTerm *first) :
  id(id_),
  minScope(-1),
  minScopeWithContext(-1),
  exactScope(true),
  firstRuleTerm(first),
  ignored(false),
  evaluationTime(0) {
  int i = 0;
  for (RuleTerm *r = firstRuleTerm; r; r = r->Next()) {
    r->rule = this;
    r->number = i;
    i++;
  }
  nTerms = i;
}

RegExpRule::RegExpRule(Element *el, const char *regexp_, IdEntry *id, Expr *mark_, 
	     const char *corr_, GotoEntry *jump_, Expr *info_, Expr *action_,
	     const char *detect_, const char *accept_) :
    Rule(id, NULL),
    element(el),
    regexp(regexp_),
    mark(mark_),
    corr(corr_),
    jump(jump_),
    info(info_),
    action(action_),
    detect(detect_),
    accept(accept_)
  {
    /*
    std::cout << "RegExpRule: " << id->Name() << 
      " regexp: '" << regexp_ <<
      "' corr: '" << (corr_ ? corr_ : "") << 
      "' detect: '" << (detect_ ? detect_ : (const char *) "") <<
      "' accept: '" << (accept_ ? accept_ : (const char *) "") << "'" << std::endl; 
    */
    std::cout << id->Name() << std::endl <<
      regexp_ << std::endl <<
      (detect_ ? detect_ : (const char *) "") << std::endl <<
      (accept_ ? accept_ : (const char *) "") << std::endl << 
      "----------------------------------------------------------" << std::endl; 
  }

const char* Rule::Name() const {
  if (id) return id->Name();
  else return "-";
}

const char *Rule::Header() const {
  static char s[100];
  sprintf(s, "%s%i %s", Type(), Number(), Name());
  return s;
}

const char *RuleTerm::Header() const {
  if (GetRule()->NTerms() == 0)
    return GetRule()->Header();
  static char s[100];
  sprintf(s, "%s, term #%i", GetRule()->Header(), Number());
  return s;
}

void Rule::FindMatchings(AbstractSentence *s) {
  const int slen = s->NWords()+2;
  for (int j=1; j <= slen - MinScopeWithContext() + 1; j++) // johan 0-> 1
    TryMatching(s, j, slen - j + 1);
}

/*
  using stack doesn't work on PC:

  RuleTerm *ruleTermsToCheck[MAX_RULES][MAX_SENTENCE_LENGTH]; 
  RuleTerm *ruleTermsToCheckEverywhere[MAX_RULES][MAX_SENTENCE_LENGTH];
  int ruleTermsToCheckIdx[MAX_RULES][MAX_SENTENCE_LENGTH];
*/

void RuleTerm::FindMatchingsOptimized(AbstractSentence *s) {
  Timer timer; if (xTakeTime) timer.Start();
  int tags[MAX_SENTENCE_LENGTH];
  const int slen = s->NWords()+2;
  ruleSet.PrepareMatching(s);
  int i, j;
  static RuleTerm **ruleTermsToCheck[MAX_RULES];
  static RuleTerm **ruleTermsToCheckEverywhere[MAX_RULES];
  static int *ruleTermsToCheckIdx[MAX_RULES];
  static int nRuleTermsToCheck[MAX_RULES];
  static int nRuleTermsToCheckEverywhere[MAX_RULES];
  static bool hej = true;
  if (hej) {
    hej = false;
    for (i=0; i<MAX_RULES; i++)
      ruleTermsToCheck[i] = new RuleTerm*[MAX_SENTENCE_LENGTH];
    for (i=0; i<MAX_RULES; i++)
      ruleTermsToCheckEverywhere[i] = new RuleTerm*[MAX_SENTENCE_LENGTH];
    for (i=0; i<MAX_RULES; i++)
      ruleTermsToCheckIdx[i] = new int[MAX_SENTENCE_LENGTH];
  }  
  const int nrules = ruleSet.NRules();
  int firstToCheck = nrules, lastToCheck = -1;
  // Find the tags of the sentence:
  for (j=1; j <= slen; j++) { // johan 0->1
    tags[j] = s->GetWordToken(j)->SelectedTag()->Index();
    nHelpRuleTermsToCheck[j] = 0;
  }
  nHelpRuleTermsToCheckEverywhere = 0;
  for (j=0; j < nrules; j++) 
    nRuleTermsToCheck[j] = nRuleTermsToCheckEverywhere[j] = 0;
  // Compute the possible ruleterms that should be checked later
  for (i = 1; i < slen; i++) { // johan 0-> 1
    const int n = matchingCheckN[tags[i]][tags[i+1]];
    for (j = 0; j < n; j++) {
      RuleTerm *r = matchingCheck[tags[i]][tags[i+1]][j];
      // std::cout << r->GetRule() << std::endl;
      if (r->anchorTokens < 0) {
	const int rulenumber = r->GetRule()->Number();
	if (r->IsHelp()) {
	  bool addR = true;
	  for (int p=0; p<nHelpRuleTermsToCheckEverywhere; p++)
	    if (helpRuleTermsToCheckEverywhere[p] == r) {
	      //std::cout << "test a" << std::endl;
	      addR = false;
	      break;
	    }
	  if (addR) {
	    const int m = nHelpRuleTermsToCheckEverywhere++;
	    helpRuleTermsToCheckEverywhere[m] = r;
	  }
	} else {
	  bool addR = true;
	  for (int p=0; p<nRuleTermsToCheckEverywhere[rulenumber]; p++)
	    if (ruleTermsToCheckEverywhere[rulenumber][p] == r) {
	      addR = false;
	      //std::cout << "test b" << std::endl;
	      break;
	    }
	  if (addR) {
	    if (rulenumber < firstToCheck) firstToCheck = rulenumber;
	    if (rulenumber > lastToCheck) lastToCheck = rulenumber;
	    const int m = nRuleTermsToCheckEverywhere[rulenumber]++;
	    ruleTermsToCheckEverywhere[rulenumber][m] = r;
	  }
	}
      } else {
	const int rulestartpos = i - r->anchorTokens;
	const int idx = rulestartpos + r->leftContextLength;
	if (rulestartpos >= 0 && rulestartpos + r->MinScopeWithContext() <= slen + 1) {
	  const int rulenumber = r->GetRule()->Number();
	  if (r->IsHelp()) {
#ifdef EXTRASAFE
	    bool addR = true;
	    for (int p=0; p<nHelpRuleTermsToCheck[idx]; p++)
	      if (helpRuleTermsToCheck[idx][p] == r) {
		addR = false;
		ensure(0); // not happended yet
		break;
	      }
	    if (addR) 
#endif
	    {
	      const int m = nHelpRuleTermsToCheck[idx]++;
	      helpRuleTermsToCheck[idx][m] = r;
	    }
	  } else {
#ifdef EXTRASAFE
	    bool addR = true;
	    for (int p=0; p<nRuleTermsToCheck[rulenumber]; p++)
	      if (ruleTermsToCheck[rulenumber][p] == r && 
		  ruleTermsToCheckIdx[rulenumber][p] == idx) {
		addR = false;
		ensure(0); // not happended yet
		break;
	      }
	    if (addR) 
#endif
	    {
	      if (rulenumber < firstToCheck) firstToCheck = rulenumber;
	      if (rulenumber > lastToCheck) lastToCheck = rulenumber;
	      const int m = nRuleTermsToCheck[rulenumber]++;
	      ruleTermsToCheck[rulenumber][m] = r;
	      ruleTermsToCheckIdx[rulenumber][m] = idx;
	    }
	  }
	}
      }
    }
  }
  for (i = 1; i <= slen; i++) {  //johan 0->1
    const Word *w = s->GetWord(i);
    for (const RuleTermList *wrt = scrutinizer->FindWordRuleTerms(w); 
	 wrt; wrt = wrt->Next()) {
      RuleTerm *r = (RuleTerm *) wrt->GetRuleTerm();
      if (r->anchorTokens < 0) {
	const int rulenumber = r->GetRule()->Number();
	if (r->IsHelp()) {
	  bool addR = true;
	  for (int p=0; p<nHelpRuleTermsToCheckEverywhere; p++)
	    if (helpRuleTermsToCheckEverywhere[p] == r) {
	      addR = false;
	      //std::cout << "test e" << std::endl;
	      break;
	    }
	  if (addR) {
	    const int n = nHelpRuleTermsToCheckEverywhere++;
	    helpRuleTermsToCheckEverywhere[n] = r;
	  }
	} else {
	  bool addR = true;
	  for (int p=0; p<nRuleTermsToCheckEverywhere[rulenumber]; p++)
	    if (ruleTermsToCheckEverywhere[rulenumber][p] == r) {
	      addR = false;
	      //	      std::cout << "test f" << std::endl;
	      break;
	    }
	  if (addR) {
	    if (rulenumber < firstToCheck) firstToCheck = rulenumber;
	    if (rulenumber > lastToCheck) lastToCheck = rulenumber;
	    const int n = nRuleTermsToCheckEverywhere[rulenumber]++;
	    ruleTermsToCheckEverywhere[rulenumber][n] = r;
	  }
	}
      } else {
	const int rulestartpos = i - r->anchorTokens;
	const int idx = rulestartpos + r->leftContextLength;
	if (rulestartpos >= 0 && rulestartpos + r->MinScopeWithContext() <= slen + 1) {
	  const int rulenumber = r->GetRule()->Number();
	  if (r->IsHelp()) {
#ifdef EXTRASAFE
	    bool addR = true;
	    for (int p=0; p<nHelpRuleTermsToCheck[idx]; p++)
	      if (helpRuleTermsToCheck[idx][p] == r) {
		ensure(0); // this has never happened yet
		addR = false;
		break;
	      }
	    if (addR) 
#endif
	    {
	      const int n = nHelpRuleTermsToCheck[idx]++;
	      helpRuleTermsToCheck[idx][n] = r;
	    }
	  } else {
#ifdef EXTRASAFE
	    bool addR = true;
	    for (int p=0; p<nRuleTermsToCheck[rulenumber]; p++)
	      if (ruleTermsToCheck[rulenumber][p] == r && 
		  ruleTermsToCheckIdx[rulenumber][p] == idx) {
		addR = false;
		ensure(0); // this has never happened yet
		break;
	      }
	    if (addR) 
#endif
	    {
	      if (rulenumber < firstToCheck) firstToCheck = rulenumber;
	      if (rulenumber > lastToCheck) lastToCheck = rulenumber;
	      const int n = nRuleTermsToCheck[rulenumber]++;
	      ruleTermsToCheck[rulenumber][n] = r;
	      ruleTermsToCheckIdx[rulenumber][n] = idx;
	    }
	  }
	}
      }
    }
  }
  // Try to match the possible ruleterms:
  if (xTakeTime) prepTime += timer.Restart();
  for (int ruleno = firstToCheck; ruleno <= lastToCheck; ruleno++) {
    int n = nRuleTermsToCheck[ruleno];
    if (n > 0) { // Kolla också om regeln är aktiv
      for (i = 0; i < n; i++) {
	RuleTerm *r = ruleTermsToCheck[ruleno][i];
	
	const int idx = ruleTermsToCheckIdx[ruleno][i];
	r->TryMatching(s, idx, slen - idx + 1);
	if (xTakeTime) r->GetRule()->evaluationTime += timer.Restart();
      }
    }
    // Kolla regler som måste matchas överallt:
    n = nRuleTermsToCheckEverywhere[ruleno];
    if (n > 0) { // Kolla också om regeln är aktiv
      for (i = 0; i < n; i++) {
	RuleTerm *r = ruleTermsToCheckEverywhere[ruleno][i];
	// if (i > 0) std::cout << "&&& i>0 för regel " << r << std::endl;
	const int ubound = slen - r->MinScopeWithContext() + 1;
	for (int j = 1; j <= ubound; j++) // johan 0 -> 1
	  r->TryMatching(s, j, slen - j + 1);
	if (xTakeTime) r->GetRule()->evaluationTime += timer.Restart();
      }
    }
  }
}

GbgRule::GbgRule(IdEntry *id, RuleTerm *posTerm, RuleTerm *negTerm, Expr *info_) :
  Rule(id, posTerm),
  negRuleTerm(negTerm),
  info(info_) {
    int i = 1;
    for (RuleTerm *r = negRuleTerm; r; r = r->Next()) {
      r->rule = this;
      r->number = i;
      i++;
    }
}

void GbgRule::FindMatchings(AbstractSentence *s) {
  /* out of order
  Rule::FindMatchings(s);
  MatchingSet &set = RuleTerm::GetMatchingSet();  
  //  const int oldNMatchings = set.NMatchings();
  for (int i=set.NMatchings()-1; i>=0; i--) {
    Matching *m = set.GetMatching(i);
    if (m->GetSentence() != s)
      break;
    if (m->GetRule() == this) {
      for (RuleTerm *t = negRuleTerm; t; t = t->Next()) {
	ensure(s == m->GetSentence());
	t->TryMatching(s, m->Start(), m->NTokens());
      }
      for (int j=oldNMatchings; j<set.NMatchings(); j++)
	if (m->CoversSameTokens(set.GetMatching(j))) {
	  m->SetNotOK();
	  for (int k=oldNMatchings; k<set.NMatchings(); k++)
	    set.GetMatching(k)->SetNotOK();
	  break;
	}
    }
  }
  //  set.DiscardNotOKMatchings(oldNMatchings, set.NMatchings()-1);
*/
}

void Rule::Print(std::ostream &out) const {  
  out << "Rule # " << number << ' ' << (id ? id->Name() : "")
      << ", at least " << minScope << " words" << std::endl;
  for (const RuleTerm *r = firstRuleTerm; r; r = r->Next())
    out << "RuleTerm: " << std::endl << r;
}

void GbgRule::Print(std::ostream &out) const {
  Rule::Print(out);
  out << "Neg terms: " << std::endl;
  for (const RuleTerm *r = negRuleTerm; r; r = r->Next())
    out << "RuleTerm: " << std::endl << r;
}

void Rule::ResolveHelpRules() {
  for (RuleTerm *t = firstRuleTerm; t; t = t->Next())
    t->ResolveHelpRules();
}

void GbgRule::ResolveHelpRules() {
  Rule::ResolveHelpRules();
  for (RuleTerm *t = negRuleTerm; t; t = t->Next())
    t->ResolveHelpRules();
}

void Rule::OptimizeRuleMatching() {
  for (RuleTerm *t = firstRuleTerm; t; t = t->Next())
    t->OptimizeRuleMatching();
}

void GbgRule::OptimizeRuleMatching() {
  Rule::OptimizeRuleMatching();
  for (RuleTerm *t = negRuleTerm; t; t = t->Next())
    t->OptimizeRuleMatching();
}

void Rule::TryMatching(AbstractSentence *s, int wordi, int wordLeft) {
  for (RuleTerm *t = firstRuleTerm; t; t = t->Next())
    t->TryMatching(s, wordi, wordLeft);
}

// ComputeScope can be called recursively if a help rule calls itself!
int Rule::ComputeScope() {
  if (minScope >= 0) return minScope;
  minScope = firstRuleTerm->ComputeScope();
  minScopeWithContext = firstRuleTerm->MinScopeWithContext();
  exactScope = firstRuleTerm->exactScope;
  for (RuleTerm *r = firstRuleTerm->Next(); r; r = r->Next()) {
    if (r->ComputeScope() < minScope)
      minScope = r->MinScope();
    if (r->MinScopeWithContext() < minScopeWithContext)
      minScopeWithContext = r->MinScopeWithContext();
    exactScope &= r->exactScope;
  }
  return minScope;
}

int GbgRule::ComputeScope() {
  Rule::ComputeScope();  
  for (RuleTerm *r = negRuleTerm; r; r = r->Next())
    if (r->ComputeScope() > minScope)
      Message(MSG_WARNING, "negRuleTerm", int2str(r->Number()),
	      "has longer minScope than the positive term");
  return minScope;
}

int RegExpRule::ComputeScope() {
  minScope = -1; /* a regexp rule has no word scope */
  return minScope;
}

void ZeroTagPairMatrix(bool t[MAX_TAGS][MAX_TAGS]) {
  for (int i = 0; i < MAX_TAGS; i++)
    for (int j = 0; j < MAX_TAGS; j++)
      t[i][j] = false;
}

// TokensBefore counts the (minimal) number of tokens before Element index.
int RuleTerm::TokensBefore(int index)
{ int sum = 0;
  for (int i = 0; i < index; i++) sum += GetElement(i).MinScope();
  return sum;
}

void RuleTerm::OptimizeRuleMatching() {
  int minAnchor = 0;
  double minProb = INFINITY_PROBABILITY;
  bool minOnlyWords = false;
  int occurrences[MAXNOOFELEMENTS];
  int i, j;
  bool minPossibleTagPair[MAX_TAGS][MAX_TAGS];
  ZeroTagPairMatrix(minPossibleTagPair);
  
  if (xPrintOptimization) {
    std::cout << GetRule();
    if (GetRule()->NTerms() > 1) std::cout << " term #" << Number();
    std::cout << ":" << std::endl;
  }
  ZeroSavedWords();
  bestWordsN = 0;
  if (MinScopeWithContext() == 1) {
    bool minPossibleSingleTags[MAX_TAGS];
    for (i = 0; i < nTags; i++) minPossibleSingleTags[i] = false;
    ensure(savedWordsN == 0);
    bool onlyWords = ComputePossibleSingleTags(minPossibleSingleTags, occurrences, 0); 
    CopyToBestWords();
    anchorTokens = 0;
    double prob = SavedWordProb(savedWords, savedWordsN);
    if (!onlyWords) {
      for (i = 0; i < nTags; i++)
	if (minPossibleSingleTags[i]) {
	  for (j = 0; j < nTags; j++)
	    prob += tagLexicon->Pt1t2((uchar)i,(uchar)j);
	}
    }
    RestoreExpressions();
    bool newOnlyWords;
    if (OptimizeRuleMatchingExtra(prob * MUST_CHECK_ALL_FACTOR, true, prob,
				  newOnlyWords, minPossibleTagPair)) {
      onlyWords = newOnlyWords;
      if (!onlyWords) {
	for (i = 0; i < nTags; i++)
	  for (j = 0; j < nTags; j++)
	    if (minPossibleTagPair[i][j]) AddMatchingCheck(i, j);
      }
    } else {
      if (!onlyWords) {
	for (i = 0; i < nTags; i++)
	  if (minPossibleSingleTags[i]) AddMatchingCheck(i);
      }
      if (xPrintOptimization) {
	std::cout << tab << "word match: ";
	PrintBestWords();
	std::cout << std::endl;
	if (!onlyWords) {
	  bool allTags = true;
	  int nSingle = 0;
	  for (i = 0; i < nTags; i++)
	    if (minPossibleSingleTags[i]) nSingle++;
	    else allTags = false;
	  if (allTags) { std::cout << tab << "All " << nTags << " tags match." << std::endl; }
	  else {
	    if (xVerbose) {
	      std::cout << "% ";
	      for (i = 0; i < nTags; i++)
		if (minPossibleSingleTags[i])
		  std::cout << Tag::tags[i]->String() << " ";
	      std::cout << std::endl;
	    }
	    std::cout << tab << nSingle << " matching tags" << std::endl;
	  }
	}
      }
    }
    AddBestWordsAndRuleTerm();
  } else {
    for (int anchor = 0; anchor<NElements() && GetElement(anchor).MinRemainingWords() >= 1; anchor++) {
      bool possibleTagPair[MAX_TAGS][MAX_TAGS];
      Element &e = GetElement(anchor);
      ZeroTagPairMatrix(possibleTagPair);
      ensure(savedWordsN == 0);
      if (e.IsHelpRule() && e.GetHelpRule()->FirstTags() == NULL) {
	if (xPrintOptimization) std::cout << tab << e << " not yet computed help rule" << std::endl;
      } else {
	bool onlyWords = ComputePossibleTagPairs(possibleTagPair, occurrences, anchor, 2, anchor, NULL);
	if (!onlyWords && e.MinRemainingWords() == 1) {
	  if (xPrintOptimization) std::cout << tab << e << " prob: no word" << std::endl;
	  RestoreExpressions();
	  break;
	}
	double prob = SavedWordProb(savedWords, savedWordsN);
	if (!onlyWords) {
	  for (i = 0; i < nTags; i++)
	    for (j = 0; j < nTags; j++)
	      if (possibleTagPair[i][j]) 
		prob += tagLexicon->Pt1t2((uchar)i,(uchar)j);
	}
	if (prob == 0.0 && !onlyWords) // Viggo
	  std::cerr << "Prob=0 in OptimizeRuleMatching() (is this bad undrar Johan?) " 
	       << GetRule() << " term #" << Number() << " element "
	       << anchor << ':' << e << std::endl;
	if (xPrintOptimization)
	  std::cout << tab << anchor << ' ' << e << " prob: " << prob
	       << (onlyWords ? " (word)" : " (tags)") << std::endl;
	if (prob < minProb) {
	  minProb = prob;
	  minAnchor = anchor;
	  CopyToBestWords();
	  minOnlyWords = onlyWords;
	  if (!onlyWords)
	    for (i = 0; i < nTags; i++)
	      for (j = 0; j < nTags; j++)
		minPossibleTagPair[i][j] = possibleTagPair[i][j];
	}
	RestoreExpressions();
      }
      if (!GetElement(anchor).ExactScope()) break;
      occurrences[anchor] = GetElement(anchor).MinScope();
    }
    bool newOnlyWords = false;
    if (OptimizeRuleMatchingExtra(minProb * MUST_CHECK_ALL_FACTOR, true, minProb,
				  newOnlyWords, minPossibleTagPair)) {
      minOnlyWords = newOnlyWords;
    } else {
      anchorTokens = minAnchor < 0 ? minAnchor : TokensBefore(minAnchor);
      if (xPrintOptimization) {
	bool everyTag = true;
	if (!minOnlyWords)
	  for (i = 0; i < nTags; i++)
	    for (j = 0; j < nTags; j++)
	      if (!minPossibleTagPair[i][j]) {
		everyTag = false;
		break;
	      }
	std::cout << tab << "anchor " << GetElement(minAnchor) << " with prob "
	     << minProb << ":" << std::endl;
	PrintBestWords();
	if (!minOnlyWords) {
	  if (everyTag) std::cout << tab << "All tag-pair combinations! (not good)" << std::endl;
	  else {
	    int nPairs = 0;
	    for (i = 0; i < nTags; i++) {
	      bool anyTag = false, allTags = true;
	      for (j = 0; j < nTags; j++)
		if (minPossibleTagPair[i][j]) {
		  nPairs++; anyTag = true; 
		} else allTags = false;
	      if (xVerbose && anyTag) {
		std::cout << "% " << Tag::tags[i]->String() << ": ";
		if (allTags) std::cout << "all tags";
		else
		  for (j = 0; j < nTags; j++)
		    if (minPossibleTagPair[i][j])
		      std::cout << Tag::tags[j]->String() << " ";
		std::cout << std::endl;
	      }
	    }
	    std::cout << tab << nPairs << " matching tag pairs" << std::endl;
	  }
	}
      }
    }
    AddBestWordsAndRuleTerm();
    if (!minOnlyWords)
      for (i = 0; i < nTags; i++)
	for (j = 0; j < nTags; j++)
	  if (minPossibleTagPair[i][j]) AddMatchingCheck(i,j);
  }
}

// the following function checks all positions in the rule, even without
// constant size left context. true is returned if better position was found.
bool RuleTerm::OptimizeRuleMatchingExtra(double probLevel, 
					 bool skipExactScope,
					 double &bestProb,
					 bool &minOnlyWords,
					 bool possibleTagPair[MAX_TAGS][MAX_TAGS]) {
  double minProb = probLevel;
  minOnlyWords = false;
  int occurrences[MAXNOOFELEMENTS];
  int i, j;
  
  if (xPrintOptimization) {
    std::cout << "X" << GetRule();
    if (GetRule()->NTerms() > 1) std::cout << " term #" << Number();
    std::cout << ":" << std::endl;
  }
  ZeroSavedWords();
  if (MinScopeWithContext() == 1) {
    bool minPossibleSingleTags[MAX_TAGS];
    int starti = 0;
    if (skipExactScope) {
      if (GetElement(0).MinScope() >= 1) return false;
      for (; starti < NElements() && GetElement(starti).MinScope() == 0;
	   starti++)
	occurrences[starti] = 0;
    }
    for (i = 0; i < nTags; i++) minPossibleSingleTags[i] = false;
    ensure(savedWordsN == 0);
    bool onlyWords = ComputePossibleSingleTags(minPossibleSingleTags, occurrences, starti); 
    double prob = SavedWordProb(savedWords, savedWordsN);
    double bestProb = probLevel;
    if (!onlyWords) {
      for (i = 0; i < nTags; i++)
	if (minPossibleSingleTags[i]) 
	  for (j = 0; j < nTags; j++)
	    prob += tagLexicon->Pt1t2((uchar)i,(uchar)j);
    }
    if (prob < bestProb) {
      CopyToBestWords();
      bestProb = prob;
      minOnlyWords = onlyWords;
      ZeroTagPairMatrix(possibleTagPair);
      if (!onlyWords) {
	for (i = 0; i < nTags; i++)
	  if (minPossibleSingleTags[i])
	    for (j = 0; j < nTags; j++)
	      possibleTagPair[i][j] = 1;
      }
    }
    RestoreExpressions();
    for (int elementNo = 0; elementNo < NElements(); elementNo++) {
      Element &e = GetElement(elementNo);
      if (e.MinScope() > 0 && e.IsHelpRule()) {
	HelpRule *r = e.GetHelpRule();
	if (r->bestWords && r->bestProb < bestProb) {
	  bestProb = r->bestProb;
	  minOnlyWords = CopyHelpRuleBestWords(r);
	  if (!minOnlyWords) 
	    for (i = 0; i < nTags; i++)
	      for (j = 0; j < nTags; j++)
		possibleTagPair[i][j] = r->BestTagsMatrix()[i][j];
	}
      }
    }
    if (bestProb == probLevel) return false;
    anchorTokens = -1;
    if (xPrintOptimization) {
      std::cout << tab << "word match: ";
      PrintBestWords();
      std::cout << std::endl;
      if (!onlyWords) {
	bool allTags = true;
	int nSingle = 0;
	for (i = 0; i < nTags; i++)
	  if (!minPossibleSingleTags[i]) { allTags = false; break; }
	if (allTags) { std::cout << tab << "All " << nTags << " tags match." << std::endl; }
	else {
	  if (xVerbose) {
	    std::cout << "% ";
	    for (i = 0; i < nTags; i++)
	      if (minPossibleSingleTags[i]) {
		std::cout << Tag::tags[i]->String() << " ";
		nSingle++;
	      }
	    std::cout << std::endl;
	  }
	  std::cout << tab << nSingle << " matching tags" << std::endl;
	}
      }
    }
    return true;
  }
  bool minPossibleTagPair[MAX_TAGS][MAX_TAGS];
  ZeroTagPairMatrix(minPossibleTagPair);
  int starti = 0;
  if (skipExactScope) {
    for (; starti < NElements() && GetElement(starti).ExactScope();
	 starti++)
      occurrences[starti] = GetElement(starti).MinScope();
    starti++;
  }
  for (int anchor = starti; anchor<NElements(); anchor++) {
    Element &e = GetElement(anchor);
    if (e.MinRemainingWords() < 1) break;
    if (e.MinScope() == 0 || e.IsHelpRule()) continue;
    bool possibleTagPair[MAX_TAGS][MAX_TAGS];
    ZeroTagPairMatrix(possibleTagPair);
    ensure(savedWordsN == 0);
    bool onlyWords;
    if (e.MinRemainingWords() == 1) {
      bool minPossibleSingleTags[MAX_TAGS];
      for (i = 0; i < nTags; i++) minPossibleSingleTags[i] = false;
      ensure(savedWordsN == 0);
      onlyWords = ComputePossibleSingleTags(minPossibleSingleTags, occurrences, anchor);
      if (!onlyWords) {
	for (i = 0; i < nTags; i++)
	  if (minPossibleSingleTags[i]) 
	    for (j = 0; j < nTags; j++)
	      possibleTagPair[i][j] = 1;
      }
    } else {
      onlyWords = ComputePossibleTagPairs(possibleTagPair, occurrences,
					  anchor, 2, anchor, NULL);
    }
    double prob = SavedWordProb(savedWords, savedWordsN);
    if (!onlyWords) {
      for (i = 0; i < nTags; i++)
	for (j = 0; j < nTags; j++)
	  if (possibleTagPair[i][j])
	    prob += tagLexicon->Pt1t2((uchar)i,(uchar)j);
    }
    if (prob == 0.0 && !onlyWords) // Viggo
      std::cerr << "Prob=0 in OptimizeRuleMatchingExtra " 
	   << GetRule() << " term #" << Number() << " element "
	   << anchor << ':' << e << std::endl;
    if (xPrintOptimization)
      std::cout << tab << anchor << ' ' << e << " prob: " << prob
	   << (onlyWords ? " (word)" : " (tags)") << " remaining: " 
	   << e.MinRemainingWords() << std::endl;
    if (prob < minProb) {
      minProb = prob;
      CopyToBestWords();
      minOnlyWords = onlyWords;
      if (!onlyWords)
	for (i = 0; i < nTags; i++)
	  for (j = 0; j < nTags; j++)
	    minPossibleTagPair[i][j] = possibleTagPair[i][j];
    }
    RestoreExpressions();
    occurrences[anchor] = e.MinScope();
  }
  for (int elementNo = 0; elementNo < NElements(); elementNo++) {
    Element &e = GetElement(elementNo);
    if (e.MinScope() > 0 && e.IsHelpRule()) {
      HelpRule *r = e.GetHelpRule();
      if (r->bestWords && r->bestProb && r->bestProb < minProb) {
	minProb = r->bestProb;
	minOnlyWords = CopyHelpRuleBestWords(r);
	if (xPrintOptimization) {
	  std::cout << "  " << bestWordsN << " matchande ord i element " << elementNo << ": ";
	  PrintBestWords();
	}
	if (!minOnlyWords && r->BestTagsMatrix())
	  for (i = 0; i < nTags; i++)
	    for (j = 0; j < nTags; j++)
	      minPossibleTagPair[i][j] = r->BestTagsMatrix()[i][j];
      }
    }
  }
  if (minProb == probLevel) return false; // no improvement made
  ZeroTagPairMatrix(possibleTagPair);
  anchorTokens = -1;
  bestProb = minProb;

  bool everyTag = true;
  if (!minOnlyWords)
    for (i = 0; i < nTags; i++)
      for (j = 0; j < nTags; j++)
	if (minPossibleTagPair[i][j]) possibleTagPair[i][j] = 1;
	else everyTag = false;
  if (xPrintOptimization) {
      // Print();
      std::cout << tab << "without anchor with prob "
	   << minProb << ":" << std::endl;
      PrintBestWords();
      if (!minOnlyWords) {
	if (everyTag) std::cout << tab << "All tag-pair combinations! (not so good)" << std::endl;
	else {
	  int nPairs = 0;
	  for (i = 0; i < nTags; i++) {
	    bool anyTag = false, allTags = true;
	    for (j = 0; j < nTags; j++)
	      if (minPossibleTagPair[i][j]) {
		nPairs++; anyTag = true; 
	      } else allTags = false;
	    if (xVerbose && anyTag) {
	      std::cout << "% " << Tag::tags[i]->String() << ": ";
	      if (allTags) std::cout << "all tags";
	      else
		for (j = 0; j < nTags; j++)
		  if (minPossibleTagPair[i][j])
		    std::cout << Tag::tags[j]->String() << " ";
	      std::cout << std::endl;
	    }
	  }
	  std::cout << tab << nPairs << " matching tag pairs" << std::endl;
	}
      }
  }
  return true;
}

void RuleTerm::AddMatchingCheck(int i, int j) {
// Check this rule term on tags (i,j)
  int n = matchingCheckN[i][j]++;
  if (n >= matchingCheckAllocatedN[i][j]) {
    RuleTerm **p = matchingCheck[i][j];
    int oldno = matchingCheckAllocatedN[i][j];
    matchingCheckAllocatedN[i][j] = 2 * n;
    matchingCheck[i][j] = new RuleTerm *[matchingCheckAllocatedN[i][j]];
    memcpy(matchingCheck[i][j], p, n * sizeof(RuleTerm *));
    if (oldno) delete p;
  }
  matchingCheck[i][j][n] = this;
}

void RuleTerm::AddMatchingCheck(int tag) {
// Check this rule term on tag
  for (int i = 0; i < nTags; i++) AddMatchingCheck(tag, i);
}

bool RuleTerm::ComputePossibleTagPairs(bool possibleTagPair[MAX_TAGS][MAX_TAGS],
				       int *occurrences, int anchor, 
				       int wordsToMatch, int elementIndex,
				       bool *firstTags) {
  bool onlyWords = false;
  if (elementIndex < NElements()) {
    occurrences[elementIndex] = 1;
    Element &e = GetElement(elementIndex);
    if (e.IsHelpRule()) {
      HelpRule *r = e.GetHelpRule();
      bool *secondTags;
      if (r->firstWordsOnly) {
	if (!allTags) {
	  allTags = new bool[nTags];
	  for (int i = 0; i < nTags; i++) allTags[i] = true;
	}
	secondTags = allTags;
      } else secondTags = r->FirstTags();
      if (wordsToMatch == 1) {
	ComputePossibleTag(possibleTagPair, occurrences, anchor, firstTags, 
			   secondTags, NULL);
      } else {
	onlyWords = AddHelpRuleWords(r);
	if (!onlyWords) {
	  if (r->MinScope() == 1)
	    ComputePossibleTagPairs(possibleTagPair, occurrences, anchor, wordsToMatch-1, elementIndex+1, r->FirstTags());
	  if (!(r->MinScope() == 1 && r->ExactScope())) {
	    occurrences[elementIndex] = 2;
	    ComputePossibleTag(possibleTagPair, occurrences, anchor, r->FirstTags(), NULL, r->FirstTagsMatrix());
	  }
	}
      }
      if (e.Occurrences() == ZeroOrOne) { // Element::
	occurrences[elementIndex] = 0;
	onlyWords &= ComputePossibleTagPairs(possibleTagPair, occurrences, anchor, wordsToMatch, elementIndex+1, firstTags);
      }
    } else {
      int occ = e.Occurrences();
      if (wordsToMatch == 1)
	ComputePossibleTag(possibleTagPair, occurrences, anchor, firstTags, NULL, NULL);
      else {
	onlyWords = ExtractWords(&e.expr);
	if (!onlyWords) {
	  ComputePossibleTagPairs(possibleTagPair, occurrences, anchor, wordsToMatch-1, elementIndex+1, NULL);
	  if ((occ == ZeroOrMore || occ == OneOrMore || occ > 1) && wordsToMatch > 1) { // Element::
	    occurrences[elementIndex] = 2;
	    ComputePossibleTag(possibleTagPair, occurrences, anchor, NULL, NULL, NULL);
	  }
	}
      }
      if (occ == ZeroOrMore || occ == ZeroOrOne) { //Element::
	occurrences[elementIndex] = 0;
	onlyWords &= ComputePossibleTagPairs(possibleTagPair, occurrences, anchor, wordsToMatch, elementIndex+1, firstTags);
      }
    }
  }
  return onlyWords;
}

void RuleTerm::ComputePossibleTag(bool possibleTagPair[MAX_TAGS][MAX_TAGS],
				  int *occurrences, int anchor, 
				  bool *firstTags, bool *secondTags,
				  bool **secondTagMatrix) {
  int i, j;
  const Expr *A = NULL, *B = NULL;
  bool unknown = false;
  union value res;
  for (i = anchor; occurrences[i] == 0; i++);
  if (occurrences[i] > 1) j = i;
  else for (j = i+1; occurrences[j] == 0; j++);
  if (!firstTags) A = GetElement(i).GetExpr();
  if (!secondTags && !secondTagMatrix) B = GetElement(j).GetExpr();
  if (firstTags) {
    for (int x = 0; x < nTags; x++) 
      if (firstTags[x]) {
	for (int y = 0; y < nTags; y++) 
	  if (B) {
	    if (!possibleTagPair[x][y]
		&& B->OptEval(unknown, res, occurrences, -1, NULL,
			      j, Tag::tags[y], anchor))
	      if (unknown || res.boolean) possibleTagPair[x][y] = true;
	  } else if (secondTags) {
	    if (secondTags[y]) possibleTagPair[x][y] = true;
	  } else
	    if (secondTagMatrix[x][y]) possibleTagPair[x][y] = true;
      }
  } else {
    for (int x = 0; x < nTags; x++) 
      if (A->OptEval(unknown, res, occurrences, -1, NULL, i, Tag::tags[x],
		     anchor))
	if (unknown || res.boolean) {
	  for (int y = 0; y < nTags; y++) 
	    if (B) {
	      if (!possibleTagPair[x][y]
		  && B->OptEval(unknown, res, occurrences, i, Tag::tags[x],
				j, Tag::tags[y], anchor))
		if (unknown || res.boolean) possibleTagPair[x][y] = true;
	    } else if (secondTags) {
	      if (secondTags[y]) possibleTagPair[x][y] = true;
	    } else
	      if (secondTagMatrix[x][y]) possibleTagPair[x][y] = true;
	}
  }
}

bool RuleTerm::ComputePossibleSingleTags(bool *possibleTags,
					 int *occurrences, int elementIndex) {
  bool onlyWords = false;
  if (elementIndex >= NElements()) return false;
  occurrences[elementIndex] = 1;
  const Element &e = GetElement(elementIndex);
  if (e.IsHelpRule()) {
    HelpRule *r = e.GetHelpRule();
    onlyWords = AddHelpRuleWords(r);
    if (!onlyWords) {
      bool *ft = r->FirstTags();
      for (int y = 0; y < nTags; y++) if (ft[y]) possibleTags[y] = true;
    }
    if (e.Occurrences() == ZeroOrOne) { // Element::
      occurrences[elementIndex] = 0;
      onlyWords &= ComputePossibleSingleTags(possibleTags, occurrences, elementIndex+1);
    }
  } else {
    Expr *A = e.GetExprToChange();
    bool unknown = false;
    union value res;
    if (A) {
      onlyWords = ExtractWords(&A);
      if (!onlyWords) {
	for (int x = 0; x < nTags; x++) 
	  if (!possibleTags[x]) {
	    if (A->OptEval(unknown, res, occurrences, -1, NULL,
			   elementIndex, Tag::tags[x], 0)) {
	      if (unknown || res.boolean) possibleTags[x] = true;
	    } else 
	      std::cerr << "ComputePossibleSingleTags: Eval error in element " << 
		elementIndex << " of " << this << std::endl;
	  }
      }
    } else 
      std::cerr << "ComputePossibleSingleTags: No expression in element " << 
	elementIndex << " of " << this << std::endl;
    int occ = e.Occurrences();
    if (occ == ZeroOrMore || occ == ZeroOrOne) { //Element::
      occurrences[elementIndex] = 0;
      onlyWords &= ComputePossibleSingleTags(possibleTags, occurrences, elementIndex+1);
    }
  }
  return onlyWords;
}

bool *HelpRule::FirstTags() {
  const int nTags = RuleTerm::nTags;
  bool possibleTagPair[MAX_TAGS][MAX_TAGS];
  bool wordsOnly = true;
  if (bestWordsN == -1) {
    std::cerr << "Regeln " << this << " rekursivt anrop av FirstTags" << std::endl;
    if (!bestTagsMatrix) {
      double prob = 0.0;
      for (int i = 0; i < nTags; i++)
	for (int j = 0; j < nTags; j++) {
	  possibleTagPair[i][j] = true;
	  prob += RuleTerm::tagLexicon->Pt1t2((uchar)i,(uchar)j);
	}
      bestWordsOnly = false;
      bestProb = prob;
    }
    return firstTags;
  }
  if (firstTags) return firstTags;
  bestWordsN = -1; // under beräkning
  firstTags = new bool[nTags];
  for (int k=0; k<nTags; k++) firstTags[k] = false;
  int occurrences[MAXNOOFELEMENTS];
  AllocateBestWords();
  RuleTerm *r;
  for (r = firstRuleTerm; r; r = r->Next()) {
    r->ZeroSavedWords();
    for (int i = 0; i < r->EndLeftContext(); i++)
      occurrences[i] = r->GetElement(i).MinScope();
    wordsOnly &= r->ComputePossibleSingleTags(firstTags, occurrences, r->EndLeftContext());
    r->AddSavedToBestWords(this);
    r->RestoreExpressions();
  }
  firstWordsOnly = wordsOnly;
  firstWords = bestWords;
  firstWordsN = bestWordsN;
  if (!wordsOnly) ComputeFirstTagsMatrix();
  AllocateBestWords();
  if (xPrintOptimization)
    std::cout << "Compute bestopt of " << this << std::endl;
  bool compoundPossibleTagPair[MAX_TAGS][MAX_TAGS];
  bool compoundWordsOnly = true;
  ZeroTagPairMatrix(compoundPossibleTagPair);
  for (r = firstRuleTerm; r; r = r->Next()) {
    wordsOnly = false;
    r->OptimizeRuleMatchingExtra(INFINITY_PROBABILITY, false, bestProb,
				 wordsOnly, possibleTagPair);
    compoundWordsOnly &= wordsOnly;
    r->AddBestToBestWords(this);
    if (!wordsOnly) 
      for (int i = 0; i < nTags; i++)
	for (int j = 0; j < nTags; j++)
	  compoundPossibleTagPair[i][j] |= possibleTagPair[i][j];
  }
  bestWordsOnly = compoundWordsOnly;
  if (!compoundWordsOnly) ComputeBestTagsMatrix(compoundPossibleTagPair);
  ComputeBestProb();
  if (xPrintOptimization) {
    std::cout << "Bestopt " << this << " words: ";
    for (int i = 0; i < bestWordsN; i++)
      std::cout << bestWords[i]->String() << " ";
    if (bestWordsOnly) std::cout << "(only words)";
    std::cout << ", probability=" << bestProb << std::endl;
  }
  return firstTags;
}

void HelpRule::ComputeBestProb() {
  double prob = SavedWordProb((const Word **) bestWords, bestWordsN);
  if (!bestWordsOnly)
    for (int i = 0; i < RuleTerm::nTags; i++)
      for (int j = 0; j < RuleTerm::nTags; j++)
	if (bestTagsMatrix[i][j])
	  prob += RuleTerm::tagLexicon->Pt1t2((uchar)i,(uchar)j);
  bestProb = prob;
}

bool **HelpRule::FirstTagsMatrix() {
  if (!firstTags) FirstTags();
  return firstTagsMatrix;
}

bool **HelpRule::BestTagsMatrix() {
  if (!firstTags) FirstTags();
  return bestTagsMatrix;
}

void HelpRule::AllocateBestWords() {
  bestWordsN = 0;
  bestWords = new Word *[MAX_WORDS_IN_EXPRESSION];
}

void HelpRule::ComputeFirstTagsMatrix() {
  const int nTags = RuleTerm::nTags;
  int occurrences[MAXNOOFELEMENTS];
  bool possibleTagPair[MAX_TAGS][MAX_TAGS];
  firstTagsMatrix = new bool *[nTags];
  for (int k = 0; k < nTags; k++) {
    firstTagsMatrix[k] = new bool[nTags];
    // for (int j = 0; j < nTags; j++) firstTagsMatrix[i][j] = false;
  }
  ZeroTagPairMatrix(possibleTagPair);
  for (RuleTerm *r = firstRuleTerm; r; r = r->Next()) {
    for (int i = 0; i < r->EndLeftContext(); i++)
      occurrences[i] = r->GetElement(i).MinScope();
    r->ComputePossibleTagPairs(possibleTagPair, occurrences, 
			       r->EndLeftContext(), 2, r->EndLeftContext(), NULL);
    r->RestoreExpressions();
  }
  for (int i = 0; i < nTags; i++)
    for (int j = 0; j < nTags; j++)
      firstTagsMatrix[i][j] = possibleTagPair[i][j];
}

void HelpRule::ComputeBestTagsMatrix(bool possibleTagPair[MAX_TAGS][MAX_TAGS]) {
  const int nTags = RuleTerm::nTags;
  bestTagsMatrix = new bool *[nTags];
  for (int k = 0; k < nTags; k++) {
    bestTagsMatrix[k] = new bool[nTags];
  }
  for (int i = 0; i < nTags; i++)
    for (int j = 0; j < nTags; j++)
      bestTagsMatrix[i][j] = possibleTagPair[i][j];
}

RuleTerm **RuleTerm::matchingCheck[MAX_TAGS][MAX_TAGS];
int RuleTerm::matchingCheckN[MAX_TAGS][MAX_TAGS];
int RuleTerm::matchingCheckAllocatedN[MAX_TAGS][MAX_TAGS];

void RuleTerm::CreateMatchingCheck() {
  for (int i = 0; i < nTags; i++)
    for (int j = 0; j < nTags; j++) {
      matchingCheck[i][j] = new RuleTerm *[MATCHING_CHECK_START_SIZE];
      matchingCheckN[i][j] = 0;
      matchingCheckAllocatedN[i][j] = MATCHING_CHECK_START_SIZE;
    }
}

inline static void WriteInt(FILE *fp, int i) {
  fwrite(&i, sizeof(int), 1, fp);
}
inline static void WriteShort(FILE *fp, int i) {
  ensure (i < SHRT_MAX);
  short i2 = short(i);
  fwrite(&i2, sizeof(short), 1, fp);
}

inline static int ReadInt(FILE *fp) {
  int i;
  if (fread(&i, sizeof(int), 1, fp) <= 0) return -1;
  return i;
}
inline static int ReadShort(FILE *fp) {
  short i;
  if (fread(&i, sizeof(short), 1, fp) <= 0) return -1;
  return i;
}

bool RuleTerm::SaveMatchingOptimization(FILE *fp) {
  WriteInt(fp, hashcode);
  int nsum = 0;
  int i;  
  int *nMatchCheck = new int[ruletermN];
  for (i=0; i<ruletermN; i++) nMatchCheck[i] = 0;
  for (i = 0; i < nTags; i++)
    for (int j = 0; j < nTags; j++) {
      nsum += matchingCheckN[i][j];
      for (int k=0; k<matchingCheckN[i][j]; k++) {
	nMatchCheck[matchingCheck[i][j][k]->GetIndex()]++;
	if (k > 0 && matchingCheck[i][j][k-1]->GetIndex() >= matchingCheck[i][j][k]->GetIndex()) {
	  Message(MSG_WARNING, "couldn't save, internal error 1");
	  delete nMatchCheck; return false;
	}
      }
    }
  const int limit = nTags*nTags / 2;
  WriteInt(fp, limit);
  for (i=0; i<ruletermN; i++)
    WriteInt(fp, nMatchCheck[i]);
  WriteInt(fp, -1);
  
  WriteInt(fp, nsum);
  for (i = 0; i < nTags; i++)
    for (int j = 0; j < nTags; j++) {
      const int n = matchingCheckN[i][j];
      WriteShort(fp, n);
      int k=0;
      for (int a=0; a<ruletermN; a++)
	if (k < n) {
	  if (a < matchingCheck[i][j][k]->GetIndex()) {
	    if (nMatchCheck[a] > limit)
	      WriteShort(fp, a);
	  } else if (a == matchingCheck[i][j][k]->GetIndex()) {
	    if (nMatchCheck[a] <= limit)
	      WriteShort(fp, a);
	    k++;
	  }
	} else
	  if (nMatchCheck[a] > limit)
	    WriteShort(fp, a);
      if (k != n) {
	Message(MSG_WARNING, "couldn't save, internal error 2");
	delete nMatchCheck; return false;
      }
      WriteShort(fp, ruletermN);
    }
  delete nMatchCheck;
  WriteInt(fp, -1);
  
  const int wordsN = scrutinizer->Words().Cw();
  for (int m = 0; m < wordsN; m++) {
    const Word &w = scrutinizer->Words()[m];
    if (w.IsRuleAnchor()) {
      WriteInt(fp, m);
      for (const RuleTermList *wrt = scrutinizer->FindWordRuleTerms(&w); wrt;
	   wrt = wrt->Next())
	WriteShort(fp, wrt->GetRuleTerm()->GetIndex());
      WriteShort(fp, -1);
    }
  }
  WriteInt(fp, -1);
  
  for (i = 0; i<ruleSet.NRules(); i++)
    for (RuleTerm *r = ruleSet.GetRule(i)->FirstRuleTerm(); r; r = r->Next())
      WriteShort(fp, r->anchorTokens);
  WriteInt(fp, -1);
  return true;
}

bool RuleTerm::ReadMatchingOptimization(FILE *fp) {
  int hashcode2 = ReadInt(fp);
  if (hashcode2 != hashcode) {
    Message(MSG_WARNING, "rule file changed, cannot fast load optimizations");
    return false;
  }
  const int limit = ReadInt(fp);
  int *nMatchCheck = new int[ruletermN];
  int i;
  for (i=0; i<ruletermN; i++)
    nMatchCheck[i] = ReadInt(fp);
  if (ReadInt(fp) != -1) {
    Message(MSG_WARNING, "error 2");
    delete nMatchCheck; return false;
  }
  const int nsum = ReadInt(fp);
  if (nsum <= 0) {
    Message(MSG_WARNING, "error 3");
    delete nMatchCheck; return false;
  }
  RuleTerm **p = new RuleTerm *[nsum];
  int pi = 0;
  for (i = 0; i < nTags; i++)
    for (int j = 0; j < nTags; j++) {
      const int n = ReadShort(fp);
      if (n < 0) {
	Message(MSG_WARNING, "error 4");
	delete nMatchCheck; return false;
      }
      matchingCheckN[i][j] = n;
      matchingCheckAllocatedN[i][j] = 0;
      matchingCheck[i][j] = &p[pi];
      pi += n;
      if (pi > nsum) {
	Message(MSG_WARNING, "error 4b");
	delete nMatchCheck; return false;
      }
      int k = 0, b = ReadShort(fp);
      for (int a=0; a<ruletermN; a++) {
	if (a < b) {
	  if (nMatchCheck[a] > limit)
	    matchingCheck[i][j][k++] = ruleterms[a];
	} else if (a == b) {
	  if (nMatchCheck[a] <= limit)
	    matchingCheck[i][j][k++] = ruleterms[a];
	  b = ReadShort(fp);
	}
      }
      if (b != ruletermN || k != n) {
	Message(MSG_WARNING, "error 5");
	delete nMatchCheck; return false;
      }
    }  
  delete nMatchCheck;
  if (ReadInt(fp) != -1) {
    Message(MSG_WARNING, "error 6"); return false;
  }  
  for(;;) {
    int wordi = ReadInt(fp);
    if (wordi == -1) break;
    if (wordi < 0 || wordi >= scrutinizer->Words().Cw()) {
      Message(MSG_WARNING, "error 7"); return false;
    }
    const Word &w = scrutinizer->Words()[wordi];
    for(;;) {
      int ri = ReadShort(fp);
      if (ri == -1) break;
      if (ri < 0 || ri >= ruletermN) {
	Message(MSG_WARNING, "error 8"); return false;
      }
      scrutinizer->AddWordRuleTerm((Word *) &w, ruleterms[ri]);
    }
  }
  for (i = 0; i<ruleSet.NRules(); i++)
    for (RuleTerm *r = ruleSet.GetRule(i)->FirstRuleTerm(); r; r = r->Next())
      r->anchorTokens = ReadShort(fp);
  return ReadInt(fp) == -1; 
  // nu borde filen fp vara slut
  return true;
}





