/* developers-tagger.hh
 * author: Johan Carlberger
 * last change: 991029
 * comments: DevelopersTagger class
 *           Loads lexicons fast if possible, otherwise loads slowly and stores
 *           the lexicons in fast format
 *           DevelopersTagger adds optimization and evaluation functionality to a Tagger
 *           A DevelopersTagger must be run before a Tagger can be run
 */

#ifndef _developerstagger_hh
#define _developerstagger_hh

#include "tagger.h"
#include <iostream>
#include <map>

class TesaurusWord {
public:
  TesaurusWord(const WordTag *wt = NULL) : wordTag(wt), wantedForm(NULL),
    use(NULL), freq(0), nDocs(0), docFreq(0), crap(-1) {}
  TesaurusWord *IndexWord() {
    if (!wantedForm)
      return this;
    return (wantedForm->use) ? wantedForm->use : wantedForm;
  }
  void Print(std::ostream&) const;
  static int totalNDocs;
  const WordTag *wordTag;
  TesaurusWord *wantedForm;
  TesaurusWord *use;
  int freq;
  int nDocs;
  int docFreq;
  int crap;
};

inline std::ostream& operator<<(std::ostream& os, const TesaurusWord &tw) {
  tw.Print(os);
  return os;
}

inline std::ostream& operator<<(std::ostream& os, const TesaurusWord *tw) {
  if (tw) os << *tw; else os << "(null TesaurusWord)";
  return os;
}

inline int CompareTesaurusWords(const TesaurusWord &t1, const TesaurusWord &t2) {
  return t1.wordTag != t2.wordTag;
}

inline uint KeyTesaurusWord(const TesaurusWord &t) {
  return Hash(t.wordTag->String());
}

class DevelopersTagger : public Tagger {
public:
  DevelopersTagger() : Tagger(), correctTags(0), correctTagsSize(0) {}
  ~DevelopersTagger() { delete [] correctTags;}
  void Optimize(char*);
  bool LexiconCheck();
  void PrintStatistics();
  
#ifdef PREDICTOR
  void GenerateExtraWordMeanings();
#endif
  void GenerateExtraWordTags(bool selectUnknownLemmas);
  void GuessRulesForWords();
  void SelectMoreWordForms(bool selectUnknownLemmas);
  void TestInflections();
  void ResetWords();
  void LoadTesaurus(const char *wtlFile, const char *tesFile);
  void ExtractTesaurusWordTags(const char *file);
  void ExtractIndexWords(char **files, int nFiles);

  // jonas: copy-pasted method from obsolete tagger code
  void ReadTaggedText();
  //jonas, copy pasted code from tagger.h 
  int EvaluateTagging();
  // jonas, new method for using correct tags
  Tag *GetCorrectTag(const WordToken *wt) const {
    if(correctTags)
      return correctTags[wt - theTokens];
    return 0;
  }
  void DontTagText(); // jonas, method to set selected tags to the correctTags
private:
  //jonas, this array keeps track of the correct tags for tokens,
  // the same index is used as in 'theTokens' (member vector of tokens read)
  // when Reset() is called should this be emptied ? 
  // I think not (it currently isn't emptied)
  Tag **correctTags;
  int correctTagsSize;

  int CountCorrectTaggings() const;
  void OptimizeParameter(char *name, bool *p);
  void OptimizeParameter(char *name, int *p); // jonas
  void OptimizeParameter(char *name, float* param, float min = -1, float max = -1);
  void OptimizeParameter(char *name, int* param, int min, int max);
  void OptimizeParameterGolden(float* param, float min = -1, float max = -1);
  void OptimizeParameterLinear(float* param, float min = -1, float max = -1);
  void PrintParameters() const;
  void SetProbsAndTagText();
  int TagAndCount(float *parameter, float x);
  int TagAndCount(int *parameter, int x);
  bool improvement;
  int bestResult;
  int nLinearOptSteps;
  int nTesaurusWords;
  HashArray<TesaurusWord> tesaurusWords;
};

#endif
