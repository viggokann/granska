class RegexpMatching {
  int RuleNumber() const;
  int StartPos() const;
  int Length() const;
  char *Text() const;

  // start is relative the matching, false is returned if i is out of bounds
  bool SubexpressionPos(int i, int &start, int &length);
 private:
  void InitSubexpression();

}

class RegexpRules {
  static int nRules;
  static const char *regexps[];
}

class Regexps {
  // false is returned on error
  bool PerformMatching(FILE *fp);
  int NumberOfMatchings();
  RegexpMatching *FirstMatching();
  RegexpMatching *NextMatching(RegexpMatching *r);
}  
