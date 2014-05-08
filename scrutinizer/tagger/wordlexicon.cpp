/* wordlexicon.cc
 * author: Johan Carlberger
 * last change: 2000-05-08
 * comments: WordLexicon class
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

#include "file.h"
#include "letter.h"
#include "message.h"
#include "newwordlexicon.h"
#include "settings.h"
#include "wordlexicon.h"
#include <string>	// getline

char StyleWord::styles[MAX_STYLES][MAX_PARAGRAPH];
DefObj(HashArray<Word>);
DefObj(HashArray<StyleWord>);
DefObj(WordLexicon);
DefObj(StyleWord);

WordLexicon::~WordLexicon() {
  Message(MSG_STATUS, "deleting word lexicon...");
  if (strings) { ExtByt(-CL); }
  if (wordsAlpha) { ExtByt(-CW * sizeof(Word*)); }
  delete [] more;
  delete [] strings;		    // jbfix: delete => delete []
  delete [] wordsAlpha;		    // jbfix: delete => delete []
  commentBuf.Reset();
  DelObj();

  // jbfix: free memory
  for(int i = 0; i < stylewords.Size(); i++)
      for(int j = 0; j < stylewords[i].NAlternatives(); j++)
	  if(stylewords[i].owns_mem[j])
	      delete stylewords[i].alt[j];
}

void WordLexicon::LoadInfo() {
  Message(MSG_STATUS, "loading words info...");
  char string[30];
  std::ifstream in;
  FixIfstream(in, lexiconDir, "info");
  in >> CW >> CL;
  in >> CWT >> string;
  in >> N_STYLEWORDS >> string;
}

void WordLexicon::AllocateMemory() {
  Message(MSG_STATUS, "allocating memory for words...");
  WordTag::words = this;
  CMW = CWT - CW;
  ensure(CW);
  Init("words", CW, CompareWords, KeyWord, RankWords, CompareStringAndWord, KeyWordString);
  more = new WordTag[CMW]; 
  if (!more) Message(MSG_ERROR, "out of memory");
  strings = new char[CL]; // new OK
  ExtByt(CL);
  if (!strings) Message(MSG_ERROR, "out of memory");
  if (N_STYLEWORDS > 0)
    stylewords.Init("stylewords", N_STYLEWORDS, CompareStyleWords, KeyStyleWord,
		    NULL, NULL, NULL);
  wordsAlpha = new Word*[CW]; // new OK
  ExtByt(CW * sizeof(Word*));
  if (!wordsAlpha) Message(MSG_ERROR, "out of memory");

}

Word *WordLexicon::FindAbbreviation(const char *abb) const { //obscure stuff
  const char *s=abb+1;
  for (; *s; s++)
    if (isspace(*s))
      break;
  if (*s || *(s-1) == '.') {
    char string[MAX_WORD_LENGTH], *u = string;
    for (const char *p = abb; *p; p++, u++)
      if (isspace(*p)) {
	if (*(u-1) != '.')
	  *u = '.';
	else
	  u--;
      } else
	*u = *p;
    if (*(u-1) != '.')
      *u++ = '.';
    *u = '\0';
    Word *w2 = Find(string);
    if (w2)
      return w2;
    *(u-1) = '\0';
    return Find(string);
  }
  return NULL;
}

WordTag *WordLexicon::GetExtraLemma(const WordTag *wt) const {
  for (int i=0; i<nExtraLemmas; i++)
    if (extraLemmas[i].wt == wt)
      return extraLemmas[i].lemma;
  Message(MSG_ERROR, "cannot find extra lemma for a word-tag");
  return NULL;
}

void WordLexicon::LoadLemmas() {
  std::ifstream in;
  FixIfstream(in, lexiconDir, "cwtl");
  Message(MSG_STATUS, "loading lemmas...");
  char wordString[MAX_WORD_LENGTH], lemmaString[MAX_WORD_LENGTH];
  Tag tag;
  std::ofstream out;
  int freq;
  nExtraLemmas = 0;
  for (int i = 0; i<CWT; i++) {
    in >> freq;
    while (in.peek() == '\t') in.get();
    //    in.getline(wordString, MAX_WORD_LENGTH, '\t'); // jonas, doesn't work if length > MAX, neither did my fix, skip long words in lexicon
    std::string tempstring;
    std::getline(in, tempstring, '\t');
    strncpy(wordString, tempstring.c_str(), MAX_WORD_LENGTH-1);
    wordString[MAX_WORD_LENGTH-1] = 0;

    in >> tag.string;
    while (in.peek() == '\t') in.get();
    //    in.getline(lemmaString, MAX_WORD_LENGTH);// jonas, doesn't work if length > MAX, neither did my fix, skip long words in lexicon
    std::getline(in, tempstring);
    strncpy(lemmaString, tempstring.c_str(), MAX_WORD_LENGTH-1);
    lemmaString[MAX_WORD_LENGTH-1] = 0;

    Word *w = Find(wordString);
    if (!w)
      Message(MSG_ERROR, "no such word in the lexicon:", wordString);
    Tag *t = tags->Find(tag);
    if (!t)
      Message(MSG_ERROR, "no such tag in the lexicon:", tag.String());
    WordTag *wt = w->GetWordTag(t);
    if (!wt)
      Message(MSG_ERROR, "no such word tag in the lexicon:",  wordString, tag.String());
    Word *lw = Find(lemmaString);
    WordTag *lwt = NULL;
    if (lw) {
      lwt = lw->GetWordTag(t->LemmaTag());
      if (!lwt && t->LemmaTag2())
	lwt = lw->GetWordTag(t->LemmaTag2());
      if (lwt && t->IsLemma() && lwt != wt && wt->TagFreq() == 0) {
	Message(MSG_MINOR_WARNING, wt->String(), t->String(), "has lemma", lwt->String());
	//	std::cout << wt << ' ' << lwt << std::endl;
      }
    }
    if (!lwt) {
      if (xPrintUnknownLemmas && !t->IsSilly()) {
	if (t->LemmaTag2() && lemmaString[strlen(lemmaString)-1] == 's')
	  std::cout<<lemmaString<<tab<<t->LemmaTag2()<<tab<<lemmaString<<std::endl;
	else
	  std::cout<<lemmaString<<tab<<t->LemmaTag()<<tab<<lemmaString<<std::endl;
      } else
	Message(MSG_MINOR_WARNING, "no such lemma in the lexicon:", lemmaString,
		t->LemmaTag()->String());
    } else if (wt->Lemma(0)) {
      if (nExtraLemmas >= MAX_N_EXTRA_LEMMAS)
	Message(MSG_ERROR, "number of multiple lemmas exceed",
		int2str(MAX_N_EXTRA_LEMMAS));
      if (wt->NLemmas() >= MAX_LEMMAS_PER_WORDTAG)
	Message(MSG_MINOR_WARNING, w->String(), "has more than",
		int2str(MAX_LEMMAS_PER_WORDTAG), "lemmas");
      else {
	extraLemmas[nExtraLemmas].wt = wt;
	extraLemmas[nExtraLemmas].lemma = lwt;
	nExtraLemmas++; // jonas ????? gdb says this line trashes memory
	wt->nExtraLemmas++;
      }
      if (xListMultipleLemmas) {
	std::cout<<w<<tab<<t<<tab<<w->lemma<<tab<<w->tagFreq<<std::endl;
	std::cout<<w<<tab<<t<<tab<<lemmaString<<tab<<freq<<std::endl;
      }
      // find all other wordtags and fix tagFreqs:
      WordTag *wt2;
      for (wt2 = wt->Next(); wt2; wt2 = wt2->Next())
	if (wt2->GetTag() == t) {
	  if (wt2->IsExtraLemma())
	    break;
	  wt->tagFreq += wt2->tagFreq;
	  wt2->extraLemma = 1;
	}
      for (wt2 = wt->Next(); wt2; wt2 = wt2->Next())
	if (wt2->GetTag() == t) {
	  wt2->tagFreq = wt->tagFreq;
	  wt2->lemma = lwt;
	}
    } else
      wt->lemma = lwt;
  }
}

void WordLexicon::AddExtraRule(WordTag *wt, ushort ruleIndex) {
  //std::cout << "Added '" << wt->String() << "'" << std::endl; 
  if (wt->NInflectRules() == 0) {
    wt->inflectRule = ruleIndex;
    return;
  }
  if (wt->nExtraInflectRules >= MAX_INFLECTION_RULES_PER_WORDTAG-1)
    Message(MSG_ERROR, "too many inflection rules for",
	    wt->String(), wt->GetTag()->String());
  ExtraRules *s = NULL;
  if (nExtraRules>0)
    if (extraRules[nExtraRules-1].wt == wt)
      s = &extraRules[nExtraRules-1];
    else if (strcmp(extraRules[nExtraRules-1].wt->String(), wt->String()) > 0)
    {
	std::cout << "Before '" << wt->String() << "', '" 
		  << extraRules[nExtraRules-1].wt->String() << "'"
		  << std::endl;
	//Message(MSG_ERROR, "word rules files must be sorted");
    }
  if (!s) {
    s = &extraRules[nExtraRules++];
    s->wt = wt;
  }
  ensure(nExtraRules < MAX_N_EXTRA_RULES);
  s->rule[wt->nExtraInflectRules++] = ruleIndex;
}

void WordLexicon::LoadWordRules() {
  std::ifstream in;
  if (!FixIfstream(in, lexiconDir, "inflection.lex", true))
    return;
  Message(MSG_STATUS, "loading word inflection rules...");
  char string[MAX_WORD_LENGTH], ruleName[10];
  //  while(in >> string >> ruleName) { // jonas, error for words like id'ehistoria, split into "id" "historia" with gcc 3.0 (not with gcc 2.95)
  while(in) {
    std::string tempstring;
    std::getline(in, tempstring);
    std::string::size_type pos = tempstring.find('\t');
    strncpy(string, tempstring.substr(0, pos).c_str(), MAX_WORD_LENGTH - 1);
    string[MAX_WORD_LENGTH-1] = 0;
    strncpy(ruleName, tempstring.substr(pos + 1).c_str(), 9);
    Word *w2 = Find(string);
    if (w2) {
      bool ok = false;
      for (WordTag *wt=w2; wt; wt = wt->Next()) {
	const Tag *t = wt->GetTag();
	if (t->OriginalTag()->IsRuleBase() ||
	    (t->IsRuleBase2() && wt->Lemma(0) && 
	     !strcmp(w2->String(), wt->Lemma(0)->String()))) {
	  uint index = inflects.FindRuleIndex(string, ruleName, t->OriginalTag()->Index());
	  if (index != INFLECT_NO_RULE) {
	    AddExtraRule(wt, index);
	    ok = true;
	  }
	}
      }
      if (!ok)
	Message(MSG_MINOR_WARNING, "inflection.lex, rule not applicable for word", w2->String());
    } else // just to mark the matching rule as used:
      inflects.FindRuleIndex(string, ruleName, TAG_INDEX_NONE);
  }
}

void WordLexicon::LoadStyleWords() {
  // this has become a rather messy method. please improve it if you feel inclined to
  if (!N_STYLEWORDS)
    return;
  Message(MSG_STATUS, "loading style words...");
  std::ifstream in;
  if (!FixIfstream(in, lexiconDir, "style")) {
    Message(MSG_WARNING, "cannot load stylewords");
    N_STYLEWORDS = 0;
    return;
  }
  int i;
  for (i=0; i<MAX_STYLES; i++)
    StyleWord::styles[i][0] = '\0';
  for (i=0; i<CW; i++)
    (*this)[i].style=0;
  //  words[i].style=0;
  char wordString[MAX_WORD_LENGTH], lemmaString[MAX_WORD_LENGTH];
  Tag tag;
  char style[100], paragraph[100];
  int k=0;
  while(in >> wordString >> tag.string >> lemmaString) {
    ensure(k < N_STYLEWORDS);
    if (wordString[0] == '#')
      continue;
    StyleWord &sw = stylewords[k];
    Word *w = Find(wordString);
    if (!w)
      Message(MSG_ERROR, "style: no such word", wordString);
    Tag *t = tags->Find(tag);
    if (!t)
      Message(MSG_ERROR, "style: no such tag", tag.String());
    WordTag *wt = w->GetWordTag(t);
    if (!wt)
      Message(MSG_ERROR, "style: no such word-tag", w->String(), t->String());
    wt->style = 1;
    sw.word = w;
    sw.wordTag = wt;
    if (!wt->Lemma(0)) {
      const Word *lw = Find(lemmaString);
      if (lw)
	wt->lemma = lw->GetWordTag(t->LemmaTag());
      if (!wt->lemma)
	Message(MSG_MINOR_WARNING, "style: no such lemma in the lexicon:",
		lemmaString, t->LemmaTag()->String());
    }
    do {
      in >> style;
      if (strlen(style) >= MAX_PARAGRAPH)
	Message(MSG_WARNING, "style: too long style type:", style);
      for (i=0; StyleWord::styles[i][0]; i++)
	if (!strcmp(StyleWord::styles[i], style))
	  break;
      ensure(i < MAX_STYLES);
      if (!StyleWord::styles[i][0])
	strcpy(StyleWord::styles[i], style);
      sw.style |= ((uint)1<<i);
      SkipSpaceButNotNewLine(in);
    } while (in.peek() == ';' && in.get());
    in >> paragraph;
    if (strlen(paragraph) >= MAX_PARAGRAPH)
      Message(MSG_WARNING, "style: too long paragraph name:", paragraph);
    strncpy(sw.paragraph, paragraph, MAX_PARAGRAPH-1);
    int j=0;
    char c;
    SkipSpaceButNotNewLine(in);
    while ((c = (char)in.peek()) != '#' && c != '\n') { // read alternatives
      if (j >= MAX_ALTERNATIVES)
	Message(MSG_ERROR, "style: too many alternatives for word on line", int2str(k+1));
      uint m;
      for (m=0; (c = (char)in.peek()) != ';' && c != '\n' && c != '#'; m++) {
	wordString[m] = (char) in.get();
	if (m >= MAX_WORD_LENGTH) {
	  wordString[m] = '\0';
	  Message(MSG_WARNING, "style: too long alternative word", wordString);
	  break;
	}
      }
      wordString[m] = '\0';
      while(IsSpace(wordString[m-1])) m--;
      ensure(m);
      wordString[m] = '\0';
      if (!strcmp(wordString, "-")) // means no alternative
	break;
      if (c == ';') in.get();
      SkipSpaceButNotNewLine(in);
      Word *ww = Find(wordString);
      if (!ww)
      {
	ww = new Word(wordString);  // jb: who is responsible for the memory?
	sw.owns_mem[j] = true;
      }
      sw.alt[j] = ww;
      j++;
    }
    sw.nAlts = j;
    if (in.peek() == '#') {
      in.get();
      char comment[1000];
      in.getline(comment, 999);
      sw.comment = commentBuf.NewString(comment);
      //sw.comment = new char[strlen(comment)+1];
      //strcpy(sw.comment, comment);
    }
    k++;
  }
  if (k != N_STYLEWORDS)
    Message(MSG_MINOR_WARNING, int2str(k), "words in style words file");
  stylewords.Hashify(false);
}

void WordLexicon::LoadSlow(const char *dir, const TagLexicon* tgs, NewWordLexicon *n) {
  tags = tgs;
  lexiconDir = dir;
  newWords = n;
  LoadInfo();
  AllocateMemory();
  inflects.LoadSlow(lexiconDir, tags);
  Message(MSG_STATUS, "loading word lexicon slow...");
  char *buff = strings;
  std::ifstream in;
  FixIfstream(in, lexiconDir, "cw");
  int i;
  for (i=0; i<CW; i++) {
    //  Word &w = words[i];
    Word &w = (*this)[i];
    w.Init();
    in >> w.freq;
    while (in.peek() == '\t') in.get();
    //    in.getline(buff, MAX_WORD_LENGTH);// jonas, doesn't work if length > MAX, neither did my fix, skip long words in lexicon
        std::string tempstring;
        std::getline(in, tempstring);
	if(tempstring.length() >= MAX_WORD_LENGTH) {
	  strncpy(buff, tempstring.c_str(), MAX_WORD_LENGTH-1);
	  buff[MAX_WORD_LENGTH-1] = 0;
	} else
	  strcpy(buff, tempstring.c_str());

    w.string = buff;
    uint len = strlen(buff);
    if (!ContainsVowel(w.string)) {
      w.compoundEndOK = 0;
      w.isForeign = 1;
      w.isSpellOK = 0;
    }
    if (len < 5)
      w.isSpellOK = 0;
    if (len < 3)
      w.compoundEndOK = 0;
    if (strchr(w.string, '-'))
      w.mayBeCapped = 1;
    buff += len+1;
    w.strLen = (uchar) len;
    if (w.freq == 0)
      w.extra = 1;
    wordsAlpha[i] = &w;
  }
  if (buff != strings + CL)
    Message(MSG_WARNING, int2str(buff - strings), "characters read from cw file");
  //  words.Hashify();
  Hashify();
  for (i=0; i<CW; i++) {
    char *string = (char*) (*this)[i].String();
    //    char *string = (char*) words[i].String();
    if (strchr(string, ' ')) {
      char str[MAX_WORD_LENGTH];
      strcpy(str, string);
      for (char *s = strtok(str, " "); s; s = strtok(NULL, " ")) {
	Word *w = Find(s);
	if (w)
	  if (s == str) w->collocation1 = 1;
	  else w->collocation23 = 1;
	else
	  Message(MSG_MINOR_WARNING, s, ", collocation part of", str, "not in lexicon");
      }
    }
  }
  qsort((char*)wordsAlpha, CW, sizeof(Word*), CompareWordPointers);
  CompressStrings();
  FixIfstream(in, lexiconDir, "cwtl");
  char wordString[MAX_WORD_LENGTH], lemmaString[MAX_WORD_LENGTH];
  Tag tag;
  int freq;
  int j = 0;
  int unknownTags = 0;
  for (i=0; i<CWT; i++) {
    in >> freq;
    while (in.peek() == '\t') in.get();
    //    in.getline(wordString, MAX_WORD_LENGTH, '\t');// jonas, doesn't work if length > MAX, neither did my fix, skip long words in lexicon
    std::string tempstring;
    std::getline(in, tempstring, '\t');
    strncpy(wordString, tempstring.c_str(), MAX_WORD_LENGTH-1);
    wordString[MAX_WORD_LENGTH-1] = 0;

    in >> tag.string;
    while (in.peek() == '\t') in.get();
    //    in.getline(lemmaString, MAX_WORD_LENGTH);// jonas, doesn't work if length > MAX, neither did my fix, skip long words in lexicon
    std::getline(in, tempstring);
    strncpy(lemmaString, tempstring.c_str(), MAX_WORD_LENGTH-1);
    lemmaString[MAX_WORD_LENGTH-1] = 0;

    Word *w = Find(wordString);
    if (!w)
      Message(MSG_ERROR, wordString, "in cwtl is an unknown word");
    if (strcmp(w->String(), wordString))
      Message(MSG_ERROR, wordString, "main lexicon must not contain capped words");
    const Tag *t = tags->Find(tag);
    if (!t) {
      Message(MSG_WARNING, tag.String(), "in cwtl is an unknown tag");
      unknownTags++;
      t = &(*tags)[0];
    }
    ensure(t);
    WordTag *wt;
    if (w->TagIndex() != TAG_INDEX_NONE) {
      for (wt = w; wt->Next(); wt=wt->Next());
      wt = wt->next = &more[j];
      wt->Init(w, false);
      j++;
      if (j > CMW)
	Message(MSG_ERROR, "too many word-tags in cwtl at line", int2str(i+1));
    } else
      wt = w;
    wt->tagIndex = t->Index();
    wt->tagFreq = freq;
    if (t->IsProperNoun())
      w->mayBeCapped = 1;
    if (t->IsProperNoun() && t->IsRuleBase())
      wt->inflectRule = inflects.FindRuleIndex(w->String(), "p1", t->Index());
    if (freq == 0) {
      wt->extraWordTag = 1;
      w->hasExtraWordTag = 1;
    }
  }
  ensure(j == CMW);
  for (i=0; i<CW; i++) {
    int f = 0;
    for (WordTag *q = &(*this)[i]; q; q=q->Next())
      //    for (WordTag *q = &words[i]; q; q=q->Next())
      f += q->tagFreq;
    if (f != (*this)[i].Freq())
      //    if (f != words[i].Freq())
      Message(MSG_ERROR, "sum of word-tag freqs wrong for word", (*this)[i].String());
    //  Message(MSG_ERROR, "sum of word-tag freqs wrong for word", words[i].String());
  }
  if (unknownTags)
    Message(MSG_WARNING, int2str(unknownTags), "unknown tags in cwtl");
  LoadLemmas();

  LoadWordRules();
  LoadStyleWords();
  LoadCompoundLists();
  LoadForeign();
  LoadVerbtypes();
}

void WordLexicon::LoadVerbtypes() {
  std::ifstream in;
  if (!FixIfstream(in, lexiconDir, "intransitivaverb", false)) {
    Message(MSG_WARNING, "cannot load intransitiva verb");
    return;
  }
  Message(MSG_STATUS, "loading intransitiva verb...");
  char string[MAX_WORD_LENGTH];
  while (in >> string) {
    Word *w = Find(string);
    if (w) {
      w->verbtype = 1;
      for (int k=0; k<w->NLemmas(); k++)
	for (int j=0; j<w->Lemma(k)->NInflectRules(); j++)
	  for (int i=0; i<tags->Ct(); i++)
	    if ((*tags)[i].IsVerb()) {
	      WordTag *wt2 = w->GetForm(&(*tags)[i], k, j);
	      if (wt2)
		wt2->verbtype = 1;
	    }
    }
  }
  if (!FixIfstream(in, lexiconDir, "bitransitivaverb", false)) {
    Message(MSG_WARNING, "cannot load bitransitiva verb");
    return;
  }
  Message(MSG_STATUS, "loading bitransitiva verb...");
  while (in >> string) {
    Word *w = Find(string);
    if (w) {
      w->verbtype = 2;
      for (int k=0; k<w->NLemmas(); k++)
	for (int j=0; j<w->Lemma(k)->NInflectRules(); j++)
	  for (int i=0; i<tags->Ct(); i++)
	    if ((*tags)[i].IsVerb()) {
	      WordTag *wt2 = w->GetForm(&(*tags)[i], k, j);
	      if (wt2)
		wt2->verbtype = 2;
	    }
    }
  }
  if (!FixIfstream(in, lexiconDir, "feminina", false)) {
    Message(MSG_WARNING, "cannot load feminin nouns");
    return;
  }
  Message(MSG_STATUS, "loading feminin nouns...");
  while (in >> string) {
    Word *w = Find(string);
    if (w) {
      w->verbtype = 3;
      for (int k=0; k<w->NLemmas(); k++)
	for (int j=0; j<w->Lemma(k)->NInflectRules(); j++)
	  for (int i=0; i<tags->Ct(); i++)
	    if ((*tags)[i].IsNoun()) {
	      WordTag *wt2 = w->GetForm(&(*tags)[i], k, j);
	      if (wt2)
		wt2->verbtype = 3;
	    }
    }
  }
  if (!FixIfstream(in, lexiconDir, "opt_space_words", false)) {
    Message(MSG_WARNING, "cannot load opt-space-words nouns");
    return;
  }
  Message(MSG_STATUS, "loading opt-space-words nouns...");
  while (in.getline(string, 100)) {
    Word *w = Find(string);
    if (w) {
      w->optSpace = 1;
    }
  }
}

void WordLexicon::LoadForeign() {
  std::ifstream in;
  if (!FixIfstream(in, lexiconDir, "foreign.w", false)) {
    Message(MSG_WARNING, "cannot load foreign.w");
    return;
  }
  Message(MSG_STATUS, "loading foreign words...");
  char string[MAX_WORD_LENGTH];
  while (in >> string) {
    Word *w = Find(string);
    if (w)
      w->isForeign = 1;
    strcat(string, "s");
    w = Find(string);
    if (w)
      w->isForeign = 1;
  }
  if (!FixIfstream(in, lexiconDir, "spellNotOK", false)) {
    Message(MSG_WARNING, "cannot load spellNotOK");
    return;
  }
  Message(MSG_STATUS, "loading spellNotOK words...");
  while (in.getline(string, MAX_WORD_LENGTH)) {
    Word *w = Find(string);
    if (w)
      w->isSpellOK = 0;
  }
  if (!FixIfstream(in, lexiconDir, "spellOK", false)) {
    Message(MSG_WARNING, "cannot load spellOK");
    return;
  }
  Message(MSG_STATUS, "loading spellOK words...");
  while (in.getline(string, MAX_WORD_LENGTH)) {
    Word *w = Find(string);
    if (w)
      w->isSpellOK = 1;
  }
}
  
void WordLexicon::LoadCompoundLists() {
  std::ifstream in;
  char string[MAX_WORD_LENGTH];
  if (!FixIfstream(in, lexiconDir, "compound-end-stop.w", false)) {
    Message(MSG_ERROR, "cannot load compound-end-stop.w");
    return;
  }
  while (in >> string) {
    Word *w = Find(string);
    if (w) w->compoundEndOK = 0;
  }
  if (!FixIfstream(in, lexiconDir, "compound-begin-ok.w", false)) {
    Message(MSG_ERROR, "cannot load compound-begin-ok.w");
    return;
  }
  while (in >> string) {
    Word *w = Find(string);
    if (w) w->compoundBeginOK = 1;
  }
}

Word **WordLexicon::GetWordsInRange(const char *s, int *n) const {
  int min = 0;
  int max, max2;
  max = max2 = CW-1;
  const int len = strlen(s);
  while(min <= max) {
    int mid = (max+min)/2;
    int cmp = strncmp(s, wordsAlpha[mid]->String(), len);
    if (cmp > 0)
      min = mid+1;
    else {
      max = mid-1;
      if (cmp < 0)
	max2 = max;
    }
  }
  int min2 = max;
  if (min2 < 0)
    min2 = 0;
  while(min2 <= max2) {
    int mid = (max2+min2)/2;
    if (strncmp(s, wordsAlpha[mid]->String(), len) >= 0)
      min2 = mid+1;
    else
      max2 = mid-1;
  }
  *n = max2 - min + 1;
  return &wordsAlpha[min];
}



void WordLexicon::SetPointersFromIndices() {
  Message(MSG_STATUS, "setting word pointers...");
  int i;
  for (i=0; i<CW; i++) {
    Word *w = &(*this)[i];
    //    Word *w = &words[i];
    w->string = strings + (size_t)w->string;
    WordTag *wt;
    for (wt = w; wt; wt=wt->Next()) {
      if (wt->lemma == (WordTag*) -1)
	wt->lemma = NULL;
      else if (((size_t)wt->lemma) < CW)
	wt->lemma = &(*this)[(size_t)wt->lemma];
      //	wt->lemma = &words[(int)wt->lemma];
      else
	wt->lemma = &more[((size_t)wt->lemma) - CW];
      if (wt->next == (WordTag*) -1)
	wt->next = w;
      else
	wt->next = &more[(size_t)wt->next];
    }
    wordsAlpha[i] = &(*this)[(size_t)wordsAlpha[i]];
    //    wordsAlpha[i] = &words[(int)wordsAlpha[i]];
  }
  for (i=0; i<nExtraLemmas; i++) {
    ExtraLemma &wtal = extraLemmas[i];
    if (((size_t)wtal.wt) < CW)
      wtal.wt = &(*this)[(size_t)wtal.wt];
    //      wtal.wt = &words[(int)wtal.wt];
    else
      wtal.wt = &more[((size_t)wtal.wt) - CW];
    if (((size_t)wtal.lemma) < CW)
      wtal.lemma = &(*this)[(size_t)wtal.lemma];
    //      wtal.lemma = &words[(int)wtal.lemma];
    else
      wtal.lemma = &more[((size_t)wtal.lemma) - CW];
  }
  for (i=0; i<nExtraRules; i++) {
    ExtraRules &wtar = extraRules[i];
    if (((size_t)wtar.wt) < CW)
      wtar.wt = &(*this)[(size_t)wtar.wt];
    //      wtar.wt = &words[(int)wtar.wt];
    else
      wtar.wt = &more[((size_t)wtar.wt) - CW];
  }
}

bool WordLexicon::LoadFast(const char *dir, const TagLexicon *tgs, NewWordLexicon *n, bool warn) {
  tags = tgs;
  lexiconDir = dir;
  newWords = n;
  std::ifstream in;
  if (!FixIfstream(in, lexiconDir, "fast", warn))
    return false;
  if (!CheckVersion(in, xVersion)) {
    Message(MSG_WARNING, "fast word lexicon file obsolete");
    return false;
  }
  Message(MSG_STATUS, "loading word lexicon fast...");
  ReadVar(in, CW);
  ReadVar(in, CWT);
  ReadVar(in, CL);
  ReadVar(in, N_STYLEWORDS);
  ReadVar(in, nExtraRules);
  AllocateMemory();
  Load(in);
  //  words.Load(in);
  ReadData(in, (char*)more, sizeof(WordTag)*CMW, "more");
  ReadData(in, strings, CL, "strings");
  ReadVar(in, nExtraLemmas);
  ReadData(in, (char*)extraLemmas, sizeof(ExtraLemma)*nExtraLemmas, "wordtaglemmas");
  inflects.LoadFast(in);
  ReadData(in, (char*)extraRules, sizeof(ExtraRules)*nExtraRules, "extrarules");
  ReadData(in, (char*)wordsAlpha, sizeof(Word*)*CW, "wordsalpha");

  SetPointersFromIndices();
  LoadStyleWords();
  if (xWarnAll)
    for (int i=0; i<CW-1; i++)
      if (strcmp(wordsAlpha[i]->String(), wordsAlpha[i+1]->String()) > 0)
	std::cout << wordsAlpha[i] << ' ' << wordsAlpha[i+1] << std::endl;
  return true;
}

bool WordLexicon::Save() {
  std::ifstream in;
  if (FixIfstream(in, lexiconDir, "fast", false))
    return false;
  Message(MSG_STATUS, "saving fast word lexicon...");
  in.close();
  int i;
  for (i=0; i<CW; i++) {
    WordTag *wt, *next;
    (*this)[i].string = (*this)[i].string - (size_t)strings;
    //    words[i].string = words[i].string - (uint)strings;
    for (wt=&(*this)[i]; wt; wt=next) {
      //    for (wt=&words[i]; wt; wt=next) {
      next = wt->Next();
      if (next)
	wt->next = (WordTag*) (wt->next - &more[0]); // i.e. index of wt->next in more
      else
	wt->next = (WordTag*) -1;
      if (wt->lemma)
	if (wt->lemma->IsWord()) {
	  ensure((int)((Word*)wt->lemma - &(*this)[0]) < CW);
	  //	  ensure((int)((Word*)wt->lemma - &words[0]) < CW);
	  wt->lemma = (WordTag*) ((Word*)wt->lemma - &(*this)[0]);
	  //	  wt->lemma = (WordTag*) ((Word*)wt->lemma - &words[0]);
	} else
	  wt->lemma = (WordTag*) (CW + (wt->lemma - &more[0]));
      else
	wt->lemma = (WordTag*)-1;
    }
    wordsAlpha[i] = (Word*) (wordsAlpha[i] - &(*this)[0]);
    //    wordsAlpha[i] = (Word*) (wordsAlpha[i] - &words[0]);
  }
  for (i=0; i<nExtraLemmas; i++) {
    ExtraLemma &wtal = extraLemmas[i];
    if (wtal.lemma->IsWord()) {
      ensure((int)((Word*)wtal.lemma - &(*this)[0]) < CW);
      //      ensure((int)((Word*)wtal.lemma - &words[0]) < CW);
      wtal.lemma = (WordTag*) ((Word*)wtal.lemma - &(*this)[0]);
      //      wtal.lemma = (WordTag*) ((Word*)wtal.lemma - &words[0]);
    } else
      wtal.lemma = (WordTag*) (CW + (wtal.lemma - &more[0]));
    if (wtal.wt->IsWord()) {
      ensure((int)((Word*)wtal.wt - &(*this)[0]) < CW);
      //      ensure((int)((Word*)wtal.wt - &words[0]) < CW);
      wtal.wt = (WordTag*) ((Word*)wtal.wt - &(*this)[0]);
      //      wtal.wt = (WordTag*) ((Word*)wtal.wt - &words[0]);
    } else
      wtal.wt = (WordTag*) (CW + (wtal.wt - &more[0]));
  }
  for (i=0; i<nExtraRules; i++) {
    ExtraRules &wtar = extraRules[i];
    if (wtar.wt->IsWord()) {
      ensure((int)((Word*)wtar.wt - &(*this)[0]) < CW);
      //      ensure((int)((Word*)wtar.wt - &words[0]) < CW);
      wtar.wt = (WordTag*) ((Word*)wtar.wt - &(*this)[0]);
      //      wtar.wt = (WordTag*) ((Word*)wtar.wt - &words[0]);
    } else
      wtar.wt = (WordTag*) (CW + (wtar.wt - &more[0]));
  }
  std::ofstream out;
  FixOfstream(out, lexiconDir, "fast");
  SetVersion(out, xVersion);
  WriteVar(out, CW);
  WriteVar(out, CWT);
  WriteVar(out, CL);
  WriteVar(out, N_STYLEWORDS);
  WriteVar(out, nExtraRules);
  //  words.Store(out);
  Store(out);
  WriteData(out, (char*)more, sizeof(WordTag)*CMW, "more");
  WriteData(out, strings, CL, "strings");
  WriteVar(out, nExtraLemmas);
  WriteData(out, (char*)extraLemmas, sizeof(ExtraLemma)*nExtraLemmas, "wordtaglemmas");
  inflects.Save(out);
  WriteData(out, (char*)extraRules, sizeof(ExtraRules)*nExtraRules, "extrarules");
  WriteData(out, (char*)wordsAlpha, sizeof(Word*)*CW, "wordsalpha");

  SetPointersFromIndices();
  return true;
}

void WordLexicon::CompressStrings() {
  Message(MSG_STATUS, "compressing word strings...");
  // assuming newWord-bit is 0 for all words
  // mark words that are are subwords:
  int i;
  for (i=0; i<CW; i++)
    for (const char *s = (*this)[i].string+1; *s; s++) {
      //    for (const char *s = words[i].string+1; *s; s++) {
      Word *w = Find(s);
      if (w) w->newWord = 1;
    }
  char *strings2, *s = strings2 = new char[CL]; // new OK // a new place to put the strings
  if (!strings2) {
    Message(MSG_WARNING, "not enough memory to compress strings");
    return;
  }
  // move all words that are not subwords:
  for (i=0; i<CW; i++) {
    //    if (!words[i].newWord) {
    //  strcpy(s, words[i].string);
    //  words[i].string = s;
    //  s += words[i].strLen + 1;
    // }
    if (!(*this)[i].newWord) {
      strcpy(s, (*this)[i].string);
      (*this)[i].string = s;
      s += (*this)[i].strLen + 1;
    }
  }
  // move all subwords:
  for (i=0; i<CW; i++)
    if (!(*this)[i].newWord)
      for (const char *ss = (*this)[i].string+1; *ss; ss++) {
	//    if (!words[i].newWord)
	//      for (const char *ss = words[i].string+1; *ss; ss++) {
	Word *w= Find(ss);
	if (w && w->newWord) {
	  w->string = (char*) ss;
	  w->newWord = 0;
	}
      }
  // std::cerr<<" (saved "<<CL-(s-strings2)<<" of "<<CL<<" bytes, "<<100.0*(CL-(s-strings2))/CL<<"%)";
  ExtByt(-CL);
  CL = s-strings2+1; // means a smaller string buffer when fast loading
  ExtByt(CL);
  delete strings;
  strings = strings2;
}

void WordLexicon::PrintStatistics() const {
  std::cout<<"word lexicon statistics: "<<std::endl;
  //  words.Statistics();
  Statistics();
}

void WordLexicon::Print() const {
  for (int i=0; i<CW; i++)
    std::cout << (*this)[i] << std::endl;
}

int WordLexicon::CompoundAnalyze(NewWord *w) const {
  // returns number of suffixes added to w
  w->isCompoundAnalyzed = 1;
  const uint len = w->StringLen();
  if (len < xCompoundMinLength)
    return 0;
  Word *bestSuffix = NULL;
  char *suffString, prefString[MAX_WORD_LENGTH];
  strcpy(prefString, w->String());
  for (uint sufLen=len-xCompoundPrefixMinLength;
       sufLen>=xCompoundSuffixMinLength; sufLen--) {
    uint preLen = len - sufLen;
    suffString = w->string + preLen;
    Word *s = Find(suffString);
    if (s && s->IsCompoundEndOK()) {
      strncpy(prefString, w->String(), preLen);
      prefString[preLen]= '\0';
      Word *p = Find(prefString);
      if (!bestSuffix)
	if (!p || p->IsCompoundBeginOK())
	  bestSuffix = s;
      if (p && p->IsCompoundBeginOK()) {
	w->AddSuffixes(s);
	if (suffString[0] != 's') 
	  goto plopp;
      } else if (w->string[preLen-1] == 's') {
	prefString[preLen-1] = '\0';
	p = Find(prefString);
	if (p && p->IsCompoundBeginOK()) {
	  w->AddSuffixes(s);
	  goto plopp;
	}
      } else if (w->string[preLen-1] == '-') {
	prefString[preLen-1] = '\0';
	p = Find(prefString);
	if (p) {
	  w->AddSuffixes(s);
	  goto plopp;
	}
      }
    }
  }
  if (!xCompoundRequirePrefix && bestSuffix)
    w->AddSuffixes(bestSuffix);
 plopp:
  return w->NSuffixes();
}

bool WordLexicon::AddTagsDerivedFromLemma(NewWord *w, WordTag *lemma) const {
    bool ok = false;
    //  std::cout << "adding word-tags to " << w << " derivable from " << lemma << std::endl;
    
    for (int j=0; j<lemma->NInflectRules(); j++)
    {
	// jbfix: if no rule found, exit gracefully
	uint ir = lemma->InflectRule(j);
	if(ir == INFLECT_NO_RULE)
	    return false;
	const InflectRule &r = inflects.Rule(ir);
	
	for (int i=0; i<r.NForms(); i++) 
	{
	    const char *ss = r.Apply(lemma->String(), i);
	    if (ss && !strcmp(ss, w->String()) &&
		r.TagIndex(i) != TAG_INDEX_NONE && 
		!(*tags)[r.TagIndex(i)].IsSms()) 
	    {
		WordTag *wt = newWords->AddWordTag(w, &(*tags)[r.TagIndex(i)]);
		//	std::cout << wt << " from " << lemma << std::endl;
		wt->lemma = lemma;
		w->isDerived = 1;
		ok = true;
		if (i == 0) // i.e. lemma
		    wt->inflectRule = lemma->InflectRule(j);
	    }
	}
    }
    //  std::cout << std::endl;
    return ok;
}

/*
int WordLexicon::CheckForWordTags(NewWord *nw, Word *w) const {
  static NewWord *prev = NULL;
  static Word *checked[200];
  static int n = 0;
  static int ok = 0;
  if (nw != prev) {
    prev = nw;
    ok = 0;
    for (int i=0; i<n; i++)
      checked[i]->suggested = 0;
    n = 0;
  }
  for (WordTag *wt = w; wt; wt=wt->Next())
    if (wt->IsLemma())
      if (AddTagsDerivedFromLemma(nw, wt))
	ok++;
  return ok;
}
*/

void WordLexicon::AnalyzeNewWord(NewWord *w, bool tryHard) const {
  // tries to determine w:s lemma and inflection rule
  if (!w->String()) return; // line added by Viggo 1999-09-27
  CompoundAnalyze(w);
  char string[MAX_WORD_LENGTH];
  Word *checkedWords[MAX_WORD_LENGTH];
  int nChecked = 0;
  int oks = 0;
  int i;
  //  std::cout << "check suffix" << std::endl;
  for (i=0; i<w->NSuffixes(); i++) {
    const WordTag *suffix = w->Suffix(i);
    ensure(suffix);
    uint len = w->StringLen() - suffix->GetWord()->StringLen();
    strncpy(string, w->String(), len);
    for (int j=0; j<suffix->NLemmas(); j++) {
      const WordTag *suffixLemma = suffix->Lemma(j);
      strcpy(string+len, suffixLemma->String());  // string is the wanted lemma-string now
      const Tag *lemmaTag = suffixLemma->GetTag()->OriginalTag();
      Word *w2 = Find(string);
      if (w2) {     // main lexicon lemma word exists
	checkedWords[nChecked++] = w2;
	w2->suggested = 1;
	WordTag *lemma = w2->GetWordTag(lemmaTag);
	if (lemma)
	  if (AddTagsDerivedFromLemma(w, lemma))
	    oks++;
      } else {
	NewWord *w3 = newWords->Find(string);
	if (!w3) {
	  w3 = newWords->AddWord(string);
	  w3->isAnalyzed = 1;
	}
	checkedWords[nChecked++] = w3;
	w3->suggested = 1;
	WordTag *lemma = w3->GetWordTag(lemmaTag);
	if (!lemma)
	  lemma = newWords->AddWordTag(w3, lemmaTag);
	lemma->lemma = lemma; // funny
	for (int k=0; k<suffixLemma->NInflectRules(); k++) {
	  lemma->inflectRule = suffixLemma->InflectRule(k);
	  if (AddTagsDerivedFromLemma(w, lemma))
	    oks++;
	}
      }
    }
    if (suffix->NLemmas() <= 0) {
      newWords->AddWordTag(w, suffix->GetTag());
      Message(MSG_MINOR_WARNING, suffix->String(),
	      suffix->GetTag()->String(), "has no lemma");
    }
  }
  if (oks < 4) {
    //std::cout << "check prefix" << std::endl;
    strcpy(string, w->String());
    int start = w->StringLen()-1;
    int stop = start - MAX_INFLECTION_CHARS_ON_NORMAL_WORD + 1;
    if (stop < MIN_PREFIX_CHARS_ON_NORMAL_WORD)
      stop = MIN_PREFIX_CHARS_ON_NORMAL_WORD;
    for (i=start; i>=stop; i--) {
      string[i] = '\0';
      //      std::cout << " test " << string << std::endl;
      Word *w2 = Find(string);
      if (!w2) {
	NewWord *w3 = newWords->Find(string);
	if (w3)
	  if (!w3->IsSuggested()) {
	    if (!w3->IsAnalyzed())
	      AnalyzeNewWord(w3);
	    w2 = w3;
	  }
      } else {
	if (w2->IsSuggested()) continue;
	checkedWords[nChecked++] = w2;
	w2->suggested = 1;
      }
      for (WordTag *wt2=w2; wt2; wt2=wt2->Next())
	if (wt2->IsLemma())
	  if (AddTagsDerivedFromLemma(w, wt2))
	    oks++;
    }
    Word **ws = NULL;
    int n = 0;
    if (oks < 4) {
      //std::cout << "check range" << std::endl;
      strcpy(string, w->String());
      uint min = w->StringLen()+2;
      for (i=start; i>=stop && oks < 2; i--) {
	string[i] = '\0';
	ws = GetWordsInRange(string, &n);
	//    std::cout << "range on " << string << " = " << n << std::endl;
	if (n > 20) break;
	for (int j=0; j<n; j++) {
	  Word *w2 = ws[j];
	  if (w2->IsSuggested()) continue;
	  w2->suggested = 1;
	  if (w2->StringLen() <= min)
	    for (WordTag *wt2=w2; wt2; wt2=wt2->Next()) {
	      //	  std::cout << w2 << ' ' << wt2->IsLemma() << "len: " << w2->StringLen() << std::endl;
	      if (wt2->IsLemma())
		if (AddTagsDerivedFromLemma(w, wt2)) {
		  oks++;
		  if (oks >= 2)
		    break;
		}
	    }
	}
      }
    }
    for (i=0; i<n; i++)
      ws[i]->suggested = 0;
  }
  for (i=0; i<nChecked; i++)
    checkedWords[i]->suggested = 0;
  if (!w->IsCompound() && !w->IsDerived()) {
    if (w->TextFreq() > 3 && w->IsAlwaysCapped()) {
      if (w->String()[w->StringLen()-1] == 's')
	newWords->AddWordTag(w, tags->SpecialTag(TOKEN_PROPER_NOUN_GENITIVE));
      else {
	WordTag *wt2 = newWords->AddWordTag(w, tags->SpecialTag(TOKEN_PROPER_NOUN));
	//	std::cerr << wt2 << std::endl;
	GuessWordTagRule(wt2);
      }
    }
    int best = -1;
    const InflectRule *bestRule = NULL;
    for (uint k=0; k<Inflects().NRules(); k++) {
      const InflectRule &r = Inflects().Rule(k);
      if (w->IsAlwaysCapped() && strcmp(r.Name(), "p1"))
	continue;
      if (r.IsApplicable(w->String(), TAG_INDEX_NONE)) {
	int nFormsFound = 0;
	for (int j=1; j<r.NForms(); j++) {
	  const char *s = r.Apply(w->String(), j);
	  if (s && strcmp(w->String(), s)) {
	    NewWord *w2 = newWords->Find(s);
	    if (w2)
	      nFormsFound++;
	  }
	}
	if (nFormsFound > best) {
	  if (tryHard) {
	    newWords->AddWordTag(w, &(*tags)[r.TagIndex(0)]);
	    w->lemma = w;
	    w->inflectRule = k;
	  }
	  bestRule = &r;
	  best = nFormsFound;
	}
      }
    }
    if (best > 0) {
      //std::cerr << w << ' ' << bestRule << std::endl;
      for (int j=1; j<bestRule->NForms(); j++) {
	const char *s = bestRule->Apply(w->String(), j);
	if (s && strcmp(w->String(), s) && bestRule->TagIndex(j) != TAG_INDEX_NONE) {
	  NewWord *w2 = newWords->Find(s);
	  if (w2)
	    newWords->AddWordTag(w2, &(*tags)[bestRule->TagIndex(j)]);
	}
      } 
    }
  }
  //  if (w->IsLemma() && w->NInflectRules() == 0)
  //    std::cerr << "no rule for " << w << std::endl;
  w->isAnalyzed = 1;
}

void WordLexicon::AnalyzeNewWords() const {
  Message(MSG_STATUS, "analyzing new words...");
  NewWord *w;
  for (int i=0; (w = (*newWords)[&i]); i++)
    if (!w->IsAnalyzed())
      AnalyzeNewWord(w);
}

void WordLexicon::GuessWordTagRule(WordTag *wt) const {
  //  Message(MSG_STATUS, "guessing rule for word", wt->String());
  const Tag *t = wt->GetTag();
  NewWord *nw = wt->GetWord()->IsNewWord() ? (NewWord*)wt->GetWord() : NULL;
  if (nw) {
    if (!nw->IsCompoundAnalyzed())
      Message(MSG_WARNING, nw->String(), "not compound analyzed");
    for (int i=0; i<nw->NSuffixes(); i++) {
      const WordTag *suffix = nw->Suffix(i);
      if (suffix->GetTag() == t && suffix->inflectRule != INFLECT_NO_RULE) {
	wt->inflectRule = suffix->inflectRule;
	return;
      }
    }
  }
  uint bestRule = INFLECT_NO_RULE;
  float best = -1;
  const char *prevName = "";
  for (uint i=0; i<Inflects().NRules(); i++) {
    const InflectRule &r = Inflects().Rule(i);
    if (strcmp(r.Name(), prevName))
      if (r.IsApplicable(wt->String(), t->Index())) {
	prevName = r.Name();
	float nFormsFound = 0.0f;
	for (int j=1; j<r.NForms(); j++) {
	  const char *string = r.Apply(wt->String(), j);
	  if (string) {
	    Word *w3 = Find(string);
	    if (w3) {
	      const WordTag *wt3 = w3->GetWordTag(r.TagIndex(j));
	      if (wt3) {
		int k;
		for (k=0; k<wt3->NLemmas(); k++)
		  if (wt3->Lemma(k)->GetWord() == wt->GetWord()) {
		    nFormsFound += 1.0f;
		    break;
		  }
		if (k >= wt3->NLemmas())
		  nFormsFound += 0.1f;
	      }// else
	      //	nFormsFound -= 0.4f;
	    }
	  }
	}
	if (nFormsFound > best) {
	  bestRule = i;
	  best = nFormsFound;
	}
      }
  }
  //  if (bestRule != INFLECT_NO_RULE) 
  //    std::cout << wt << tab << Inflects().Rule(bestRule).Name() << std::endl; 
  wt->inflectRule = bestRule;
}

uint WordLexicon::ExtraInflectRule(const WordTag *wt, int n) const {
  if (n<1 || n >= MAX_INFLECTION_RULES_PER_WORDTAG)
    Message(MSG_ERROR, "ExtraInflectRule called with", int2str(n));
  if (nExtraRules > 0) {
    int mid, min = 0, max = nExtraRules-1, cmp;
    do {
      mid = (min+max)/2;
      ensure(extraRules[mid].wt);
      cmp = strcmp(extraRules[mid].wt->String(), wt->String());
      if (cmp < 0) min = mid+1;
      else if (cmp > 0) max = mid-1;
      else break;
    } while (min <= max);
    if (cmp) {
      Message(MSG_WARNING, "no extra inflect rule for", wt->String());
      return INFLECT_NO_RULE;
    }
    if (extraRules[mid].wt == wt)
      return extraRules[mid].rule[n-1];
    else if (mid>0 && extraRules[mid-1].wt == wt)
      return extraRules[mid-1].rule[n-1];
    else if (mid<nExtraRules-1 && extraRules[mid+1].wt == wt)
      return extraRules[mid+1].rule[n-1];
    //Message(MSG_ERROR, "problem in ExtraInflectRule(), johan must fix this", wt->String());
    Message(MSG_WARNING, "problem in ExtraInflectRule(), johan must fix this", wt->String());
    return INFLECT_NO_RULE;
  }
  Message(MSG_ERROR, "cannot find extra inflect rule for", wt->String());
  return INFLECT_NO_RULE;
}

WordTag *WordLexicon::GetInflectedForm(WordTag* wt, uint ruleIndex, const Tag* t) const {
  if (!wt->IsLemma())
    Message(MSG_MINOR_WARNING, "GetInflectedForm():", wt->String(), "is not a lemma");
  const char *string = inflects.GetInflectedForm(wt->String(), ruleIndex, t->Index());
  if (string) {
    Word *w = Find(string);
    if (w) {
      WordTag *wt2 = w->GetWordTag(t);
      if (wt2)  // if wanted form exists, it is returned
	return wt2;
    }
    // or else a new word is used:
    NewWord *nw = newWords->Find(string);
    if (!nw) {
      nw = newWords->AddWord(string, t);
      nw->lemma = wt->GetWord();
      nw->isAnalyzed = 1;
      return nw;
    }
    WordTag *wt2 = nw->GetWordTag(t);
    if (wt2)
      return wt2;
    wt2 = newWords->AddWordTag(nw, t);
    wt2->lemma = wt;
    return wt2;
  }
  return NULL;
}



void WordLexicon::GenerateInflections(bool onlyUnknownWordTags) const {
  Message(MSG_STATUS, "generating inflections...");
  for (int i=0; i<CW; i++) {
    const Word* w = &(*this)[i];
    //    const Word* w = &words[i];
    for (const WordTag *wt = w; wt; wt = wt->Next())
      if (!wt->GetTag()->IsProperNoun())
	for (int j=0; j<wt->NInflectRules(); j++) {
	  uint ruleIndex = wt->InflectRule(j);
	  bool found = false;
	  int nOK = 0;
	  for (int k=0; k<tags->Ct(); k++) {
	    const char *string = inflects.GetInflectedForm(w->String(), ruleIndex, (*tags)[k].Index());
	    if (string) {
	      found = true;
	      if (!onlyUnknownWordTags)
		std::cout << w << tab << string << tab << (*tags)[k];
	      Word *w3;
	      if ((w3 = Find(string))) {
		const WordTag *wt3 = w3->GetWordTag(&(*tags)[k]);
		if (wt3) {
		  int m;
		  for (m=0; m<wt3->NLemmas(); m++)
		    if (wt3->Lemma(m)->GetWord() == w) {
		      nOK++;
		      if (!onlyUnknownWordTags)
			std::cout << tab << '*';
		      break;
		    }
		  if (m >= wt3->NLemmas()) {
		    if (!onlyUnknownWordTags)
		      std::cout << tab << "& " << wt3->Lemma(0);
		  }
		} else {
		  const WordTag *u;
		  for (u=w3; u; u=u->Next())
		    if (u->GetTag()->OriginalTag() == &(*tags)[k]) {
		      nOK++;
		      if (!onlyUnknownWordTags)
			std::cout << tab << '*';
		      break;
		    }
		  if (!u) {
		    if (onlyUnknownWordTags)
		      std::cout << string << tab << (*tags)[k] << tab << w << tab << "GI" << std::endl;
		    else {
		      std::cout << tab << '~';
		      for (u=w3; u; u=u->Next())
			std::cout << u->GetTag() << tab;
		    }
		  }
		}
	      }
	      if (!onlyUnknownWordTags)
		std::cout << std::endl;
	    }
	  }
	  if (!onlyUnknownWordTags) {
	    if (nOK < 2)
	      std::cout << w << tab << '§' << std::endl;
	    if (!found)
	      std::cout << w << " no inflections found" << std::endl;
	  }
	}
  }
}

void WordLexicon::TestInflections() const {
  xPrintWordInfo = true;
  bool showApplicable = false;
  char string[MAX_WORD_LENGTH];
  while (std::cin) {
    std::cout << std::endl << "enter word (or '-' to toggle show applicable rules): ";
    std::cin >> string;
    if (string[0] == '\0' || !std::cin) {
      break;
    }
    if (string[0] == '-') {
      showApplicable = !showApplicable;
      continue;
    }
    AnalyzeWordAndPrintInflections(string);
    if (showApplicable) {
      std::cout << tab << "all applicable rules:" << std::endl;
      const char *prevName = "";
      for (uint i=0; i<inflects.NRules(); i++) {
	const InflectRule &r = inflects.rules[i];
	for (int k=0; k<tags->Ct(); k++)
	  if (strcmp(r.Name(), prevName))
	    if (r.IsApplicable(string, (*tags)[k].Index())) {
	      prevName = r.Name();
	      std::cout << r.Name() << " (" << i << ')';
	      for (int j=1; j<r.NForms(); j++) {
		const char *s = r.Apply(string, j);
	      if (s)
		std::cout << ' ' << s;
	      }
	      std::cout << std::endl;
	    }
      }
    } 
  }
}

void WordLexicon::AnalyzeWordAndPrintInflections(const char *string,
						 const char *newline) const {
  Word *w = Find(string);
  if (w) {
    std::cout << tab << "all forms found in lexicon:" << newline;
    for (const WordTag *wt=w; wt; wt=wt->Next())
      wt->Print(std::cout);
  } else {
    std::cout << string << " is not in main lexicon, analyzing..." << newline;
    NewWord *nw = newWords->AddWord(string);
    AnalyzeNewWord(nw, true);
    nw->Print();
    for (WordTag *wt=nw; wt; wt=wt->Next())
      if (wt->NLemmas() == 0) {
	wt->Print(std::cout);
	std::cout << newline;
      }
    w = nw;
  }
  for (const WordTag *wt=w; wt; wt=wt->Next()) {
    std::cout << tab << "all forms that can be generated from " << wt << ':' << newline;
    for (int k=0; k<wt->NLemmas(); k++)
      for (int j=0; j<wt->Lemma(k)->NInflectRules(); j++)
	for (int i=0; i<tags->Ct(); i++) {
	  const WordTag *wt2 = wt->GetForm(&(*tags)[i], k, j);
	  if (wt2) {
	    std::cout << wt2;
	    if (wt2->GetWord()->IsNewWord())
	      std::cout << " *";
	    std::cout << newline;
	  }
	}
  }
}

char *WordLexicon::GetInflectionList(const char *string, char *result) const {
  Word *w = Find(string);
  if (!w) {
    NewWord *nw = newWords->AddWord(string);
    AnalyzeNewWord(nw, true);
    w = nw;
  }
  result[0] = '\0';
  char res[100];
  for (const WordTag *wt=w; wt; wt=wt->Next()) {
    for (int k=0; k<wt->NLemmas(); k++)
      for (int j=0; j<wt->Lemma(k)->NInflectRules(); j++)
	for (int i=0; i<tags->Ct(); i++) {
	  const WordTag *wt2 = wt->GetForm(&(*tags)[i], k, j);
	  if (wt2) {
	    strcpy(res, wt2->String());
	    strcat(res, " ");
	    if (!strstr(result, res))
	      strcat(result, res);
	  }
	}
  }
  return result;
}

#ifdef DEVELOPERS
/* jonas, from obsolete tagger code */
void WordLexicon::GenerateExtraWordTags() const {
  Message(MSG_STATUS, "generating extra word tags...");
  for (int i=0; i<CW; i++) {
    const Word* w = &(*this)[i];
    //    const Word* w = &words[i];
    for (const WordTag *wt = w; wt; wt = wt->Next())
      if (!wt->GetTag()->IsProperNoun())
	for (int j=0; j<wt->NInflectRules(); j++) {
	  uint ruleIndex = wt->InflectRule(j);
	  for (int k=0; k<tags->Ct(); k++) {
	    const char *string = inflects.GetInflectedForm(w->String(), ruleIndex, (*tags)[k].Index());
	    if (string) {
	      Word *w3 = Find(string);
	      if (w3) {
		const WordTag *wt3 = w3->GetWordTag(&(*tags)[k]);
		if (!wt3)
		  for (WordTag *wt4 = w3; wt4; wt4 = wt4->Next())
		    if (wt4->GetTag()->OriginalTag() == &(*tags)[k]) {
		      wt3 = wt4;
		      break;
		    }
		if (!wt3)
		  std::cout << w3 << tab << (*tags)[k] << tab << w << std::endl;
	      } else if (!strcmp(Inflects().Rule(ruleIndex).Name(), "v5") && 
			 Inflects().Rule(ruleIndex).TagIndex(k) != TAG_INDEX_NONE)
		std::cout << string << tab << (*tags)[k] << tab << w << std::endl;
	    }
	  }
	}
  }
}


/* Is supposed to do the same thing as TestInflections, but
   for the server version */

void WordLexicon::ServerAnalyzeWordAndPrintInflections(std::iostream &socket,
						       const char *string
						       ) const {
  Word *w = Find(string);
  if (w) {
    socket << tab << "all forms found in lexicon:" << "\n";
    for (const WordTag *wt=w; wt; wt=wt->Next())
      wt->Print(socket);
  } else {
    socket << string << " is not in main lexicon, analyzing..." << "\n";
    NewWord *nw = newWords->AddWord(string);
    AnalyzeNewWord(nw, true);
    nw->Print();
    for (WordTag *wt=nw; wt; wt=wt->Next())
      if (wt->NLemmas() == 0) {
	wt->Print(socket);
	socket << "\n";
      }
    w = nw;
  }
  for (const WordTag *wt=w; wt; wt=wt->Next()) {
    socket << tab << "all forms that can be generated from " << wt << ':' << "\n";
    for (int k=0; k<wt->NLemmas(); k++)
      for (int j=0; j<wt->Lemma(k)->NInflectRules(); j++)
	for (int i=0; i<tags->Ct(); i++) {
	  const WordTag *wt2 = wt->GetForm(&(*tags)[i], k, j);
	  if (wt2) {
	    socket << wt2;
	    if (wt2->GetWord()->IsNewWord()) {
	      socket << " *";
	    }
	    socket << "\n";
	  }
	}
  }
}

#endif
