/* wordtoken.hh
 * author: Johan Carlberger
 * last change: 2000-05-03
 * comments: WordToken class
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

#ifndef _wordtoken_hh
#define _wordtoken_hh

#include "letter.h"
#include "settings.h"
#include "stringbuf.h"
#include "token.h"
#include "word.h"

extern bool xAcceptSpellCapitalWords;
extern bool xAcceptSpellProperNouns;
extern bool xAcceptAllWordsInCompounds;

class WordToken {
    friend class Tagger;
    friend class DynamicSentence;
    friend class AbstractSentence;
 public:
    WordToken();
    ~WordToken() { DelObj(); }
    void Clear() { word = NULL; string = NULL; tagSelected = NULL; selectedTagErasable = 1; 
    // next 2 lines by jonas 030321
    token=TOKEN_END;offset=0;current=0;orgPos=0;firstCapped=0;allCapped=0;firstInSentence=0;
    trailingSpace=0;hard=0;repeated=0;changed=0;inserted=0;marked=0;marked2=0;
  
    }
    const WordTag *FirstInterpretation();
    const WordTag *NextInterpretation();
    const WordTag *CurrentInterpretation() const { return current; }
    const char *LexString() const { return GetWord()->String(); }
    const char *RealString() const { return string; }
    const char *LemmaString() const;
    Word *GetWord() const { return word; }
    WordTag *GetWordTag() const { return GetWord()->GetWordTag(SelectedTag()); }
    int Offset() const { return offset; }
    int OrgPos() const { return orgPos; }
    Tag *SelectedTag() const { return tagSelected; }
    void SetSelectedTag(Tag*, bool erasable = true);
    void SetWord(Word*, const char *string, Token);
    void SetCapped(const char *);
    void SetMarked(int a = 1) { marked = a; }
    void SetMarked2(int a = 1) { marked2 = a; }
    bool IsSpellOK() const { return GetWord()->IsSpellOK() ||
                                 (IsAllCapped() && xAcceptSpellCapitalWords) ||
                                 (IsFirstCapped() && !IsFirstInSentence() &&
                                  xAcceptSpellProperNouns && SelectedTag()->IsProperNoun()); }
    bool IsFirstCapped() const { return firstCapped; }
    bool IsAllCapped() const { return allCapped; }
    bool HasTrailingSpace() const { return trailingSpace; }
    bool IsHard() const { return hard; }
    bool IsRepeated() const { return repeated; }
    bool IsBeginOK() const { return xAcceptAllWordsInCompounds || GetWord()->IsCompoundBeginOK(); }
    bool IsEndOK() const { return xAcceptAllWordsInCompounds || GetWord()->IsCompoundEndOK(); }
    bool IsChanged() const { return changed; }
    bool IsMarked() const { return marked; }
    bool IsMarked2() const { return marked2; }
    Token GetToken() const { return token; }
    bool IsFirstInSentence() const { return firstInSentence; }
    void SetFirstInSentence(bool);
    void Print(std::ostream& = std::cout, bool printSpace = true) const; 
    void PrintVerbose(std::ostream& = std::cout) const;
    void PrettyPrintTextString(std::ostream& = std::cout) const;
    static void Reset() { stringBuf.Reset(); }
    WordToken *next;
 private:
    static StringBuf stringBuf;
    Token token;
    Word *word;
    const char *string;
    Tag *tagSelected;
    int offset;             // character position in text
    const WordTag* current; // for iterating
    char orgPos;            // used by DynamicSentence only
    Bit firstCapped : 1;
    Bit allCapped : 1;
    Bit firstInSentence : 1;
    Bit trailingSpace : 1;
    Bit hard : 1;
    Bit selectedTagErasable : 1;
    Bit repeated : 1;
    Bit changed : 1;
    Bit inserted : 1;
    Bit marked : 1;         // used by rules
    Bit marked2 : 1;         // used by rules
    // 13 bits free

    DecObj();
};

inline WordToken::WordToken()
    : token(TOKEN_END),
     word(NULL),
     string(NULL),
     tagSelected(NULL),
     offset(0),
     current(0),
     orgPos(0),
     firstCapped(0),
     allCapped(0),
     firstInSentence(0),
     trailingSpace(0),
     hard(0),
     selectedTagErasable(1),
     repeated(0),
     changed(0),
     inserted(0),
     marked(0),
     marked2(0)
{
    NewObj();
} 

inline void WordToken::SetSelectedTag(Tag* t, bool erasable) {
    if (selectedTagErasable)
        tagSelected = t;
    if (!erasable) {
        selectedTagErasable = 0;
        ensure(tagSelected);
    }
}

inline const WordTag *WordToken::FirstInterpretation() {  // to iterate over all interpretations 
    current = word;
    return word;
}

inline const WordTag *WordToken::NextInterpretation() {  // next when iterating, NULL when finished 
    if (current) {
        current = current->Next();
        if (current && current->IsExtraLemma())
            return NextInterpretation();
        return current;
    } else
        return NULL;
}

inline std::ostream& operator<<(std::ostream& os, const WordToken &w) {
    w.Print(os); return os;
}
inline std::ostream& operator<<(std::ostream& os, const WordToken *w) {
    if (w) os << *w; else os << "(null WordToken)"; return os;
}

#endif
