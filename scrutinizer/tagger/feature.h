/* feature.hh
 * author: Johan Carlberger
 * last change: 990326
 * comments: Feature class
 */

#ifndef _feature_hh
#define _feature_hh

#include <iostream>
#include "ensure.h"

// these should be in Feature:
static const int MAX_CLASSES = 21;
static const int MAX_VALUES = 100;
static const int MAX_NAME = 8;
static const int MAX_DESCRIPTION = 45;
static const int MAX_FEATURES_PER_CLASS = 25;
static const int UNDEF = 0;

class Feature { // abstract class
  friend class TagLexicon;
public:
  const char* Name() const { return name; }
  const char* Description() const { return description; }
  int Index() const { return index; }
private:
  char name[MAX_NAME];
  char description[MAX_DESCRIPTION];
  int index;
};

class FeatureValue : public Feature {
  friend class TagLexicon;
public:
  int GetFeatureClass() const { return featureClass; } 
private:
  int featureClass;
};

class FeatureClass : public Feature {
  friend class TagLexicon;
public:
  FeatureClass() { nFeatures = 0; }
  int NFeatures() const { return nFeatures; }
  int GetFeature(int n) const {
    ensure(n >= 0 && n < nFeatures);
    return features[n];
  }
  bool HasFeature(int fv) const {
    for (int i=0; i<nFeatures; i++)
      if (features[i] == fv)
	return true;
    return false;
  }
private:
  int features[MAX_FEATURES_PER_CLASS];
  int nFeatures;
};

inline std::ostream& operator<<(std::ostream& os, const Feature &f) {
  os << f.Name();
  return os;
}

inline std::ostream& operator<<(std::ostream& os, const Feature *f) {
  if (f)
    os << *f;
  else
    os << "(null)";
  return os;
}

#endif
