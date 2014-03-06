/* wordtag.cc
 * author: Johan Carlberger
 * last change: 2000-03-14
 * comments: WordTag class
 */

#include "word.h"
#include "wordlexicon.h"
#include "wordtag.h"

WordLexicon *WordTag::words;
DefObj(WordTag);

const char *WordTag::String() const {
  return GetWord()->String();
}

int WordTag::StemLen() const {
  if (NInflectRules() == 0) return -1;
  return Lemma(0)->GetWord()->StringLen() - words->Inflects().Rule(inflectRule).NCharsToRemove();
}

WordTag* WordTag::Lemma(int n) const {
  if (n == 0)
    return lemma;
  return words->GetExtraLemma(this);
}

void WordTag::GuessInflectRule() {
  ensure(inflectRule == INFLECT_NO_RULE);
  words->GuessWordTagRule(this);
}

uint WordTag::InflectRule(int n) const {
  if (n == 0)
    return inflectRule;
  return words->ExtraInflectRule(this, n);
}

WordTag *WordTag::GetForm(const Tag* t, int lemmaNumber, int ruleNumber) const {
  if (lemmaNumber == 0 && t == GetTag())
    return (WordTag*) this;
  ensure(lemmaNumber < NLemmas());
  const WordTag *lemma = Lemma(lemmaNumber);
  ensure(lemma);
  ensure(ruleNumber < lemma->NInflectRules());
  if (t->Index() == lemma->TagIndex())
    return (WordTag*) lemma;
  uint ruleIndex = lemma->InflectRule(ruleNumber);
  const WordTag *wt2 = words->GetInflectedForm(lemma->GetWord(), ruleIndex, GetTag());
  if (wt2 == this || (wt2 && wt2->GetWord()->IsNewWord()))
    return words->GetInflectedForm(lemma->GetWord(), ruleIndex, t);
  if (wt2)
    Message(MSG_MINOR_WARNING, wt2->String(), " does not seem to be the correct form");
  if (!wt2 && GetTag()->OriginalTag() != GetTag()) {
    wt2 = words->GetInflectedForm(lemma->GetWord(), ruleIndex, GetTag()->OriginalTag());
    if (wt2)
      return words->GetInflectedForm(lemma->GetWord(), ruleIndex, t);
  }
  return NULL;
}

//std::ostream& operator<<(std::ostream&, const Tag*);

void WordTag::PrintInfo(std::ostream& out) const {
  if (IsExtraLemma() || IsAddedByTagger() || IsExtraWordTag())
    out << " -";
  if (IsExtraLemma())
    out << 'x';
  if (IsAddedByTagger())
    out << 't';
  if (IsExtraWordTag())
    out << 'e';
}

void WordTag::PrintTag(std::ostream& out) const {
  out << GetTag();
}

void WordTag::Print(std::ostream& out) const {
  out << '<' << GetTag() << "> " << TagFreq() << ' ';
  if (xPrintWordInfo) {
    for (int i=0; i<NInflectRules(); i++)
      out << words->Inflects().Rule(InflectRule(i)) << ' ';
    PrintInfo(out);
  }
  if (xPrintLemma) {
    out << '(';
    for (int i=0; i<NLemmas(); i++) {
      if (i > 0) out << ' ';
      out << Lemma(i);
    }
    out << ')';
  }
  out << std::endl;
}
