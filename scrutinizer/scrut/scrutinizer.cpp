/* scrutinizer.cc
 * author: Johan Carlberger
 * last change: 2002-04-09
 * comments: Scrutinizer class
 */

#include "scrutinizer.h"
#include "matchingset.h"
#include "message.h"
#include "ruleset.h"
#include "timer.h"
#include <sstream>

extern "C" {
#include "stavaapi.h"
}

#ifdef PROBCHECK
#include "prob.h"
#include "output.h"
#include <sstream>
#endif // PROBCHECK

// these are used for the xml output
#include "defines.h"
#include "report.h"

std::ofstream messageStream("messages");


static const int GE_BUF_SIZE = 200;

static Timer timer;
static Timer::type readTime, scrutTime;
//static Timer::type loadTime;  // jb: not used?


DefObj(Scrutinizer);

Scrutinizer::Scrutinizer(OutputMode mode) : ruleSet(NULL) {
    if (GramError::scrutinizer)
        Message(MSG_ERROR, "more than one scrutinizer instance is not recommended now");
    SetFormatSettings(mode);
    GramError::scrutinizer = this;
    GramError::matchingSet = &GetMatchingSet();
    gramErrorBufSize = 0;
    gramErrors = NULL;
    nGramErrors = 0;
    xAmbiguousNewWords = true; // för demon
    NewObj();
}

Scrutinizer::~Scrutinizer() {
    Message(MSG_STATUS, "deleting scrutinizer...");
    if (gramErrors) {
        for (int i=0; i<nGramErrors; i++)
            delete gramErrors[i];
        delete [] gramErrors; // jonas, delete -> delete []
    }
    GramError::Reset();
    GetMatchingSet().DeleteBuffers();
    DeleteElements();
    DelObj();
}

inline static void WriteInt(FILE *fp, int i) {
    fwrite(&i, sizeof(int), 1, fp);
}
inline static int ReadInt(FILE *fp) {
    int i;
    if (fread(&i, sizeof(int), 1, fp) <= 0) return 0;
    return i;
}

bool Scrutinizer::Load(const char *taggerLexDir, const char *ruleFile) {
    if (xTakeTime) timer.Start();
    Message(MSG_STATUS, "loading default scrutinizer...");
    if (!taggerLexDir)
        taggerLexDir = getenv("TAGGER_LEXICON");
    if (!taggerLexDir)
        taggerLexDir = "/afs/nada.kth.se/misc/tcs/granska/lib/lexicons/suc/";
    Tagger::Load(taggerLexDir);
    if (!IsLoaded()) {
        Message(MSG_WARNING, "cannot load tagger from", taggerLexDir);
        return false;
    }
    char *stavaDir = getenv("STAVA_LEXICON");
    if (!stavaDir)
        stavaDir = "/afs/nada.kth.se/misc/tcs/language/lib/";
    // Specialinställningar till Stava:
    xGenerateCompounds = 1;
    xAcceptCapitalWords = xAcceptSpellCapitalWords;
    if (!StavaReadLexicon(stavaDir,1,1,1,1,1,1,(uchar*)"\t")) {
        Message(MSG_WARNING, "cannot load Stava lexicons from", stavaDir);
        return false;
    }
    if (!ruleFile)
        ruleFile = getenv("SCRUTINIZER_RULE_FILE");
    if (!ruleFile)
        ruleFile = "/afs/nada.kth.se/misc/tcs/granska/lib/lexicons/suc/default-swedish-rules";
    ruleSet = ReadRules(this, ruleFile);
    if (!ruleSet->IsFixed())
        return false;
    RuleTerm::nTags = Tags().Ct();
    RuleTerm::tagLexicon = &Tags();
#ifdef PROBCHECK
    Prob::report_granska_rules(ruleSet);
#endif // PROBCHECK
    if (xOptimizeMatchings) {
        char optFileName[1000];
        sprintf(optFileName, "%s.opt", ruleFile);
        FILE *fp = fopen(optFileName, "rb");
        const int magic = 6509869;
        bool ok = false;
        if (fp) {
            if (!xPrintOptimization) {
                const int magic2 = ReadInt(fp);
                if (magic == magic2) {
                    Message(MSG_STATUS, "loading rule optimizations from", optFileName);
                    ok = RuleTerm::ReadMatchingOptimization(fp);
                    if (!ok)
                        Message(MSG_WARNING, "error in optimization file", optFileName);
                }
            }
            fclose(fp);
        }
        if (!ok) {
            GetRuleSet()->OptimizeMatchings();
            Message(MSG_STATUS, "saving rule optimizations to", optFileName);
            fp = fopen(optFileName, "wb");
            WriteInt(fp, magic);
            RuleTerm::SaveMatchingOptimization(fp);
            fclose(fp);
        }
    }
    if (xTakeTime) loadTime = timer.Get();
    return true;
}

const Text *Scrutinizer::ReadTextFromString(char *text) {
    if (xVerbose) std::cout << "ReadTextFromString(\"" << text << "\")" << std::endl;
    std::istringstream in(text);
    return ReadTextFromStream(&in);
}

const Text *Scrutinizer::ReadTextFromFile(const char *fileName) {
    std::ifstream in;
    if (!FixIfstream(in, fileName, NULL, true))
        return NULL;
    return ReadTextFromStream(&in);
}
extern bool xReadTaggedText;// jonas, intended for use only for evaluation study 030120 - 030228
const Text *Scrutinizer::ReadTextFromStream(std::istream *in) {
    if (xTakeTime) timer.Start();
    SetStream(in);
    xTaggedText = false;
    while(nGramErrors > 0)
        delete gramErrors[--nGramErrors];
    if(xReadTaggedText) 
        ReadTaggedTextQnD();// jonas, intended for use only for evaluation study 030120 - 030228
    else
        ReadText();// jonas, this is the normal (not study 030120...) way to do things
    TagText();
    if (xTakeTime) readTime = timer.Get();
    return &theText;
}

void Scrutinizer::Scrutinize(AbstractSentence *s) {
    xCurrSentence = s;
    s->SetContentBits();
    if (xPrintMatchings) {
        std::cout << std::endl;
        if (GetMatchingSet().CheckMode()) std::cout << tab;
        std::cout << s << std::endl;
    }
    if (xOptimizeMatchings)
        RuleTerm::FindMatchingsOptimized(s);
    else
        ruleSet->FindMatchings(s);
    GetMatchingSet().TerminateSearch(s);

    xCurrSentence = NULL;
}

GramError **Scrutinizer::Scrutinize(int *n) {
    if (nGramErrors > 0) {
        Message(MSG_WARNING, "same text scrutinized twice");
        *n = nGramErrors;
        return gramErrors;
    }   
    Message(MSG_STATUS, "scrutinizing text...");
    if (ruleSet->NRules() <= 0) {
        Message(MSG_WARNING, "there are no rules");
        *n = 0;
        return NULL;
    }
    for (int i=0; i<20; i++) xCase[i] = 0; // this line is the only one that uses xCase? // jonas
    if (xTakeTime) timer.Start();
    GetMatchingSet().Clear();
    RuleTerm::prepTime = 0;
    Expr::evalTime = 0;
    for (Sentence *s=theText.FirstSentence(); s; s=s->Next()) {
        Scrutinize(s);
        for (const GramError *g = s->gramError; g; g = g->Next()) {
            if (nGramErrors >= gramErrorBufSize) {
                gramErrorBufSize += GE_BUF_SIZE;
                GramError **ge = new GramError*[gramErrorBufSize]; // new OK
                if (nGramErrors > 0) {
                    memcpy(ge, gramErrors, nGramErrors*sizeof(GramError*));
                    delete [] gramErrors; // jonas delete -> delete [] 
                }
                gramErrors = ge;
            }
            gramErrors[nGramErrors++] = (GramError*) g;
        }
    }
    *n = nGramErrors;
    if (xTakeTime) scrutTime = timer.Get();
    //  if (xVerbose) {
    //    for (int i=0; i<20; i++) std::cout << xCase[i] << ' ';
    //    std::cout << std::endl; }
    return gramErrors;
}

void Scrutinizer::CheckAcceptAndDetect() {
    Message(MSG_STATUS, "checking accept and detect texts of rules...");
    SetMessageStream(MSG_STATUS, NULL);
    xPrintAllSentences = true;
    std::cout << std::endl << std::endl;
    int nWithout = 0, nMissed = 0, nFalse = 0;
    for (int i=0; i<ruleSet->NRules(); i++) {
        Rule *r = ruleSet->GetRule(i);
        const char *d = r->FirstRuleTerm()->GetDetect();
        const char *a = r->FirstRuleTerm()->GetAccept();
        if (d || a) {
            std::cout << "checking " << r->Header() << "..." << std::endl << std::endl;
            if (d) {
                std::cout << "detect: " << std::endl;
                // Scrutinize(d);
                /*	if (!FirstGramError()) {
                    Message(MSG_WARNING, r->Header(), "did not detect");
                    Message(MSG_CONTINUE, d);
                    nMissed++;
                    }*/
            }
            if (a) {
                std::cout << "accept: " << std::endl;
                // Scrutinize(a);
                /*if (FirstGramError()) {
                  Message(MSG_WARNING, r->Header(), "detected");
                  Message(MSG_CONTINUE, a);
                  nFalse++;
                  }*/
            }
        } else
            nWithout++;
    }
    if (nWithout > 0)
        Message(MSG_WARNING, int2str(nWithout), "rules have neither detect or accept text");
    if (nMissed > 0)
        Message(MSG_WARNING, int2str(nMissed), "detect texts were not detected");
    if (nFalse > 0)
        Message(MSG_WARNING, int2str(nFalse), "accept texts were detected");
    Message(MSG_COUNTS, "during accept and detect check");
}

void Scrutinizer::PrintResult(std::ostream &out) {
    if (xPrintMatchings || xPrintOptimization || !xPrintGramErrors) return;
    xPrintAllWords = true;
    xPrintOneWordPerLine = false;
    if (xTakeTime) {
        out.setf(std::ios::fixed); out.precision(4);
        theText.CountContents();
        if (xOptimizeMatchings) GetRuleSet()->PrintEvaluationTimes();
        out << int(loadTime * 1000.0 / Timer::clocks_per_sec())
            << tab << "ms to load scrutinizer" << std::endl
            << int((double)Timer::clocks_per_sec() * theText.NWordTokens()/readTime)
            << tab << "word-tokens/s read and tagged (" << theText << ')' << std::endl
            << int((double)Timer::clocks_per_sec() * theText.NWordTokens()/scrutTime)
            << tab << "word-tokens/s scrutinized (" << nGramErrors << " gram-errors)" << std::endl
            << int((double)Timer::clocks_per_sec() * theText.NWordTokens()/(RuleTerm::prepTime))
            << tab << "word-tokens/s prepared for matching" << std::endl
            << int((double)Timer::clocks_per_sec() * theText.NWordTokens()/(ruleSet->matchTime))
            << tab << "word-tokens/s in TryMatching() including Eval()" << std::endl
            << int((double)Timer::clocks_per_sec() * theText.NWordTokens()/(Expr::evalTime))
            << tab << "word-tokens/s in Eval()" << std::endl
            << int((double)Timer::clocks_per_sec() * theText.NWordTokens()/(scrutTime+readTime))
            << tab << "word-tokens/s read, tagged and scrutinized" << std::endl
            << int((double)Timer::clocks_per_sec() * theText.NWordTokens()/(scrutTime+readTime+loadTime))
            << tab << "word-tokens/s read, tagged and scrutinized (incl. loading)" << std::endl;
        return;
    }
    if (xPrintHTML) {
        out << "<H4>Förklaringar till markeringarna</H4>"
            << "<LI>Misstänkta områden i texten markeras med "
            << xRed << "rött" << xNoColor << "</LI>" << xEndl
            << "<LI>Ersättningsförslagen presenteras därefter och ändringar markeras med "
            << xGreen << "grönt" << xNoColor << "</LI>" << xEndl
            << "<LI>En kortfattad" << xBlue << " blå " << xNoColor
            << "kommentar skrivs efter varje förslag</LI>" << xEndl
            << "<LI>En länk till utförligare information om feltypen"
            << " finns efter vissa kommentarer</LI>" << xEndl;
        if (!xSuggestionSameAsOriginalMeansFalseAlarm) 
            out << "<LI>Ett " << xRed << 'F' << xNoColor
                << " efter förslaget betyder att inga ändringar gjorts, "
                << "vilket indikerar ett möjligt falskt alarm</LI>" << xEndl;
        if (xAcceptNonImprovingCorrections)
            out << "<LI>Ett " << xRed << 'E' << xNoColor
                << " efter förslaget betyder att rättelsen kan ge ett annat fel</LI>" << xEndl;
        out << "<LI>Om en mening innehåller flera misstänkta"
            << " felområden presenteras samma mening flera gånger</LI>"
            << xEndl << xEndl
            << "Skicka gärna synpukter till <A HREF=\"mailto:knutsson@nada.kth.se\">&lt;knutsson@nada.kth.se&gt;</A>"
            << "<HR WIDTH=\"100%\"></H1>";
    }

#ifdef PROBCHECK
    Prob::Output &o = Prob::output();
    o.push("scrutinizer");
    for(const Sentence *s=theText.FirstSentence(); s; s=s->Next())
        {
            const GramError *g = 0;

            // jb: this is used to determine whether there is an actual
            // error or just @recog rules. If error, we will print.
            bool found = false;
            for(g = s->GetGramError(); g; g = g->Next())
                {
                    g->Report();
                    if(g->IsError())
                        found = true;
                }
            // no error found, don't output sentence
            if(!found && !xPrintAllSentences)
                continue;

            int offset = s->GetWordToken(0 + 2)->Offset();
            o.push("s");
            o.attr("ref", offset);
#ifdef DEVELOPER_OUTPUT
            if(s->GetGramError() || xPrintAllSentences)
                {
                    o.add("tokens", s->NTokens());

                    //std::ostringstream ss;
                    //ss << s;                    
                    //o.add("text", ss.str().c_str());                    
                    //std::string orgText = fixXML(s->getOriginalText());
                    o.add("text", Misc::fixXML(s->getOriginalText())); //Oscar
                    
                    if(!s->IsHeading())
                        o.add("heading");
                    if(s->EndsParagraph())
                        o.add("paragraph");
                    o.push("contents");
                    for(int i = 0; i < s->NTokens(); i++)
                        {
                            const WordToken *token = s->GetWordToken(i);
                            const Tag *tag = token->SelectedTag();

                            //std::string xmlOkWord = fixXML(token->RealString());
                            //if(xmlOkWord.length()>0)
                                o.add("w", Misc::fixXML(token->RealString()));
                                //else
                                //    o.add("w", token->RealString());

                            o.attr("no", i);
                            if(token->Offset())
                                o.attr("ref", token->Offset() - offset);
                            o.attr("tag", tag->String());
                            
                            //std::string xmlOkLemma = fixXML(token->LemmaString());
                            //if(xmlOkLemma.length()>0)
                            //    o.attr("lemma", xmlOkLemma);
                            //else
                                o.attr("lemma", Misc::fixXML(token->LemmaString()));
                            //if(token->LemmaString()[0] != '"')
                            //    o.attr("lemma", token->LemmaString());
                            //else
                            //    o.attr("lemma", "'");
                        }
                    o.pop();  // push("contents");
                }
#endif // DEVELOPER_OUTPUT
            if(found)
                {
                    o.push("gramerrors");
                    for(g = s->GetGramError(); g; g = g->Next())
                        if(g->IsError())
                            g->Output();
                    o.pop();  // push("gramerrors");
                }

            o.pop();  // push("sentence");
        }
    o.pop();  // push("scrutinizer");

    Prob::print(this);
#endif
#ifndef PROBCHECK
    // Old style (not PROBCHECK)
    for(const Sentence *s=theText.FirstSentence(); s; s=s->Next())
        {
            const GramError *g = 0;

            // jb: this is used to determine whether there is an actual
            //     error or just @recog rules. If error, we will print.
            bool found = false;
            for(g = s->GetGramError(); g; g = g->Next())
                {
                    g->Report();
                    if(g->IsError())
                        found = true;
                }
            // no error found, don't output sentence
            if(!found && !xPrintAllSentences)
                continue;

            if(s->GetGramError() || xPrintAllSentences)
                {
                    out << s << ' ';
                    if(s->GetGramError())
                        {
                            if(!s->IsHeading()) out << xEndl; 
                        }
                    else if(s->EndsParagraph() && !s->IsHeading())
                        out << xEndl << xEndl;
                }
            for(g = s->GetGramError(); g; g = g->Next())
                if(g->IsError())
                    out << g << xEndl;
        }
#endif

}

// std::string Scrutinizer::fixXML(std::string word) {
//     //Oscar, fix of bad XML output
//     std::ostringstream xmlOkWord;
//     int len = word.length();
//     int prevOffset = -1;
//     int offset;
//     char c;    
//     for(offset = 0; offset < len; offset++) {        
//         c = word.at(offset);
//         if(c=='&'||c=='\''||c=='\"'||c=='<'||c=='>') {
//             if(prevOffset < offset-1)
//                 xmlOkWord << word.substr(prevOffset+1,offset-prevOffset-1);
//             switch(c) {
//             case '&': xmlOkWord << "&amp;"; break;
//             case '\'': xmlOkWord << "&apos;"; break;
//             case '\"': xmlOkWord << "&quot;"; break;
//             case '<': xmlOkWord << "&lt;"; break;
//             case '>': xmlOkWord << "&gt;"; break;            
//             }
//             prevOffset=offset;
//         }
//     }
//     if(prevOffset != 0 && prevOffset < --offset) {        
//         xmlOkWord << word.substr(prevOffset+1, offset-prevOffset);
//     }
//     std::string ret = xmlOkWord.str();
    
//     if(ret.length()==0) return word;
//     else return ret;    
// }

// std::string Scrutinizer::fixXML(const char* word) {
//     //Oscar, fix of bad XML output
//     //should check if the Token can have any bad XML first    
//     std::ostringstream xmlOkWord;
//     int prevOffset = -1;
//     int offset;
//     char c;
//     for(offset = 0; word[offset]; offset++) {
//         c=word[offset];
//         if(c=='&'||c=='\''||c=='\"'||c=='<'||c=='>') {
//             if(prevOffset < offset-1)
//                 xmlOkWord.write(word+prevOffset+1,offset-prevOffset-1);
//             switch(c) {
//             case '&': xmlOkWord << "&amp;"; break;
//             case '\'': xmlOkWord << "&apos;"; break;
//             case '\"': xmlOkWord << "&quot;"; break;
//             case '<': xmlOkWord << "&lt;"; break;
//             case '>': xmlOkWord << "&gt;"; break;
//             }
//             prevOffset=offset;
//         }
//     }    
//     if(prevOffset != -1 && prevOffset < --offset)
//         xmlOkWord.write(word+prevOffset+1,offset-prevOffset);
//     return xmlOkWord.str();        
// }

bool Scrutinizer::IsSpellOK(const char *s, Token token) {
    //  if (xVerbose)
    //    std::cout << xCurrentRule << " spell-checking: " << s << ' ' << token << std::endl;
    switch(token) {
    case TOKEN_E_MAIL:
    case TOKEN_MATH:
    case TOKEN_CARDINAL:
        return true;
    case TOKEN_BAD_CARDINAL:
        // return false here gives a lot of false alarms
        // such as "Tabell 1.1"
        return true; // return false;
    case TOKEN_PARAGRAPH:
        if (*s++ != '§') {
            while(*s && !IsSpace(*s)) s++;
            if (!IsSpace(*s)) return false;
            if (*++s != '§') return false;
            if (!*++s) return true;
            return false;
        }
        if (!IsSpace(*s++)) return false;
        if (!IsDigit(*s)) return false;
        return true;
    case TOKEN_PERCENTAGE:
        for (; *s != '%' && *s != 'p'; s++);
        if (IsSpace(*(s-1)))
            return true;
        return false;
    case TOKEN_TIME:
        if (*s == 'k') {
            if (!strncmp(s, "klockan ", 8))
                s += 8;
            else if (!strncmp(s, "kl. ", 4))
                s += 4;
            else return false;
        }
        if (IsDigit(*s)) s++; else return false;
        if (IsDigit(*s)) s++;
        if (!*s || *s == '.')
            return true;
        return false;
    case TOKEN_YEAR:
        for (; *s; s++) {
            if (IsPunct(*s) && IsPunct(s[1]))
                return false;
            if (IsLetter(*s) && IsDigit(s[1]))
                return false;
        }
        return true;
    case TOKEN_DATE: {
        if (Lower(*s) == 'd') {
            s += 3;
            if (!IsSpace(*s++)) return false;
            if (IsSpace(*s)) return false;
        }
        char ss[100];
        for (int k=0; k<2; k++) {
            int i;
            for (i=0; IsDigit(*s); i++)
                ss[i] = *s++;
            ss[i] = '\0';
            int n = atoi(ss);
            if (n < 1 && n > 31) return false;
            if (*s != '-') break;
            s++;
        }
        if (!IsSpace(*s++)) return false;
        if (strncmp(s, "januari", 7) && strncmp(s, "februari", 8) && strncmp(s, "mars", 4) &&
            strncmp(s, "april", 5) && strncmp(s, "maj", 3) && strncmp(s, "juni", 4) &&
            strncmp(s, "juli", 4) && strncmp(s, "augusti", 7) && strncmp(s, "september", 8) && 
            strncmp(s, "oktober", 7) && strncmp(s, "november", 8) && strncmp(s, "december", 8))
            return false;
        while(IsLetter(*s)) s++;
        if (!*s) return true;
        if (!IsSpace(*s++)) return false;
        if (IsSpace(*s)) return false;
        return true;
    }
    case TOKEN_SPLIT_WORD: {
        for (int p = strlen(s)-2; p>0; p--)
            if (IsSpace(s[p]))
                return (StavaWord((unsigned char *) (s+p+1))) ? true : false;
        Message(MSG_WARNING, s, "not a split-word");
        return true;
    }
    case TOKEN_ABBREVIATION: {
        if (!strcmp(s, "m") || !strcmp(s, "m.")) {
            Word *w = FindMainWord("meter");
            if (w->TextFreq() > 0)
                return false;
            return true;
        }
        return false;
    }
    default: {
        bool b = (StavaWord((unsigned char *) s)) ? true : false;
        if (!b) return false;
        for (const char *t=s; *t; t++)
            if (IsDigit(*t) && IsLetter(t[1]) && IsLetter(t[2]))
                return false;
        return true;
    }
    }
}

char *Scrutinizer::SpellOK(const char *s, Token token) {
    //  if (xVerbose)
    //    std::cout << xCurrentRule << " spell-corr: " << s << ' ' << token << std::endl;
    static char string[MAX_WORD_LENGTH * MAX_SUGGESTIONS];
    switch(token) {
    case TOKEN_PARAGRAPH:
        return "§ 7";
    case TOKEN_SPLIT_WORD: {
        for (int p = strlen(s)-2; p>0; p--)
            if (IsSpace(s[p])) {
                char *r = (char*) StavaCorrectWord((unsigned char *) (s+p+1));
                if (r) {
                    strcpy(string, r);
                    free(r);
                    return string;
                }
            }
        return "";
    }
    case TOKEN_ABBREVIATION: {
        if (!strcmp(s, "m") || !strcmp(s, "m."))
            return "meter";
        int len = strlen(s);
        if (s[len-1] == '.') {
            strcpy(string, s);
            string[len-1] = '\0';
        } else {
            strcpy(string, s);
            strcat(string, ".");
        }
        if (FindMainWord(string))
            return string;
        return "";
    }
    case TOKEN_PERCENTAGE: {
        char *ss = string;
        for (; *s; s++) {
            if (*s == '%' || *s == 'p') 
                if (!IsSpace(*(ss-1)))
                    *ss++ = ' ';
            *ss++ = *s;
        }
        *ss = '\0';
        return string;
    }
    case TOKEN_TIME: {
        if (*s == 'k') {
            if (!strcmp(s, "klockan")) strcpy(string, "klockan ");
            else strcpy(string, "kl. ");
        } else *string = '\0';
        char *ss = string;
        for (; *ss; ss++);
        for (; *s; s++) if (IsDigit(*s)) *ss++ = *s;
        if (IsDigit(*(ss-3))) {
            *ss = *(ss-1); *(ss-1) = *(ss-2); *(ss-2) = '.'; ss++; }
        *ss = '\0';
        return string;
    }
    case TOKEN_YEAR: {
        char *ss = string;
        for (; *s; s++)
            if (IsPunct(*s) && IsPunct(s[1])) {
                *ss++ = '-'; s++;
            } else if (IsLetter(*s) && IsDigit(s[1])) {
                *ss++ = *s; *ss++ = ' ';
            } else
                *ss++ = *s;
        *ss = '\0';
        return string;
    }
    case TOKEN_DATE: {
        char *ss = string;
        *ss = '\0';
        if (!strncmp(s, "den", 3)) {
            strcpy(ss, "den ");
            s+=3;
            while(IsSpace(*s)) s++;
        }
        char tt[MAX_WORD_LENGTH], *t = tt;
        int i;
        const char *u = s;
        for (i=0; IsDigit(*s); i++)
            t[i] = *s++;
        t[i] = '\0';
        int n = atoi(t);
        if (n < 1)
            strcat(ss, "1");
        else if (n > 31)
            strcat(ss, "31");
        else if (n < 10)
            strncat(ss, u, 1);
        else
            strncat(ss, u, 2);
        strcat(ss, " ");
        if (!strncmp(s, ":e", 2))
            s+=2;
        while(IsSpace(*s)) s++;
        switch(Lower(*s)) {
        case 'j': if (s[1] == 'a') strcat(ss, "januari ");
        else if (!strncmp(s, "jun", 3)) strcat(ss, "juni ");
        else strcat(ss, "juli"); break;
        case 'f': strcat(ss, "februari"); break;
        case 'm': if (!strncmp(s, "mar", 3)) strcat(ss, "mars");
        else strcat(ss, "maj"); break;
        case 'a': if (s[1] == 'p') strcat(ss, "april");
        else strcat(ss, "augusti"); break;
        case 's': strcat(ss, "september"); break;
        case 'o': strcat(ss, "oktober"); break;
        case 'n': strcat(ss, "november"); break;
        default: strcat(ss, "december");
        }
        while(*s && !IsDigit(*s)) s++;
        if (IsDigit(*s)) {
            strcat(ss, " ");
            while(*ss) ss++;
            for (; *s; s++)
                if (IsDigit(*s))
                    *ss++ = *s;
            *ss = '\0';
        }
        return string;
    }

    case TOKEN_BAD_CARDINAL: {
        int a=0; const char *p = NULL, *t;
        for (t = s; *t; t++)
            if (IsDigit(*t)) {
                if (!p) {
                    if (*t != '0' || a) a++;
                }
            } else if (IsPunct(*t))
                if (!p) p = t;
        char *ss = string;
        if (*s == '-') {
            *ss++ = '-';
            s++;
        }
        if (a == 0) {
            *ss++ = '0';
            if (p) *ss++ = ',';
            if (p)  // buggfix 001125 johan
                for (t=p; *t; t++)
                    if (IsDigit(*t)) *ss++ = *t;
            *ss = '\0';
            return string;
        }
        bool b = true;
        for (;*s; s++)
            if (IsDigit(*s)) {
                if (a) {
                    if (b) b = false;
                    else if (a%3 == 0) *ss++ = ' ';
                    *ss++ = *s;
                    a--;
                } else *ss++ = *s; 
            } else if (IsPunct(*s) && p) {
                *ss++ = ',';
                p = NULL;
            }
        *ss = '\0';
        char *k;
        if ((k = strchr(string, ',')) != NULL && IsDigit(k[1]) &&
            IsDigit(k[2]) && IsDigit(k[3])) {
            strcpy(ss+1, string);
            *ss = '\t';
            *k = ' ';
        }
        return string;
    }
    default: {
        char *t = (char*) StavaCorrectWord((unsigned char *) s);
        if (t) {
            strcpy(string, t);
            free(t);
            if (*string) return string;
        }
        char *ss = string;
        for (const char *tt=s; *tt; tt++) {
            *ss++ = *tt;
            if (IsDigit(*tt) && IsLetter(tt[1]))
                *ss++ = '-';
        }
        *ss = '\0';
        if (FindMainWord(string))
            return string;
        ss = string;
        for (const char *tt=s; *tt; tt++) {
            *ss++ = *tt;
            if (IsDigit(*tt) && IsLetter(tt[1]))
                *ss++ = ' ';
        }
        *ss = '\0';
        if(strcmp(s, string) == 0) {
            // if string == s, return something else or we get "false alarm" // jonas
            *ss++ = ' '; // this should lead to a NON_IMPROVING_SUGGESTION, which is fine
            *ss++ = ' '; // but it is still an ugly hack
            *ss++ = ' '; 
            *ss = '\0';
        }
        return string;
    }
    }
}

void Scrutinizer::Analyze() {
    for (int i=0; i<ruleSet->NRules(); i++)
        {
            Rule *r = ruleSet->GetRule(i);
            if (!r->IsHelpRule() && strcmp(r->CategoryName(), "analyze"))
                ruleSet->InActivate(r);
        }
    xPrintMatchings = true;
    int n;
    Scrutinize(&n);
    PrintResult();
}


