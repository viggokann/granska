#include <regex>
#include <iostream>

// static const char *lastRegex = 0;

// const char *GetLastRegex() {
//   return lastRegex;
// }

// bool isValidRegex(const char *regex)
// returns true if the string in 'regex' is a valid regular
// expression, false if not
bool isValidRegex(const char *regex) {
    try {
      std::regex ex(regex);
    }
    catch (const std::regex_error& ) {
      // std::cout << "isValidRegex( " << regex << " ) -> False" << std::endl;
      return false;
    }
    // std::cout << "isValidRegex( " << regex << " ) -> True" << std::endl;
    return true;
}

// bool regexMatch(const char *leftVal, const char *rightVal)
// returns true if the regular expression in 'rightVal' matches the
// contents of the string in 'leftVal', false if not.
bool regexMatch(const char *leftVal, const char *rightVal) {
  std::string text(leftVal);
  std::string rTemp(rightVal);
  if(rTemp[0] == '/' && rTemp[rTemp.size() - 1] == '/') {
    rTemp = (rightVal + 1);
    rTemp.pop_back();
  }
  std::regex ex(rTemp);
  std::smatch m;

  // lastRegex = rightVal;
  
  // if(std::regex_search(text, m, ex)) {
  if(std::regex_match(text, m, ex)) {
    // should we use match (the whole token should match the regex)
    // or use search (enough if a substring of the token matches the regex)
    return true;
  }
  return false;
}

// std::vector<std::string> regexGroups(std::string text, const char *re)
// Matches the regular expression in 're' to the text in 'text'. If
// the contents of 're' start and end with '/', the slashes are
// removed first. The result is a vector with the strings matching the
// subgroups in the regular expression. So with '3kg' as the text,
// '/([0-9])((kg)|(km))/' as the regular expression, the result would be
// a vector with contents ['3kg', '3', 'kg', 'kg', '']
std::vector<std::string> regexGroups(std::string text, const char *re) {
  std::string rTemp(re);
  if(rTemp[0] == '/' && rTemp[rTemp.size() - 1] == '/') {
    rTemp = (re + 1);
    rTemp.pop_back();
  }
  std::regex ex(rTemp);
  std::smatch m;

  std::vector<std::string> res;
  
  if(std::regex_match(text, m, ex)) {
    for(unsigned int i = 0; i < m.size(); i++) {
      res.push_back(m[i]);
    }
    return res;
  }

  if(std::regex_search(text, m, ex)) {
    for(unsigned int i = 0; i < m.size(); i++) {
      res.push_back(m[i]);
    }
    return res;
  }
  
  return res;
}
