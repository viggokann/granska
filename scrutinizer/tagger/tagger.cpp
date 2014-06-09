/* tagger.cc
 * author: Johan Carlberger
 * last change: 2000-05-08
 * comments: Tagger class
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
#include "tagger.h"
#include <sstream>
#include <algorithm>
#include <vector>

static const int TOKEN_BUF_CHUNK = 13000;

void Tagger::Reset() {
    Message(MSG_STATUS, "resetting tagger...");
    theText.Reset();
    WordToken::Reset();
    for (int i=0; i<nTokens; i++)
        theTokens[i].Clear();
    nTokens = 0;
    NewWords().Reset();
}

bool Tagger::Load(const char *dir) {
    Timer timer;
    if (xTakeTime) 
        timer.Start();
    if (Lexicon::LoadTags(dir)) {
        CT_CONTENT = 0;
        for (int i=0; i<Tags().Ct(); i++)
            if (Tags()[i].IsContent()) {
                contentTags[CT_CONTENT] = (Tag*) &Tags()[i];
                CT_CONTENT++;
            }
    } else {
        Message(MSG_ERROR, "tag lexicon not loaded");
        return false;
    }
    if (Lexicon::LoadWordsAndMorfs(this, dir)) {
        int i;
        for (i=0; i<N_TOKENS; i++) {
            specialWord[i] = NULL;
            specialWordToken[i].word = NULL;
        }
        Word *w = Words().Find("$.");
        ensure(w);
        specialWord[TOKEN_DELIMITER_PERIOD] = w;
        specialWordToken[TOKEN_DELIMITER_PERIOD].word = w;
        specialWordToken[TOKEN_DELIMITER_PERIOD].string = "$.";
        specialWordToken[TOKEN_DELIMITER_PERIOD].SetSelectedTag(w->GetTag(), false);
        specialWordToken[TOKEN_DELIMITER_PERIOD].trailingSpace = 1;
        w = Words().Find("$!");
        ensure(w);
        specialWord[TOKEN_DELIMITER_EXCLAMATION] = w;
        specialWordToken[TOKEN_DELIMITER_EXCLAMATION].word = w;
        specialWordToken[TOKEN_DELIMITER_EXCLAMATION].string = "$!";
        specialWordToken[TOKEN_DELIMITER_EXCLAMATION].SetSelectedTag(w->GetTag(), false);
        specialWordToken[TOKEN_DELIMITER_EXCLAMATION].trailingSpace = 1;
        w = Words().Find("$?");
        ensure(w);
        specialWord[TOKEN_DELIMITER_QUESTION] = w;
        specialWordToken[TOKEN_DELIMITER_QUESTION].word = w;
        specialWordToken[TOKEN_DELIMITER_QUESTION].string = "$?";
        specialWordToken[TOKEN_DELIMITER_QUESTION].SetSelectedTag(w->GetTag(), false);
        specialWordToken[TOKEN_DELIMITER_QUESTION].trailingSpace = 1;
        w = Words().Find("$h");
        ensure(w);
        specialWord[TOKEN_DELIMITER_HEADING] = w;
        specialWordToken[TOKEN_DELIMITER_HEADING].word = w;
        specialWordToken[TOKEN_DELIMITER_HEADING].string = "$h";
        specialWordToken[TOKEN_DELIMITER_HEADING].SetSelectedTag(w->GetTag(), false);
        specialWordToken[TOKEN_DELIMITER_HEADING].trailingSpace = 1;
        w = Words().Find("$*");
        ensure(w);
        specialWord[TOKEN_DELIMITER_OTHER] = w;
        specialWordToken[TOKEN_DELIMITER_OTHER].word = w;
        specialWordToken[TOKEN_DELIMITER_OTHER].string = "$*";
        specialWordToken[TOKEN_DELIMITER_OTHER].SetSelectedTag(w->GetTag(), false);
        specialWordToken[TOKEN_DELIMITER_OTHER].trailingSpace = 1;
        specialWord[TOKEN_CARDINAL_SIN] = Words().Find("1");
        ensure(specialWord[TOKEN_CARDINAL_SIN]);
        specialWord[TOKEN_CARDINAL] = specialWord[TOKEN_MATH] =  specialWord[TOKEN_E_MAIL] =
            specialWord[TOKEN_URL] = Words().Find("5");
        ensure(specialWord[TOKEN_CARDINAL]);
        specialWord[TOKEN_BAD_CARDINAL] = Words().Find("4711");
        ensure(specialWord[TOKEN_BAD_CARDINAL]);
        specialWord[TOKEN_ORDINAL] = Words().Find("femte");
        ensure(specialWord[TOKEN_ORDINAL]);
        specialWord[TOKEN_YEAR] = specialWord[TOKEN_DATE] =
            specialWord[TOKEN_TIME] = Words().Find("1998");
        ensure(specialWord[TOKEN_YEAR]);
        specialWord[TOKEN_PARAGRAPH] = Words().Find("7 §");
        ensure(specialWord[TOKEN_PARAGRAPH]);   
        specialWord[TOKEN_PERIOD] = Words().Find(".");
        ensure(specialWord[TOKEN_PERIOD]);
        specialWord[TOKEN_QUESTION_MARK] = Words().Find("?");
        ensure(specialWord[TOKEN_QUESTION_MARK]);
        specialWord[TOKEN_EXCLAMATION_MARK] = Words().Find("!");
        ensure(specialWord[TOKEN_EXCLAMATION_MARK]);
        specialWord[TOKEN_LEFT_PAR] = Words().Find("(");
        ensure(specialWord[TOKEN_LEFT_PAR]);
        specialWord[TOKEN_RIGHT_PAR] = Words().Find(")");
        ensure(specialWord[TOKEN_RIGHT_PAR]);
        specialWord[TOKEN_CITATION] = Words().Find("\"");
        ensure(specialWord[TOKEN_CITATION]);
        specialWord[TOKEN_PERCENTAGE] = Words().Find("17 %");
        ensure(specialWord[TOKEN_PERCENTAGE]);
        //    for (i=0; i<N_TOKENS; i++)
        //      if (!specialWord[i])
        //	std::cout << "no word for " << (Token)i << std::endl;
        gadget[0].SetTags(specialWord[TOKEN_DELIMITER_PERIOD]);
        gadget[1].SetTags(specialWord[TOKEN_DELIMITER_PERIOD]);
        if (xTakeTime) 
            loadTime = timer.Get();
        Message(MSG_STATUS, "lexicon loaded successfully");
        return true;
    } else {
        Message(MSG_ERROR, "lexicon not loaded");
        return false;
    }
}

Tagger::Tagger() : theTokens(NULL), tokensBufSize(0), theOriginalText()
{
    //  std::cout << "Word size: " << sizeof(Word) << std::endl;
    //  std::cout << "NewWord size: " << sizeof(NewWord) << std::endl;
    ensure(xMinLastChars >= MIN_LAST_CHARS);
    ensure(xMaxLastChars <= MAX_LAST_CHARS);
    ensure(xCompoundMinLength == xCompoundPrefixMinLength + xCompoundSuffixMinLength);
    if (xNNewWordVersions > MAX_WORD_VERSIONS)
        Message(MSG_ERROR, "xNNewWordVersions cannot exceed MAX_WORD_VERSIONS, program must be recompiled");
    if (xNWordVersions > MAX_WORD_VERSIONS)
        Message(MSG_ERROR, "xNWordVersions cannot exceed MAX_WORD_VERSIONS, program must be recompiled");
    if (xTaggingEquation < 19 || xTaggingEquation > 21)
        Message(MSG_ERROR, "unknown tagging equation selected, program must be recompiled");
    xSetNewWordTagFreqs = true;
    AbstractSentence::tagger = this;
    nTokens = 0;
}

Tagger::~Tagger() {    
    //delete theOriginalText;
    Message(MSG_STATUS, "deleting tagger...");
    WordToken::stringBuf.Reset();
    if (theTokens) { delete[] theTokens; theTokens = 0;}
#ifdef COUNT_OBJECTS
    WordToken::nObjects -= tokensBufSize;
#endif
}

void Tagger::TagUnknownWord(Word *w, bool normalize, bool suffixCheck,
                            const WordToken *token) {
    // computes lexProbs for all content tags based on the morphology of w
    // if token not null capped-weighths are used
    Tag::probsWord = w;
    int i;
    const uint len = w->StringLen();
    if (xNewWordsMemberTaggingOnly) {
        for (i=0; i<CT_CONTENT; i++)
            contentTags[i]->lexProb = contentTags[i]->ctm_cwt;
    } else {
        for (i=0; i<CT_CONTENT; i++)
            contentTags[i]->lexProb = (float) 1e-10;
        if (xMinLastChars < xMaxLastChars) {
            if (len > MIN_PREFIX_LENGTH) {
                uint max = (len < (xMaxLastChars+MIN_PREFIX_LENGTH)) ? 
                    len-MIN_PREFIX_LENGTH : xMaxLastChars;
                const char *string = w->String()+len-1;	
                for (uint j=xMinLastChars; j<=max; j++) {
                    for (WordTag* q=Morfs().Find(string); q; q=q->Next()) {
                        Tag *t = q->GetTag();
                        t->lexProb += q->LexProb();
                    }
                    string--;
                }
            }
        }
        if (suffixCheck) {      
            NewWord *nw = w->IsNewWord() ? (NewWord*)w : NULL;
            ensure(nw);
            // added 2001-06-25:
            for (const WordTag *wt = w; wt; wt = wt->Next())
                if (wt->TagIndex() != TAG_INDEX_NONE) {
                    wt->GetTag()->lexProb *= xAlphaSuffix;
                    if (xPrintLexicalProbs)
                        std::cout << w << ": Derived or Compound: " << wt->GetTag()
                                  << " factored by " << xAlphaSuffix << std::endl;
                }
            /* removed 2001-06-35
               Words().CompoundAnalyze(nw);
               for (int i=0; i<nw->NSuffixes(); i++) {
               WordTag *wt = nw->Suffix(i);
               wt->GetTag()->lexProb += xAlphaSuffix*wt->LexProb()*wt->GetTag()->CompoundProb();
               }
            */
            // terrible ad-hoc fix for words like 10b to avoid false scrutinizing alarms:
            if (nw->NSuffixes() == 0 && ContainsDigit(w->String()) &&
                !strchr(w->String(), '-')) {
                Tags().SpecialTag(TOKEN_PROPER_NOUN)->lexProb *= 1000;
                if (xPrintLexicalProbs)
                    std::cout << w << ": ContainsDigit, prob of " << Tags().SpecialTag(TOKEN_PROPER_NOUN)
                              << " factored by 1000" << std::endl;
            }
        }
    }
    if (token) {
        if (xMorfCapital)
            if (token->IsFirstCapped() && !token->IsFirstInSentence()) {
                Tags().SpecialTag(TOKEN_PROPER_NOUN)->lexProb *= xAlphaUnknownCapital;
                Tags().SpecialTag(TOKEN_PROPER_NOUN_GENITIVE)->lexProb *= xAlphaUnknownCapital;
                if (xPrintLexicalProbs)
                    std::cout << w << ": FirstCapped, pm-tags "
                              << Tags().SpecialTag(TOKEN_PROPER_NOUN) << " and "
                              << Tags().SpecialTag(TOKEN_PROPER_NOUN_GENITIVE)
                              << " factored by " << xAlphaUnknownCapital << std::endl;
            } else if (token->IsAllCapped() && len < 5) {
                Tags().SpecialTag(TOKEN_PROPER_NOUN)->lexProb *= 5000*xAlphaUnknownCapital;
                Tags().SpecialTag(TOKEN_PROPER_NOUN_GENITIVE)->lexProb *= 5000*xAlphaUnknownCapital;
                if (xPrintLexicalProbs)
                    std::cout << w << ": AllCapped, len<5, pm-tags factored by " << xAlphaUnknownCapital << std::endl;
            }
        if (xMorfNonCapital) 
            if (!token->IsFirstCapped()) {
                Tags().SpecialTag(TOKEN_PROPER_NOUN)->lexProb *= xAlphaUnknownNonCapital;
                Tags().SpecialTag(TOKEN_PROPER_NOUN_GENITIVE)->lexProb *= xAlphaUnknownNonCapital;
                if (xPrintLexicalProbs)
                    std::cout << w << ": NOT FirstCapped, pm-tags factored by " << xAlphaUnknownNonCapital << std::endl;
            }
    }
    if (normalize) {
        float sum = 0;
        for (i=0; i<CT_CONTENT; i++) {
            if (contentTags[i]->lexProb <= 0)
                Message(MSG_ERROR, "negative prob for", w->String()); 
            else
                sum += contentTags[i]->lexProb;
        }
        for (i=0; i<CT_CONTENT; i++)
            contentTags[i]->lexProb /= sum;
    }
}

typedef Tag* TagPointer;
const float tagLexProb(const TagPointer t) { return t->LexProb(); }

void Tagger::AddTagsToUnknownWord(NewWord *w) {
    // SelectBest((TagPointer*) contentTags, CT_CONTENT, (TagPointer*) g->tag,
    // g->n, tagLexProb);
    Tag *best[MAX_WORD_VERSIONS];
    int i, worst = -1;
    for (i=0; i<xNNewWordVersions; i++)
        best[i] = contentTags[i];
    for (; i<CT_CONTENT; i++) {
        if (worst < 0) {
            worst = 0;
            for (int j=1; j<xNNewWordVersions; j++)
                if (best[worst]->lexProb > best[j]->lexProb)
                    worst = j;
        }
        if (best[worst]->lexProb < contentTags[i]->lexProb) {
            best[worst] = contentTags[i];
            worst = -1;
        }
    }
    for (i=0; i<xNNewWordVersions; i++) {
        ensure(best[i]->lexProb > 0);
        NewWords().AddWordTagUnsafe(w, best[i])->lexProb = best[i]->lexProb;
    }
}

void Tagger::SetLexicalProbs(Word *w, const WordToken &t, TrigramGadget *g) {
    ensure(w);
    if (t.SelectedTag()) {
        g->n = 1;
        g->tag[0] = t.SelectedTag();
        g->lexProb[0] = 1;
        if (xPrintLexicalProbs)
            std::cout << "lex-probs of " << w << ": " << g->tag[0] << " 1 (selected)" << std::endl;
        return;
    }
    if (w->IsNewWord()) {
        NewWord *nw = (NewWord*) w;
        if (xAnalyzeNewWords && !nw->IsAnalyzed()) {
            Message(MSG_MINOR_WARNING, "new-word", nw->String(), "not analyzed before tagging");
            Words().AnalyzeNewWord(nw);
        }
        TagUnknownWord(nw, false || xAlwaysNormalizeNewWords, true, &t);
        g->n = xNNewWordVersions;
        //     SelectBest((TagPointer*) contentTags, CT_CONTENT, (TagPointer*) g->tag, g->n, tagLexProb);    
        int i;
        for (i=0; i<g->n; i++)
            g->tag[i] = contentTags[i];
        int worst = -1;
        for (; i<CT_CONTENT; i++) {
            if (worst < 0) {
                worst = 0;
                for (int j=1; j<g->n; j++)
                    if (g->tag[worst]->lexProb > g->tag[j]->lexProb)
                        worst = j;
            }
            if (g->tag[worst]->lexProb < contentTags[i]->lexProb) {
                g->tag[worst] = contentTags[i];
                worst = -1;
            }
        }
        for (i=0; i<g->n; i++)
            g->lexProb[i] = g->tag[i]->lexProb;
        if (xPrintLexicalProbs) {
            std::cout << "lex-probs of new word " << w << ':' << std::endl;
            for (i=0; i<g->n; i++)
                std::cout << tab << g->tag[i] << ' ' << g->lexProb[i] << std::endl;
            std::cout << std::endl;
        }
        return;
    }
    int i = 0, max = w->IsNewWord() ? xNNewWordVersions : xNWordVersions;
    for (WordTag *q=w; q && i < max; q=q->Next()) {
        if (!q->IsExtraLemma()) {
            Tag *tg = q->GetTag();
            ensure(tg);
            g->tag[i] = tg;
            g->lexProb[i] = q->LexProb();
            if (!w->IsNewWord() && tg->IsProperNoun())
                if (t.IsFirstCapped()) { 
                    if (!t.IsFirstInSentence() && xMorfCapital) {
                        if (xPrintLexicalProbs)
                            std::cout << w << ": FirstCapped, prob of " << tg << " factored by "
                                      <<  xAlphaCapital << std::endl;
                        g->lexProb[i] *= xAlphaCapital;
                    }
                } else if (xMorfNonCapital) {
                    g->lexProb[i] *= xAlphaNonCapital;
                    if (xPrintLexicalProbs)
                        std::cout << w << ": NOT First Capped, prob of " << tg << " factored by "
                                  << xAlphaNonCapital << std::endl;
                }
            if (g->lexProb[i] <= 0)
                Message(MSG_WARNING, w->String(), "has non-positive prob for tag",
                        q->GetTag()->String());
            //      std::cout << q << ' ' << g->lexProb[i] << std::endl;
            i++;
        }
    }
    ensure(i > 0);
    if (t.IsAllCapped() && w->StringLen() < 6 && i<max &&
        !w->GetWordTag(Tags().SpecialTag(TOKEN_PROPER_NOUN))) {
        if (xPrintLexicalProbs) std::cout << "(AllCapped -> pm added)"; 
        g->tag[i] = Tags().SpecialTag(TOKEN_PROPER_NOUN);
        g->lexProb[i] = 0.1f * Tags().SpecialTag(TOKEN_PROPER_NOUN)->FreqInv();
        i++;
    }
    g->n = i;
  
    if (xPrintLexicalProbs) {
        std::cout << "lex-probs of known word " << w << ':' << std::endl;
        for (i=0; i<g->n; i++)
            std::cout << tab << g->tag[i] << ' ' << g->lexProb[i] << std::endl;
        std::cout << std::endl;
    }
    // test Åström exponent:
    //  for (i=0; i<g->n; i++) g->lexProb[i] = pow(g->lexProb[i], 0.9);
}

#ifdef PREDICTOR

#include "wordsuggestions.hh"

void Tagger::TagSentenceForPredictor(Sentence *s, Word **ws, int n, WordSuggestions &sugg, uint minLength, float factor) {
    int endPos = s->NWords();
    int lastPos = s->NWords() - 1;
    ensure(lastPos >= 0);
    ensure(n > 0);
    s->tokens[lastPos].GetToken() = WORD;
    s->tokens[lastPos].word = ws[0];
    TagSentenceInterval(s, MaxOf(0, lastPos-1), endPos);
    for (int i=0; i<n; i++) {
        Word *w = ws[i];
        if (!w->IsSuggested()) {
            s->tokens[lastPos].word = w;
            TagSentenceInterval(s, lastPos, endPos);
            probType p = 0;
            for (int j=0; j<gadget[endPos+1].n; j++)
                for (int k=0; k<gadget[endPos].n; k++)
                    p += gadget[endPos+1].prob[j][k];
            if (factor > 0)
                p *= factor;
            if (xRecency)
                if (w->TextFreq())
                    p *= (1 + xRecencyFactor*w->TextFreq()/w->Freq());
                else {
                    const Word *b = w->LemmaWord();
                    if (b && b->SomeFormOccursInText())
                        p *= (1 + xRecencyFactorBase/w->Freq());
                }
            if (xWordBigramsUsed) {
                Word *prev = (lastPos ? s->GetWord(lastPos-1) : sentenceDelimiter);
                Bigram *b = Words().FindBigram(prev, w);
                if (b)
                    p *= (1 + xLambdaWordBi*b->prob);
                if (!b && prev->Freq() >= 80)
                    p *= (1 + xLambdaWordBi*xNewParameter/prev->Freq());
            }
            if (p > sugg.WorstProb())
                if (w->StringLen() >= minLength)
                    sugg.AddCandidate(w, p);
        }
    }
}

#endif

void Tagger::TagSentenceInterval(AbstractSentence *s, int startPos, int endPos) {
    ensure(startPos >= 0);
    ensure(endPos < s->NTokens());
    int i=startPos;
    TrigramGadget *g0 = gadget+i;
    i++;
    TrigramGadget *g1 = gadget+i;
    //  Word *w1 = s->GetWord(i);
    i++;
    TrigramGadget *g2 = gadget+i;
    for (; i<=endPos; i++, g0++, g1++, g2++) {
        WordToken *t = s->GetWordToken(i);
        //    g2->Reset();                   // make sure this has no effect on the tagging // jonas, this crashes the tagger, why?
        SetLexicalProbs(t->GetWord(), *t, g2);
        // computes the lexical probabilities of w2 and assigns the possible tags of w2 to g2.  
        for (int u=0; u<g2->n; u++) {
            const Tag *tag2 = g2->tag[u];
            for (int v=0; v<g1->n; v++) {
                const Tag *tag1 = g1->tag[v];
                //std::cout << "Pt1t2(" << tag1 << ',' << tag2 << ") = " << Tags().Pt1t2(tag1->Index(), tag2->Index()) << std::endl;
                probType best = 0.0;	  
                for (int z=0; z<g0->n; z++) {
                    const Tag *tag0 = g0->tag[z];
                    probType prob = g1->prob[v][z]*(Tags().Pt3_t1t2(tag0,tag1,tag2)); //+biProb);//jonas, this sometimes gives UMR-error
                    if (prob > best) {
                        best = prob;
                        g2->prev[u][v] = (char) z; // remember which tag in position 0 that gave the best probability
                    }
                }
                g2->prob[u][v] = best*g2->lexProb[u];
            }
        }
        if (g2->prob[0][0] < MIN_PROB) {
            if (g2->prob[0][0] <= 0) {
                //s->Print();
                Message(MSG_WARNING, "prob is zero during tagging at", t->GetWord()->String(),
                        g2->tag[0]->String());
            }
            Message(MSG_MINOR_WARNING, "prob too small, normalizing");
            //std::cerr << "normalize, too small, index " << i << std::endl;
            g2->Normalize(g2->n, g1->n);
        } else if (g2->prob[0][0] > MAX_PROB) {
            Message(MSG_MINOR_WARNING, "prob too big, normalizing");
            //std::cerr << "normalize, too large, index " << i << std::endl;
            g2->Normalize(g2->n, g1->n);
        }
        //    w1 = t->GetWord();
    }
    if (endPos >= s->NWords()+2)
        Rewind(s, endPos);
}

void Tagger::Rewind(AbstractSentence *s, int endPos) {
    s->prob = gadget[endPos+1].prob[0][0];
    TrigramGadget *g = gadget;
    int x = 0;
    int y = 0;
    int prevY;
    for (int i=endPos-2; i>=2; i--) {
        WordToken *t = s->GetWordToken(i);
        Word *old_word = t->GetWord(); // jonas
        prevY = y;
        y = g[i+2].prev[x][y];
        /*
          if(std::string("bye-bye") == t->GetWord()->string) {
          std::cerr << t << std::endl;
          }
        */
        //    std::cerr << t->GetWord()->GetWordTag(g[i].tag[y]) << "\t" << g[i].prob[x][y] / g[i-1].prob[y][g[i+1].prev[x][y]] << std::endl; // jonas, test of output probabilities

        g[i].selected = y;
        ensure(y >= 0);
        x = prevY;
        Tag *tag = g[i].tag[y];
        ensure(tag);
        WordTag *wt = t->GetWord()->GetWordTag(tag);
        if (!wt) {
            if (!t->GetWord()->IsNewWord()) {
                Message(MSG_MINOR_WARNING, "the tagger selected unknown tag",
                        tag->String(), "for main lexicon word", t->GetWord()->String());
                wt = t->word = NewWords().AddWord(t->GetWord()->String(), tag);
            } else {
                wt = NewWords().AddWordTag((NewWord*)t->GetWord(), tag);
                Words().GuessWordTagRule(wt);
                if (tag->IsLemma())
                    wt->lemma = wt;
                wt->addedByTagger = 1;
            }
        }
        if (t->GetWord()->IsNewWord() && (xSetNewWordTagFreqs || wt->tagFreq==0))
            wt->tagFreq++;
        t->SetSelectedTag(tag);
        if(t->GetWord()->IsNewWord() // jonas
           && t->SelectedTag() != tag) // jonas, tagger chose unallowed tag, use old word instead
            t->word = old_word; // jonas
        /*
          if(std::string("bye-bye") == t->GetWord()->string) {
          std::cerr << t->SelectedTag() << std::endl;
          }
        */
    }
    if(xPrintProbs) {
        /* estimate probability of chosen tag. count how much of the 
           total Markov-probability would give this choice.
        */
        for(int i = 2; i <= endPos -2; ++i) {
            probType current = g[i].prob[g[i].selected][g[i-1].selected];
            probType previous = g[i-1].prob[g[i-1].selected][g[i-2].selected];
            probType a = current / previous / std::max(s->GetWordToken(i)->GetWord()->Freq(), 1);
            probType sumProbs = 0, sumU = 0;
            for(int vv = 0; vv < g[i-1].n; ++vv){ 
                for(int uu = 0; uu < g[i].n; ++uu)
                    sumProbs += g[i].prob[uu][vv] / g[i-1].prob[vv][g[i].prev[uu][vv]];
                sumU += g[i].prob[g[i].selected][vv] / g[i-1].prob[vv][g[i].prev[g[i].selected][vv]];
            }
            a = sumU / sumProbs;
            std::cout << a ;
            std::cout << "\t" << s->GetWordToken(i);
        }
    }
}

void Tagger::TagText() {
    if (!xOptimize) Message(MSG_STATUS, "tagging text...");
    Timer timer;
    if (xTakeTime) 
        timer.Start();
    if (xAnalyzeNewWords)
        Words().AnalyzeNewWords();
    if (xTakeTime) 
        analyzeTime = timer.Restart();
    int i;
    for (Sentence *s=theText.FirstSentence(); s; s = s->Next())
        TagSentence(s);
    if (!xAmbiguousNewWords)
        for (Sentence *s=theText.FirstSentence(); s; s=s->Next()) {
            for (i=2; i<s->NWords()+2; i++) {
                WordToken *wt = s->GetWordToken(i);
                Word *w = wt->GetWord();
                if (w->IsNewWord()) {
                    const WordTag *wm = w->GetWordTag(wt->SelectedTag());
                    if (wm) {
                        int best = wm->TagFreq();
                        for (WordTag *q=w; q; q=q->Next())
                            if (q->TagFreq() > best) {
                                wt->SetSelectedTag(q->GetTag());
                                best = q->TagFreq();
                            }
                    } else {
                        if (!wt->SelectedTag())
                            Message(MSG_ERROR, "TagText(): no selected tag for", w->String());
                        Message(MSG_ERROR, "TagText(): unexpected tag", wt->SelectedTag()->String(),
                                "for", w->String());
                    }
                }
            }
        }
    if (xTakeTime) 
        tagTime = timer.Get();
}

/*
  jonas, 
  moved ifdef CORRECT_TAG_KNOWN -> Tagger::EvaluateTagging() 
  to developer-tagger.cpp
*/

void Tagger::ReadText() {
    Reset();
    Message(MSG_STATUS, "reading tokens...");
    Timer timer;
    if (xTakeTime) 
        timer.Start();
    //  char string[MAX_WORD_LENGTH];
    char lookUpString[MAX_WORD_LENGTH];
    int offset = 0, prevOffset;
    if (tokensBufSize == 0) {
        if(input_size > 0) { // jonas, this part is new
            tokensBufSize = 1000 + input_size / 4; // guess words approx 4 chars
            std::cerr << "allocated space for " << tokensBufSize << " tokens " << std::endl;
            theTokens = new WordToken[tokensBufSize];
        } else { // jonas, this is the old stuff
            theTokens = new WordToken[TOKEN_BUF_CHUNK]; // new OK
            tokensBufSize = TOKEN_BUF_CHUNK;
        }
    }
    theTokens[0].SetWord(specialWord[TOKEN_DELIMITER_PERIOD], "$.", TOKEN_END);
    nTokens = 1;
    for (;;) {
        //if(nTokens % 100000 == 1)
        //  std::cerr << "tokens read: " << nTokens << std::endl;
        if (nTokens >= tokensBufSize) {
            tokensBufSize *= 2;      //jonas      tokensBufSize += TOKEN_BUF_CHUNK;
            WordToken *tok = new WordToken[tokensBufSize]; // new OK
            memcpy(tok, theTokens, nTokens * sizeof(WordToken));
#ifdef COUNT_OBJECTS
            WordToken::nObjects -= nTokens;
#endif
            delete [] theTokens;	// jbfix: delete p --> delete [] p
            theTokens = tok;
            std::cerr << "REallocated space for " << tokensBufSize << " tokens" << std::endl;
        }
        Word *w = NULL;
        const char *lookUp = NULL;
        Token token = tokenizer.Parse();
        theOriginalText << tokenizer.TokenString();
        prevOffset = offset;
        offset += tokenizer.TokenLength();
        switch(token) {
        case TOKEN_END:
            goto endText;
        case TOKEN_SPLIT_WORD: { // "hälso- och sjukvård"
            const char *string = tokenizer.TokenString();
            for (int j=tokenizer.TokenLength()-1; j>0; j--)
                if (IsSpace(string[j])) {
                    lookUp = string+j+1;
                    break;
                }
            ensure(lookUp);
            break;
        }
        case TOKEN_ABBREVIATION:
        case TOKEN_WORD: {
            const char *string = tokenizer.TokenString();
            w = FindMainOrNewWord(string);
            if (!w) {
                strcpy(lookUpString, string);
                SpaceFix(lookUpString);
                w = FindMainOrNewWord(lookUpString);
                if (!w) {
                    if (token == TOKEN_ABBREVIATION)
                        Space2Punct(lookUpString);
                    else
                        PunctFix(lookUpString);
                    w = FindMainOrNewWord(lookUpString);
                }
            }
            break;
        }
        case TOKEN_PUNCTUATION:
        case TOKEN_SIMPLE_WORD:
            break;
        case TOKEN_LEFT_PAR:
        case TOKEN_RIGHT_PAR:
        case TOKEN_CITATION:
        case TOKEN_CARDINAL_SIN:
        case TOKEN_CARDINAL:
        case TOKEN_BAD_CARDINAL:
        case TOKEN_ORDINAL:
        case TOKEN_YEAR:
        case TOKEN_DATE:
        case TOKEN_TIME:
        case TOKEN_PARAGRAPH:
        case TOKEN_PERIOD:
        case TOKEN_QUESTION_MARK:
        case TOKEN_DELIMITER_PERIOD:
        case TOKEN_DELIMITER_QUESTION:
        case TOKEN_DELIMITER_EXCLAMATION:
        case TOKEN_DELIMITER_HEADING:
        case TOKEN_DELIMITER_OTHER:
        case TOKEN_PERCENTAGE:
        case TOKEN_MATH:
        case TOKEN_E_MAIL:
        case TOKEN_URL:
            w = specialWord[token];
            if(!Tags().SpecialTag(token)) {
                std::cerr << token << std::endl;
            }
            ensure(Tags().SpecialTag(token));
            theTokens[nTokens].SetSelectedTag(Tags().SpecialTag(token), false);
            ensure(w);
            break;
        case TOKEN_EXCLAMATION_MARK:
            w = specialWord[token];
            break;
        case TOKEN_NEWLINE:
            if (!xNewlineMeansNewSentence) {
                theTokens[nTokens-1].trailingSpace = 1;
                continue;
            }
        case TOKEN_BEGIN_PARAGRAPH:
        case TOKEN_BEGIN_HEADING:
        case TOKEN_END_HEADING:
        case TOKEN_BEGIN_TITLE:
        case TOKEN_END_TITLE:
            if (theTokens[nTokens-1].token == TOKEN_NEWLINE)
                nTokens--;
            theTokens[nTokens++].SetWord(specialWord[TOKEN_DELIMITER_HEADING], NULL, token);
            continue;
        case TOKEN_BEGIN_TABLE:
        case TOKEN_TABLE_TAB:
        case TOKEN_END_TABLE:
            continue;
        case TOKEN_SPACE:
            theTokens[nTokens-1].trailingSpace = 1;
            continue;
        case TOKEN_PROPER_NOUN:
        case TOKEN_PROPER_NOUN_GENITIVE:
        case TOKEN_UNKNOWN:
        case TOKEN_SILLY:
        case TOKEN_ERROR:
            Message(MSG_WARNING, int2str((uchar)*tokenizer.TokenString()), "was recognized as",
                    Token2String(token), "by the tokenizer");
            break;
        } 
        const char *string = tokenizer.TokenString();
        if (!w) {
            w = FindMainOrNewWordAndAddIfNotPresent(lookUp ? lookUp : string);
            if (token == TOKEN_PUNCTUATION)
                theTokens[nTokens].SetSelectedTag(Tags().SpecialTag(token), false);
        }
        if (w->IsNewWord()) {
            NewWord *nw = (NewWord*) w;
            nw->freq++;
        }
        w->textFreq++;
        WordToken &t = theTokens[nTokens++];
        t.offset = prevOffset;
        t.SetWord(w, string, token);
    }
 endText:    
    theTokens[nTokens].SetWord(specialWord[TOKEN_DELIMITER_PERIOD], "$.", TOKEN_END);
    theTokens[nTokens+1].SetWord(specialWord[TOKEN_DELIMITER_PERIOD], "$.", TOKEN_END);
    // for(int i = 0; i < nTokens; ++i)  std::cout<< theTokens[i] << std::endl;  // jonas, debug
    if (xTakeTime) 
        tokenizeTime = timer.Restart();
    if(nTokens > 1) // jonas
        BuildSentences(theTokens);
    if (xTakeTime) 
        sentenceTime = timer.Restart();
}

enum AbbrCheck {
    W_DOT = 1,
    W_NO_DOT = 2,
    CAPPED = 4,
    MAY_CAP = 8,
    DIGIT = 16
};

void Tagger::BuildSentences(WordToken *tokens) {
    std::string theOriginalString = theOriginalText.str();
    int start = 1;
    int end = 0;
    int nCit = 0;
    int nOpenPar = 0;
    int firstPeriodPos = 0;
    int periods = 0;
    int checkedUntil = 0;
    bool punktLista = false;
    //  int nChecked = 0, nFound = 0;
    Token delToken = TOKEN_DELIMITER_OTHER;
    if(theText.firstSentence!=NULL) theText.delete_sentences();
    Message(MSG_STATUS, "building sentences...");
    theText.firstSentence = NULL;
    Sentence *s = NULL;
    char string[MAX_WORD_LENGTH], lookUpString[MAX_WORD_LENGTH];
    for (int j=1;; j++) {
        //    std::cout << tokens[j] << std::endl;
        if (j - end >= MAX_SENTENCE_LENGTH -5) { // this if-statement fixes bug for long sentences
            // sentence is too long, what to do...
            end = j; //jonas, otherwise one word disappears... end = j-1; 

        } else {
            //   else
            switch (tokens[j].token) {
            case TOKEN_DELIMITER_HEADING:
            case TOKEN_END_HEADING:
            case TOKEN_BEGIN_TITLE:
            case TOKEN_END_TITLE:
                delToken = TOKEN_DELIMITER_HEADING;
            case TOKEN_BEGIN_HEADING:
            case TOKEN_NEWLINE:
            case TOKEN_BEGIN_PARAGRAPH:
                if (s)
                    s->endsParagraph = 1;
                end = j-1;
                break;
            case TOKEN_WORD:
            case TOKEN_SIMPLE_WORD:
                if (!xNoCollocations) { // jonas
                    if (j > checkedUntil) {
                        ensure(tokens[j].GetWord() && tokens[j+1].GetWord() && tokens[j+2].GetWord());
                        if (tokens[j].GetWord()->IsCollocation1() &&
                            tokens[j+1].GetWord()->IsCollocation23()) {
                            //	  nChecked++;
                            if (tokens[j+2].GetWord()->IsCollocation23()) {	    
                                sprintf(string, "%s %s %s", tokens[j].RealString(), tokens[j+1].RealString(),
                                        tokens[j+2].RealString());
                                Word *w = FindMainWord(string);
                                if (w) {
                                    if (w->IsOptSpace()) {
                                        sprintf(lookUpString, "%s%s%s", tokens[j].RealString(),
                                                tokens[j+1].RealString(),
                                                tokens[j+2].RealString());
                                        w = FindMainWord(lookUpString);
                                        ensure(w);
                                    }
                                    tokens[j].SetWord(w, string, TOKEN_WORD);
                                    tokens[j+1].word = tokens[j+2].word = NULL;
                                    j+=2;
                                    checkedUntil = j;
                                    //	      nFound++;
                                    continue;
                                }
                            }
                            sprintf(string, "%s %s", tokens[j].RealString(), tokens[j+1].RealString());
                            Word *w = FindMainWord(string);
                            if (w) {
                                if (w->IsOptSpace()) {
                                    sprintf(lookUpString, "%s%s", tokens[j].RealString(),
                                            tokens[j+1].RealString());
                                    w = FindMainWord(lookUpString);
                                    ensure(w);
                                }
                                tokens[j].SetWord(w, string, TOKEN_WORD);
                                tokens[++j].word = NULL;
                                //	    nFound++;
                            }
                        }
                        checkedUntil = j;
                    }
                }
                continue;
            case TOKEN_QUESTION_MARK:
                delToken = TOKEN_DELIMITER_QUESTION;
                goto there;
            case TOKEN_EXCLAMATION_MARK:
                delToken = TOKEN_DELIMITER_EXCLAMATION;
                goto there;
            case TOKEN_PERIOD:
                if (tokens[j-1].GetWord() && tokens[j-1].token == TOKEN_SIMPLE_WORD &&
                    tokens[j+1].token != TOKEN_BEGIN_PARAGRAPH && 
                    tokens[j+1].token != TOKEN_END && tokens[j].RealString()[1] == '\0') {
                    sprintf(string, "%s.", tokens[j-1].LexString());
                    Word *w1 = tokens[j-1].GetWord();
                    Word *w2 = Words().Find(string);
                    int check = 0;
                    if (w2) {
                        check = W_DOT;    // e.g. "etc." is a word in lexicon
                        //std::cout << "W_DOT " << w2 << ' ';
                    }
                    if (!tokens[j-1].GetWord()->IsNewWord()) {
                        check |= W_NO_DOT;     // e.g. "sak" is not a known word in lexicon
                        //std::cout << "W_NO_DOT ";
                    }
                    if (tokens[j+1].IsFirstCapped()) {
                        check |= CAPPED;
                        //std::cout << "CAPPED ";
                    } else if (IsDigit(tokens[j+1].RealString()[0])) {
                        check |= DIGIT;
                        //std::cout << "DIGIT ";
                    }
                    if (tokens[j+1].GetWord()->MayBeCapped()) {
                        check |= MAY_CAP;
                        //std::cout << "MAY_CAP ";
                    }
                    //std::cout << std::endl;
                    bool abb = false, period = true;
                    switch (check) {
                    case 0: if (!IsLetter(tokens[j+1].RealString()[0])) break;
                    case DIGIT:
                    case W_DOT | MAY_CAP:
                    case W_DOT | W_NO_DOT: // check this case
                    case W_DOT | W_NO_DOT | MAY_CAP:
                    case W_DOT | W_NO_DOT | CAPPED | MAY_CAP: // added 2001-06-27, OK?
                    case W_DOT | W_NO_DOT | DIGIT:
                        abb = true; period = false; break;
                    case W_DOT | W_NO_DOT | CAPPED:
                        if (w2->Freq() > w1->Freq()) abb = true;
                        break;
                    }
                    //std::cout << tokens[j-1] << tokens[j] << tokens[j+1]
                    //     << check << ' ' << (abb ? "ABB " : "") << (period ? "PERIOD " : "") << std::endl << std::endl;
                    if (abb)
                        tokens[j-1].SetWord(w2 ? w2 : w1,
                                            period ? tokens[j-1].RealString() : string,
                                            TOKEN_ABBREVIATION);
                    if (!period) {
                        tokens[j].word = NULL;
                        continue;
                    }
                }
                if (tokens[j].RealString()[1] == '.' && !tokens[j+1].IsFirstCapped())
                    continue;
                delToken = TOKEN_DELIMITER_PERIOD;
            there:
                if (nCit%2 == 0 && nOpenPar == 0) {
                    end = j; break;
                }
                if (!periods) {
                    firstPeriodPos = j;
                    if (nOpenPar == 1 && tokens[start].token == TOKEN_LEFT_PAR
                        && tokens[j+1].token == TOKEN_RIGHT_PAR) {
                        j = end = start; break;
                    }
                    if (nCit%2 && tokens[start].token == TOKEN_CITATION
                        && tokens[j+1].token == TOKEN_CITATION) {
                        j = end = start; break;
                    }
                }
                if (++periods > 1) {
                    if (tokens[start].token == TOKEN_CITATION ||
                        tokens[start].token == TOKEN_LEFT_PAR)
                        j = end = start;
                    else
                        j = end = firstPeriodPos;
                    break;
                }
                continue;
            case TOKEN_END:
                end = j-1;
                break;
            case TOKEN_CITATION:
                nCit++;
                continue;
            case TOKEN_LEFT_PAR:
                nOpenPar++;
                continue;
            case TOKEN_RIGHT_PAR:
                if (nOpenPar > 0) nOpenPar--;
                else if (j==start) {
                    end = j; break;
                }
                continue;
            case TOKEN_ABBREVIATION:
                if (tokens[j+1].IsFirstCapped() &&
                    !tokens[j+1].GetWord()->MayBeCapped() &&
                    tokens[j].RealString()[strlen(tokens[j].RealString())-1] == '.') {
                    end = j; break;
                }
            default:
                continue;
            }
        }
        int n = end + 5 - start;
        if (n > 4) {
            if (s)
                s = s->next = new Sentence(n); // new OK
            else
                theText.firstSentence = s =  new Sentence(n); // new OK
            ensure(n < MAX_SENTENCE_LENGTH);
            ensure(s);
            short mm = 0;
            for (int i=start; i<=end; i++)
                if (tokens[i].word) {
                    ensure(tokens[i].token != TOKEN_BEGIN_HEADING);
                    ensure(tokens[i].token != TOKEN_END_HEADING);
                    s->tokens[2+mm++] = tokens + i;
                }
            ensure(mm>0);
            const short m = mm;
            s->nTokens = m + 4;
            s->nWords = m;
            /* jonas, why is this here?
               if (m > 1 && s->tokens[3]->IsFirstCapped() &&
               (s->tokens[2]->token == TOKEN_BAD_CARDINAL ||
               s->tokens[2]->token == TOKEN_CARDINAL ||
               s->tokens[2]->token == TOKEN_CARDINAL_SIN)) {
               s->tokens[2]->token = TOKEN_RIGHT_PAR;
               s->tokens[2]->word = specialWord[TOKEN_RIGHT_PAR];
               }
            */
            unsigned int startOffset = 0;
            unsigned int endOffset = 0;
            for (int k=2; k<m+2; k++)
                if (s->tokens[k]->GetToken() != TOKEN_PUNCTUATION &&
                    s->tokens[k]->GetToken() != TOKEN_CITATION &&
                    s->tokens[k]->GetToken() != TOKEN_RIGHT_PAR) {
                    s->tokens[k]->firstInSentence = 1;
                    startOffset = s->tokens[k]->Offset(); //Oscar
                    break;
                }
            WordToken *del = &specialWordToken[delToken];
            if (s->tokens[2]->GetToken() == TOKEN_PUNCTUATION ||
                s->tokens[2]->GetToken() == TOKEN_RIGHT_PAR) {
                if (punktLista)
                    del = &specialWordToken[TOKEN_DELIMITER_OTHER];
            } else {
                if (s->tokens[m+1]->GetToken() == TOKEN_PUNCTUATION &&
                    !strcmp(":", s->GetWord(m+1)->String()))
                    punktLista = true;
                else
                    punktLista = false;
            }
            endOffset = s->tokens[m+1]->Offset(); //Oscar
            s->setOriginalText(theOriginalString.substr(startOffset,endOffset-startOffset+1)); //Oscar
            ensure(del->word);
            s->tokens[0] = s->tokens[1] = 
                s->tokens[m+2] = s->tokens[m+3] = del;
        }        
        start = j+1;
        if (start >= nTokens)
            break;
        nCit = 0;
        nOpenPar = 0;
        periods = 0;
        firstPeriodPos = 0;
        delToken = TOKEN_DELIMITER_OTHER;
    } 
    s->next = NULL;
    //  std::cout << "nChecked: " << nChecked << " nFound: "<< nFound << std::endl;
}

void Tagger::GenerateInflections() {
    Message(MSG_STATUS, "generating inflections...");
    Words().GenerateInflections();
    Message(MSG_COUNTS, "during inflection generating");
}

void Tagger::PrintTimes() {
    theText.CountContents();
    std::cout << theText;
    std::cout << std::endl << "timings:" << std::endl
              << tab << loadTime / (double)Timer::clocks_per_sec() * 1000000.0 //jonas, switched * and /
              << " ms to load lexicon" << std::endl
              << tab << (double)Timer::clocks_per_sec() * theText.NWordTokens()/tokenizeTime 
              << " words tokenized per second" << std::endl
              << tab << (double)Timer::clocks_per_sec() * theText.NWordTokens()/sentenceTime 
              << " words sentenized per second" << std::endl
              << tab << (double)Timer::clocks_per_sec() * theText.NWordTokens()/analyzeTime 
              << " words analyzed per second" << std::endl
              << tab << (double)Timer::clocks_per_sec() * theText.NWordTokens()/tagTime 
              << " words tagged per second" << std::endl
              << tab << (double)Timer::clocks_per_sec() * theText.NWordTokens()/
        (tokenizeTime + tagTime + analyzeTime + sentenceTime) 
              << " words tokenized and tagged per second" << std::endl;
}




// warning, quick, dirty, ugly, etc. // jonas

// read to two buffers ?
// use tokenizer, use every odd as tag ?

inline int my_tolower(const int i) {
    if(i >= 'A' && i <= 'Z')
        return i - 'A' + 'a';
    if(i == 'Å')
        return 'å';
    if(i == 'Ä')
        return 'ä';
    if(i == 'Ö')
        return 'ö';
    else return tolower(i);
}
inline int next_index(std::vector<std::string> &v,
                      const char *s) {
    // very naive sync, find next index were word is 's'
    static std::string tt ="";
    static int start = 0;
  
    int i;
    for(i=0; i< (int) strlen(s); i++)
        if(s[i] != ' ')
            tt += s[i];
    std::string temp = "";
    for(i=start; i < (int) v.size() && temp.length() < tt.length(); i++)
        temp += v[i];
    std::transform(tt.begin(),tt.end(),tt.begin(),my_tolower); // takes very little time
    if(temp == tt) {
        tt = "";
        start = i;
        return i-1;
    } else
        return -1;
}

//handles tokenization errors badly, but sometimes succeds

void Tagger::ReadTaggedTextQnD() {
    std::ostringstream oss;
    std::vector<Tag *> tagvec;
    std::vector<std::string> wordvec;
    tagvec.reserve(1000 + input_size / 10);
    wordvec.reserve(1000 + input_size / 10);
    while(*inputStream) {
        std::string l;
        std::getline(*inputStream, l);
        std::cerr << "found '" << l << "'" << std::endl;
        if(l != "") {
            std::string w,t;
            std::istringstream isl(l);
            isl >> w >> t;
            oss << w << "\n";
            Tag *ct = Tags().Find(t.c_str());
            if (!ct) {
                Message(MSG_WARNING, "unknown tag for", w.c_str(), t.c_str());
                ct = Tags().DummyTag(); // this is not very good
            }
            std::transform(w.begin(),w.end(),w.begin(),my_tolower); // takes very little time
            wordvec.push_back(w);
            tagvec.push_back(ct);
        } else 
            oss << "\n"; // keep double newlines
    }
    std::istringstream iss(oss.str());
    tokenizer.SetStream(&iss);

    // now do the usual, but add tags... harder than one might think

    Reset();
    Message(MSG_STATUS, "reading tokens...");
    Timer timer;
    if (xTakeTime) 
        timer.Start();
    char lookUpString[MAX_WORD_LENGTH];
    int offset = 0, prevOffset;
    if (tokensBufSize == 0) {
        if(input_size > 0) { // jonas, this part is new
            tokensBufSize = 1000 + input_size / 10; // guess words approx 4 chars, tags approx 6
            std::cerr << "allocated space for " << tokensBufSize << " tokens " << std::endl;
            theTokens = new WordToken[tokensBufSize];
        } else { // jonas, this is the old stuff
            theTokens = new WordToken[TOKEN_BUF_CHUNK];
            tokensBufSize = TOKEN_BUF_CHUNK;
        }
    }
    theTokens[0].SetWord(specialWord[TOKEN_DELIMITER_PERIOD], "$.", TOKEN_END);
    nTokens = 1;
    for (;;) {
        if (nTokens >= tokensBufSize) {
            tokensBufSize *= 2;
            WordToken *tok = new WordToken[tokensBufSize];
            memcpy(tok, theTokens, nTokens * sizeof(WordToken));
#ifdef COUNT_OBJECTS
            WordToken::nObjects -= nTokens;
#endif
            delete [] theTokens;
            theTokens = tok;
            std::cerr << "REallocated space for " << tokensBufSize << " tokens" << std::endl;
        }
        Word *w = NULL;
        const char *lookUp = NULL;
        Token token = tokenizer.Parse();
        prevOffset = offset;
        offset += tokenizer.TokenLength();
        switch(token) {
        case TOKEN_END:
            goto endText;
        case TOKEN_SPLIT_WORD: { // "hälso- och sjukvård"
            const char *string = tokenizer.TokenString();
            for (int j=tokenizer.TokenLength()-1; j>0; j--)
                if (IsSpace(string[j])) {
                    lookUp = string+j+1;
                    break;
                }
            ensure(lookUp);
            break;
        }
        case TOKEN_ABBREVIATION: // these don't work ok....
        case TOKEN_WORD: {
            const char *string = tokenizer.TokenString();
            w = FindMainOrNewWord(string);
            if (!w) {
                strcpy(lookUpString, string);
                SpaceFix(lookUpString);
                w = FindMainOrNewWord(lookUpString);
                if (!w) {
                    if (token == TOKEN_ABBREVIATION)
                        Space2Punct(lookUpString);
                    else
                        PunctFix(lookUpString);
                    w = FindMainOrNewWord(lookUpString);
                }
            }
            break;
        }
        case TOKEN_PUNCTUATION:
        case TOKEN_SIMPLE_WORD:
            break;
        case TOKEN_LEFT_PAR:
        case TOKEN_RIGHT_PAR:
        case TOKEN_CITATION:
        case TOKEN_CARDINAL_SIN:
        case TOKEN_CARDINAL:
        case TOKEN_BAD_CARDINAL:
        case TOKEN_ORDINAL:
        case TOKEN_YEAR:
        case TOKEN_DATE:
        case TOKEN_TIME:
        case TOKEN_PARAGRAPH:
        case TOKEN_PERIOD:
        case TOKEN_QUESTION_MARK:
        case TOKEN_DELIMITER_PERIOD:
        case TOKEN_DELIMITER_QUESTION:
        case TOKEN_DELIMITER_EXCLAMATION:
        case TOKEN_DELIMITER_HEADING:
        case TOKEN_DELIMITER_OTHER:
        case TOKEN_PERCENTAGE:
        case TOKEN_MATH:
        case TOKEN_E_MAIL:
        case TOKEN_URL:
            w = specialWord[token];
            if(!Tags().SpecialTag(token)) {
                std::cerr << token << std::endl;
            }
            ensure(Tags().SpecialTag(token));
            theTokens[nTokens].SetSelectedTag(Tags().SpecialTag(token));
            ensure(w);
            break;
        case TOKEN_EXCLAMATION_MARK:
            w = specialWord[token];
            break;
        case TOKEN_NEWLINE:
            if (!xNewlineMeansNewSentence) {
                theTokens[nTokens-1].trailingSpace = 1;
                continue;
            }
        case TOKEN_BEGIN_PARAGRAPH:
        case TOKEN_BEGIN_HEADING:
        case TOKEN_END_HEADING:
        case TOKEN_BEGIN_TITLE:
        case TOKEN_END_TITLE:
            if (theTokens[nTokens-1].token == TOKEN_NEWLINE)
                nTokens--;
            theTokens[nTokens++].SetWord(specialWord[TOKEN_DELIMITER_HEADING], NULL, token);
            continue;
        case TOKEN_BEGIN_TABLE:
        case TOKEN_TABLE_TAB:
        case TOKEN_END_TABLE:
            continue;
        case TOKEN_SPACE:
            theTokens[nTokens-1].trailingSpace = 1;
            continue;
        case TOKEN_PROPER_NOUN:
        case TOKEN_PROPER_NOUN_GENITIVE:
        case TOKEN_UNKNOWN:
        case TOKEN_SILLY:
        case TOKEN_ERROR:
            Message(MSG_WARNING, int2str((uchar)*tokenizer.TokenString()), "was recognized as",
                    Token2String(token), "by the tokenizer");
            break;
        } 
        const char *string = tokenizer.TokenString();
        if (!w) {
            w = FindMainOrNewWordAndAddIfNotPresent(lookUp ? lookUp : string);
            if (token == TOKEN_PUNCTUATION)
                theTokens[nTokens].SetSelectedTag(Tags().SpecialTag(token));
        }
        if (w->IsNewWord()) {
            NewWord *nw = (NewWord*) w;
            nw->freq++;
        }
        w->textFreq++;
        WordToken &t = theTokens[nTokens++];
        t.offset = prevOffset;
        t.SetWord(w, string, token);
        // don't use this strategy, it is bad...
        int temp = next_index(wordvec, string);
        if(temp >= 0) {
            t.SetSelectedTag(tagvec[temp], false);
        } 
    }
 endText:
    theTokens[nTokens].SetWord(specialWord[TOKEN_DELIMITER_PERIOD], "$.", TOKEN_END);
    theTokens[nTokens+1].SetWord(specialWord[TOKEN_DELIMITER_PERIOD], "$.", TOKEN_END);
    if (xTakeTime) 
        tokenizeTime = timer.Restart();
    if(nTokens > 1) // jonas
        BuildSentences(theTokens);
    if (xTakeTime) 
        sentenceTime = timer.Restart();
    TagText(); // fix the ones were tokenization was different
}

