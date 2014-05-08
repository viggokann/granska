/* morflexicon.cc
 * author: Johan Carlberger
 * last change: 2000-05-08
 * comments: MorfLexicon class
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
#include "message.h"
#include "morflexicon.h"
#include "settings.h"

DefObj(MorfLexicon);

MorfLexicon::~MorfLexicon() {
  Message(MSG_STATUS, "deleting morf lexicon...");
  if (strings) { delete [] strings; ExtByt(-CL); } // jbfix: delete => delete []
  if (more) delete [] more;
  DelObj();
}

void MorfLexicon::LoadInfo() {
  Message(MSG_STATUS, "loading morf info...");
  char string[30];
  std::ifstream in;
  FixIfstream(in, lexiconDir, "info");
  in >> CW >> CL;
  in >> CWT >> string;
}

void MorfLexicon::AllocateMemory() {
  Message(MSG_STATUS, "allocating memory for morfs...");
  CMW = CWT - CW;
  Init("morfs", CW, CompareWords, KeyWord, RankWords, CompareStringAndWord, KeyWordString);
  strings = new char[CL]; // new OK
  ExtByt(CL);
  ensure(strings);
  more = new WordTag[CMW];  // why word, and not wordtag?
  ensure(more);
}

void MorfLexicon::LoadSlow(const char *dir, TagLexicon& tags) {
  lexiconDir = dir;
  LoadInfo();
  AllocateMemory();
  Message(MSG_STATUS, "loading morf words...");
  char *buff = strings;
  std::ifstream in;
  FixIfstream(in, lexiconDir, "cw");
  int i;
  for (i=0; i<CW; i++) {
    Word &w = (*this)[i];
    w.Init();
    in >> w.freq;
    if (in.peek() == '\t') in.get(); 
    in.getline(buff, MAX_WORD_LENGTH);
    w.string = buff;
    uint len = strlen(buff);
    w.strLen = (uchar) len;
    buff += len+1;
  }
  if (buff != strings + CL)
    Message(MSG_WARNING, int2str(buff-strings), "characters read from morfs/cw");
  Hashify();  
  CompressStrings();
  FixIfstream(in, lexiconDir, "cwt");
  char string[MAX_WORD_LENGTH];
  Tag tag;
  int freq;
  int j = 0;
  for (i=0; i<CWT; i++) {
    in >> freq;
    if (in.peek() == '\t') in.get(); 
    in.getline(string, MAX_WORD_LENGTH, '\t');
    in >> tag.string;
    Word *w = Find(string);
    if (!w)
      Message(MSG_ERROR, "unknown word in morfs/cwt:", string);
    const Tag *t = tags.Find(tag);
    if (!t)
      Message(MSG_ERROR, "unknown tag in morfs/cwt:", tag.string);
    if (w->TagIndex() != TAG_INDEX_NONE) {
      WordTag *q;
      for (q = w; q->Next(); q=q->Next());
      q->next = &more[j];
      more[j].Init(w, false);
      more[j].tagIndex = t->Index();
      more[j].tagFreq = freq;
      j++;
      if (j > CMW)
	Message(MSG_ERROR, "too many word-tags in morfs/cwt at line:", int2str(i+1));
    } else {
      w->tagIndex = t->Index();
      w->tagFreq = freq;
    }
  }
  ensure(j == CMW);
  for (i=0; i<CW; i++) {
    int f = 0;
    for (WordTag *q = &(*this)[i]; q; q=q->Next())
      f += q->TagFreq();
    if (f != (*this)[i].freq) {
      Message(MSG_WARNING, "sum of word-tag-freqs for word", (*this)[i].String(),
	      "does not sum up to word-freq");
      Message(MSG_CONTINUE, "sum:", int2str(f), "word-freq:", int2str((*this)[i].freq));
    }
  }
}

void MorfLexicon::SetPointersFromIndices() {
  Message(MSG_STATUS, "setting morf pointers...");
  for (int i=0; i<CW; i++) {
    Word *w = &(*this)[i];
    w->string = strings + (size_t)w->string;
    WordTag *wt;
    for (wt = w; wt; wt=wt->Next())
      if (wt->next == (WordTag*) -1)
	wt->next = w;
      else
	wt->next = &more[(size_t)wt->next];
  }
}

bool MorfLexicon::LoadFast(const char *dir, bool warn) {
  lexiconDir = dir;
  std::ifstream in;
  if (!FixIfstream(in, lexiconDir, "fast", warn))
    return false;
  if (!CheckVersion(in, xVersion)) {
    Message(MSG_WARNING, "fast morf lexicon obsolete");
    return false;
  }
  Message(MSG_STATUS, "loading morf lexicon fast...");
  ReadVar(in, CW);
  ReadVar(in, CWT);
  ReadVar(in, CL);
  AllocateMemory();
  Load(in);
  ReadData(in, (char*)more, sizeof(WordTag)*CMW, "more");
  ReadData(in, strings, CL, "strings");
  SetPointersFromIndices();
  return true;
}

bool MorfLexicon::Save() {
  std::ifstream in;
  if (FixIfstream(in, lexiconDir, "fast", false))
    return false;
  Message(MSG_STATUS, "saving fast morf lexicon...");
  in.close();
  for (int i=0; i<CW; i++) {
    WordTag *wt, *next;
    (*this)[i].string = (*this)[i].string - (size_t)strings;
    for (wt=&(*this)[i]; wt->Next(); wt=next) {
      next = wt->next;
      wt->next = (WordTag*) (wt->next - &more[0]); // i.e. index of wt->next in more
    }
    wt->next = (WordTag*) -1;
  }
  std::ofstream out;
  FixOfstream(out, lexiconDir, "fast");
  SetVersion(out, xVersion);
  WriteVar(out, CW);
  WriteVar(out, CWT);
  WriteVar(out, CL);
  Store(out);
  WriteData(out, (char*)more, sizeof(WordTag)*CMW, "more");
  WriteData(out, strings, CL, "strings");
  SetPointersFromIndices();
  return true;
}

void MorfLexicon::CompressStrings() {
  Message(MSG_STATUS, "compressing morf strings...");
  // assuming suggested-bit is 0 for all words
  // mark words that are are subwords:
  int i;
  for (i=0; i<CW; i++)
    for (const char *s = (*this)[i].string+1; *s; s++) {
      Word *w;
      if ((w = Find(s)) != NULL)
	w->suggested = 1;
    }
  char *s, *strings2;
  s = strings2 = new char[CL];      // new OK // a new place to put the strings
  ensure(s);
  // move all words that are not subwords:
  for (i=0; i<CW; i++) {
    if (!(*this)[i].suggested) {
      strcpy(s, (*this)[i].string);
      (*this)[i].string = s;
      s += (*this)[i].strLen + 1;
    }
  }
  // move all subwords:
  for (i=0; i<CW; i++)
    if (!(*this)[i].suggested)
      for (const char *ss = (*this)[i].string+1; *ss; ss++) {
	Word *w;
	if ((w = Find(ss)) != NULL)
	  if (w->suggested) {
	    w->string = (char*) ss;
	    w->suggested = 0;
	  }
      }
  //  for (i=0; i<CW; i++)
  //    ensure(!words[i].suggested);
  //  std::cout<<" (saved "<<CL-(s-strings2)<<" of "<<CL<<" bytes, "<<100.0*(CL-(s-strings2))/CL<<"%)";
  ExtByt(-CL);
  CL = s-strings2+1;
  ExtByt(CL);
  delete strings;
  strings = strings2;
}

/* jonas */
void MorfLexicon::PrintStatistics() const {
  std::cout<<"morf lexicon statistics: "<<std::endl;
  Statistics();
}

