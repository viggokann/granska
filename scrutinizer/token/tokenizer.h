/* tokenizer.hh
 * author: Johan Carlberger
 * last change: 2000-04-18
 * comments: Tokenizer class:
 */

#ifndef _tokenizer_hh
#define _tokenizer_hh

#include <fstream>
#include <string.h>
#include <ctype.h>

#undef yyFlexLexer
#define yyFlexLexer tokenizerFlexLexer
#include "FlexLexer.h"

#include "basics.h"
#include "message.h"
#include "token.h"
#include <sstream>
//inline void tokenizerFlexLexer::LexerError(const char *msg) {
//  std::cout << msg;
//}

class Tokenizer : public tokenizerFlexLexer {
public:
    Tokenizer();
  void SetStream(std::istream *instream) { switch_streams(instream, NULL); } 
  Token Parse() { return (Token) yylex(); }
  int TokenLength() { return YYLeng(); }
  const char* TokenString() { return YYText(); }
  void ParseText();
  int CheckTokens() const;
};

inline Tokenizer::Tokenizer() {
  //  if (xWarnAll && !CheckTokens())
  //    Message(MSG_ERROR, "tokens don't work properly");
}

inline void Tokenizer::ParseText() {
  Token token;
  while ((token = Parse()) != TOKEN_END) {
    if (token != TOKEN_SPACE) {
        if (token != TOKEN_NEWLINE && token != TOKEN_BEGIN_PARAGRAPH) {
            std::cout << TokenString();
        }
        std::cout << tab << token << std::endl;
    }
  }
}

inline int Tokenizer::CheckTokens() const {
  for (int i=0; i<N_TOKENS; i++) {
    Token t = (Token)i;
    if (String2Token(Token2String(t)) != t) {
      std::cout << "CheckTokens(): Token[" << i << "] = " << t << "doesn't work" << std::endl;
      return 0;
    }
  }
  return 1;
}

#endif




