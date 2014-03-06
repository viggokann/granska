/* matchingset.cc
 * authors: Johan Carlberger
 * last change: 2000-03-24
 * comments: MatchingSet class
 */

#include "matchingset.h"
#include "matching.h"
#include "gramerror.h"
#include "rulesettings.h"

static int EM_BUF_SIZE = 5000;
static int HELP_MATCHING_BUF_SIZE = 4000;
/*
static const int EM_BUF_SIZE = 5000;
static const int HELP_MATCHING_BUF_SIZE = 1000;
*/
static const int HELP_TAG_BUF_SIZE = 10000; // Viggo increased size 2000->3000
                                            // Johnny increased size 3000->10000

static int max_tag_buf_end = 0;

MatchingSet::MatchingSet() : currSentence(NULL), checkMode(false),
			     nFound(0), nCheckModeFound(0),
			     helpEmBufEnd(0), mBufEnd(0), helpTagBufEnd(0) {
  helpEmBuf = new ElementMatching[EM_BUF_SIZE]; // new not OK yet
  mBuf = new Matching[HELP_MATCHING_BUF_SIZE];  // new not OK yet
  helpTagBuf = new ChangeableTag[HELP_TAG_BUF_SIZE];
}

void MatchingSet::DeleteBuffers() {
  if (helpEmBuf) { delete[] helpEmBuf; helpEmBuf = NULL; }
  if (mBuf) { delete[] mBuf; mBuf = NULL; }
  if (helpTagBuf) { delete[] helpTagBuf; helpTagBuf = NULL; }
}

MatchingSet::~MatchingSet() {
  DeleteBuffers();
}

void MatchingSet::Clear() {
  //  Message(MSG_STATUS, "cleaing matching set...");
  ensure(!checkMode);
  //  std::cout << "max tagBufEnd = " << max_tag_buf_end << std::endl;
  GramError::Reset();
  currSentence = NULL;
  nFound = 0;
  nCheckModeFound = 0;
  helpEmBufEnd = 0;
  mBufEnd = 0;
  helpTagBufEnd = 0;
}

ChangeableTag *MatchingSet::GetChangeableTag() {
  ensure(helpTagBufEnd < HELP_TAG_BUF_SIZE);
  helpTagBuf[helpTagBufEnd].Reset();
  return helpTagBuf + helpTagBufEnd++;
}

ElementMatching *MatchingSet::CopyHelpElementMatchings(const ElementMatching *em, const int n) {
  if (helpEmBufEnd + n >= EM_BUF_SIZE) {
    Message(MSG_WARNING, "resizing help ems");

    helpEmBuf = new ElementMatching[EM_BUF_SIZE];  // new not OK yet
    helpEmBufEnd = 0;
  }
  ElementMatching *em2 = helpEmBuf + helpEmBufEnd;
  memcpy(em2, em, n * sizeof(ElementMatching));
  helpEmBufEnd += n;
  return em2;
}

void MatchingSet::Found(Matching *m) {
  //  std::cout << "saving matching" << m << std::endl;
  if (currSentence) {
    if (currSentence != m->GetSentence()) {
      Message(MSG_ERROR, "MatchingSet::Found() called without prior call to TerminateSearch()");
      return;
    }
  } else
    currSentence = m->GetSentence();
  ensure(m->GetRuleTerm()->IsScrutinizing());
  if (xPrintMatchings) {
    if (checkMode) std::cout << tab;
    std::cout << "err:  " << m << std::endl;
    m->PrintParse();
    std::cout << std::endl;
  }
  if (checkMode)
    nCheckModeFound++;
  else {
    nFound++;
    m = Save(m, m->NElements());
    GramError *g = new GramError(m); // new ok ??
    if (!m->GetSentence()->gramError)
      m->GetSentence()->gramError = g;
    else {
      GramError *g2;
      for (g2 = m->GetSentence()->gramError; g2 && g2->next; g2 = g2->next);
      g2->next = g;
    }
    m->GetSentence()->nGramErrors++;
  }
}

Matching *MatchingSet::SaveHelp(const Matching *m) {
  if (xPrintMatchingHelpRules) {
    if (checkMode) std::cout << tab;
    std::cout << "help: " << m << std::endl;
  }
  return Save(m, m->NElements()+1);
}

Matching *MatchingSet::Save(const Matching *m, int n) {
  // std::cout << "saving help matching " << m << std::endl;
  if (mBufEnd >= HELP_MATCHING_BUF_SIZE) {
    Message(MSG_WARNING, "resizing m");
    std::cerr << std::endl << m->GetSentence() << ' ' << m->GetSentence()->NGramErrors() << std::endl;

    mBuf = new Matching[HELP_MATCHING_BUF_SIZE]; // new not OK yet
    mBufEnd = 0;
  }
  Matching *m2 = mBuf + mBufEnd;
  memcpy(m2, m, sizeof(Matching));
  mBufEnd++;
  m2->elements = CopyHelpElementMatchings(m->elements, n); // m->NElements()+1);
  return m2;
}

void MatchingSet::TerminateSearch(AbstractSentence *s) {
  //  std::cout << "terminate search";
  if (currSentence && currSentence != s) {
    Message(MSG_ERROR, "a bad thing happened");
    return;
  }
  currSentence = NULL;
  if (!checkMode) {
    GramError *prev = NULL, *nxt = NULL;
    for (GramError *g = s->gramError; g; g = nxt) {
      nxt = g->next;
      if (!g->Evaluate()) {
	delete g;
	if (prev) prev->next = nxt;
	else {
	  s->gramError = nxt;
	  // jonas s->nGramErrors--;
	  // do we really want to do nGramErrors-- ?
	  // the false alarms will be counted when checking
	  // suggestions for improvement...
	  // note: this is not the same nGramErrors as in scrutinizer.cpp
	}
      } else prev = g;
    }
    if (helpTagBufEnd > max_tag_buf_end)
      max_tag_buf_end = helpTagBufEnd;
    nFound = 0;
    mBufEnd = 0;
    helpEmBufEnd = 0;
    helpTagBufEnd = 0;
  }
}
