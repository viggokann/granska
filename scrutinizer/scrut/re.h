#include <vector>
#include <string>

// bool isValidRegex(const char *regex)
// returns true if the string in 'regex' is a valid regular
// expression, false if not
bool isValidRegex(const char *regex);

// bool regexMatch(const char *leftVal, const char *rightVal)
// returns true if the regular expression in 'rightVal' matches the
// contents of the string in 'leftVal', false if not.
bool regexMatch(const char *leftVal, const char *rightVal);

// std::vector<std::string> regexGroups(std::string text, const char *re)
// Matches the regular expression in 're' to the text in 'text'. If
// the contents of 're' start and end with '/', the slashes are
// removed first. The result is a vector with the strings matching the
// subgroups in the regular expression. So with '3kg' as the text,
// '/([0-9])((kg)|(km))/' as the regular expression, the result would be
// a vector with contents ['3kg', '3', 'kg', 'kg', '']
std::vector<std::string> regexGroups(std::string text, const char *re);

// const char *GetLastRegex();
