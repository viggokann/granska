/* token.hh
 * author: Johan Carlberger
 * last change: 2000-05-30
 * comments: Tokens
 */

#ifndef _token_hh
#define _token_hh

#include <string.h>
#include <iostream>

enum Token {                 // examples:
  TOKEN_END = 0,
  TOKEN_WORD,                // IT-användning 
  TOKEN_SIMPLE_WORD,         // hej
  TOKEN_SPLIT_WORD,          // hälso- och sjukvård, a-, b- och c-uppgiften
  TOKEN_ABBREVIATION,        // t.ex., p
  TOKEN_CARDINAL,            // 675 000, -1,5
  TOKEN_BAD_CARDINAL,        // 6750 000, 45.4
  TOKEN_CARDINAL_SIN,        // 1, 1,0000
  TOKEN_ORDINAL,             // 6:e
  TOKEN_PERCENTAGE,          // 22 %
  TOKEN_MATH,                // 1+1 = 3
  TOKEN_YEAR,                // 1927, år 2000
  TOKEN_DATE,                // den 4 november 1927, 4 dec. 
  TOKEN_TIME,                // klockan 9, kl. 14.37
  TOKEN_PARAGRAPH,           // $ 7, 7 $, $7, $$ 7-9, $
  TOKEN_PUNCTUATION,         // ,
  TOKEN_PERIOD,              // . 
  TOKEN_QUESTION_MARK,       // ?
  TOKEN_EXCLAMATION_MARK,    // !
  TOKEN_LEFT_PAR,            // (
  TOKEN_RIGHT_PAR,           // )
  TOKEN_CITATION,            // "\""
  TOKEN_E_MAIL,              // jfc@nada.kth.se 
  TOKEN_URL,                 // http://www.nada.kth.se/~jfc/
  TOKEN_SPACE,               // \t 
  TOKEN_NEWLINE,             // \n
  TOKEN_BEGIN_TITLE,
  TOKEN_END_TITLE,
  TOKEN_BEGIN_HEADING,     
  TOKEN_END_HEADING,
  TOKEN_BEGIN_PARAGRAPH,
  TOKEN_BEGIN_TABLE,
  TOKEN_TABLE_TAB,
  TOKEN_END_TABLE,
  TOKEN_PROPER_NOUN,
  TOKEN_PROPER_NOUN_GENITIVE,
  TOKEN_DELIMITER_PERIOD,
  TOKEN_DELIMITER_QUESTION,
  TOKEN_DELIMITER_EXCLAMATION,
  TOKEN_DELIMITER_HEADING,
  TOKEN_DELIMITER_OTHER,
  TOKEN_SILLY,     
  TOKEN_ERROR,
  TOKEN_UNKNOWN
};

const int N_TOKENS = TOKEN_UNKNOWN + 1;
const char *Token2String(Token t);
Token String2Token(const char *s);

inline const char *Token2String(Token t) {
  switch (t) {
  case TOKEN_END : return "TOKEN_END";
  case TOKEN_WORD : return "TOKEN_WORD";
  case TOKEN_SIMPLE_WORD : return "TOKEN_SIMPLE_WORD";
  case TOKEN_SPLIT_WORD : return "TOKEN_SPLIT_WORD";
  case TOKEN_ABBREVIATION : return "TOKEN_ABBREVIATION";
  case TOKEN_LEFT_PAR : return "TOKEN_LEFT_PAR";
  case TOKEN_RIGHT_PAR : return "TOKEN_RIGHT_PAR";
  case TOKEN_CITATION : return "TOKEN_CITATION";
  case TOKEN_YEAR : return "TOKEN_YEAR";
  case TOKEN_DATE : return "TOKEN_DATE";
  case TOKEN_TIME : return "TOKEN_TIME";
  case TOKEN_PARAGRAPH : return "TOKEN_PARAGRAPH";
  case TOKEN_ORDINAL : return "TOKEN_ORDINAL";
  case TOKEN_CARDINAL : return "TOKEN_CARDINAL";
  case TOKEN_BAD_CARDINAL : return "TOKEN_BAD_CARDINAL";
  case TOKEN_CARDINAL_SIN : return "TOKEN_CARDINAL_SIN";
  case TOKEN_PERCENTAGE : return "TOKEN_PERCENTAGE";
  case TOKEN_MATH : return "TOKEN_MATH";
  case TOKEN_PERIOD : return "TOKEN_PERIOD";
  case TOKEN_QUESTION_MARK : return "TOKEN_QUESTION_MARK";
  case TOKEN_EXCLAMATION_MARK : return "TOKEN_EXCLAMATION_MARK";
  case TOKEN_BEGIN_TITLE : return "TOKEN_BEGIN_TITLE";
  case TOKEN_END_TITLE : return "TOKEN_END_TITLE";
  case TOKEN_BEGIN_HEADING : return "TOKEN_BEGIN_HEADING";
  case TOKEN_END_HEADING : return "TOKEN_END_HEADING";
  case TOKEN_BEGIN_PARAGRAPH : return "TOKEN_BEGIN_PARAGRAPH";
  case TOKEN_BEGIN_TABLE : return "TOKEN_BEGIN_TABLE";
  case TOKEN_TABLE_TAB : return "TOKEN_TABLE_TAB";
  case TOKEN_END_TABLE : return "TOKEN_END_TABLE";
  case TOKEN_PROPER_NOUN: return "TOKEN_PROPER_NOUN";
  case TOKEN_PROPER_NOUN_GENITIVE: return "TOKEN_PROPER_NOUN_GENITIVE";
  case TOKEN_DELIMITER_PERIOD: return "TOKEN_DELIMITER_PERIOD";
  case TOKEN_DELIMITER_QUESTION: return "TOKEN_DELIMITER_QUESTION";
  case TOKEN_DELIMITER_EXCLAMATION: return "TOKEN_DELIMITER_EXCLAMATION";
  case TOKEN_DELIMITER_HEADING: return "TOKEN_DELIMITER_HEADING";
  case TOKEN_DELIMITER_OTHER: return "TOKEN_DELIMITER_OTHER";
  case TOKEN_SILLY : return "TOKEN_SILLY";
  case TOKEN_PUNCTUATION : return "TOKEN_PUNCTUATION";
  case TOKEN_E_MAIL : return "TOKEN_E_MAIL";
  case TOKEN_URL : return "TOKEN_URL";
  case TOKEN_NEWLINE : return "TOKEN_NEWLINE";
  case TOKEN_SPACE : return "TOKEN_SPACE";
  case TOKEN_ERROR : return "TOKEN_ERROR";
  case TOKEN_UNKNOWN: return "TOKEN_UNKNOWN";
  }
  return "TOKEN_UNKNOWN";
}

inline Token String2Token(const char *s) {
  if (!strcmp(s, "TOKEN_BEGIN_TITLE"))
    return TOKEN_BEGIN_TITLE;
  if (!strcmp(s, "TOKEN_END_TITLE"))
    return TOKEN_END_TITLE;
  if (!strcmp(s, "TOKEN_BEGIN_HEADING"))
    return TOKEN_BEGIN_HEADING;
  if (!strcmp(s, "TOKEN_END_HEADING"))
    return TOKEN_END_HEADING;
  if (!strcmp(s, "TOKEN_BEGIN_PARAGRAPH"))
    return TOKEN_BEGIN_PARAGRAPH;
  if (!strcmp(s, "TOKEN_BEGIN_TABLE"))
    return TOKEN_BEGIN_TABLE;
  if (!strcmp(s, "TOKEN_TABLE_TAB"))
    return TOKEN_TABLE_TAB;
  if (!strcmp(s, "TOKEN_END_TABLE"))
    return TOKEN_END_TABLE;
  if (!strcmp(s, "TOKEN_ERROR"))
    return TOKEN_ERROR;
  if (!strcmp(s, "TOKEN_END"))
    return TOKEN_END;
  if (!strcmp(s, "TOKEN_WORD"))
    return TOKEN_WORD;
  if (!strcmp(s, "TOKEN_LEFT_PAR"))
    return TOKEN_LEFT_PAR;
  if (!strcmp(s, "TOKEN_RIGHT_PAR"))
    return TOKEN_RIGHT_PAR;
  if (!strcmp(s, "TOKEN_CITATION"))
    return TOKEN_CITATION;
  if (!strcmp(s, "TOKEN_SIMPLE_WORD"))
    return TOKEN_SIMPLE_WORD;
  if (!strcmp(s, "TOKEN_SPLIT_WORD"))
    return TOKEN_SPLIT_WORD;
  if (!strcmp(s, "TOKEN_ABBREVIATION"))
    return TOKEN_ABBREVIATION;
  if (!strcmp(s, "TOKEN_YEAR"))
    return TOKEN_YEAR;
  if (!strcmp(s, "TOKEN_DATE"))
    return TOKEN_DATE;
  if (!strcmp(s, "TOKEN_TIME"))
    return TOKEN_TIME;
  if (!strcmp(s, "TOKEN_PARAGRAPH"))
    return TOKEN_PARAGRAPH;
  if (!strcmp(s, "TOKEN_ORDINAL"))
    return TOKEN_ORDINAL;
  if (!strcmp(s, "TOKEN_CARDINAL"))
    return TOKEN_CARDINAL;
  if (!strcmp(s, "TOKEN_BAD_CARDINAL"))
    return TOKEN_BAD_CARDINAL;
  if (!strcmp(s, "TOKEN_CARDINAL_SIN"))
    return TOKEN_CARDINAL_SIN;
  if (!strcmp(s, "TOKEN_PERCENTAGE"))
    return TOKEN_PERCENTAGE;
  if (!strcmp(s, "TOKEN_MATH"))
    return TOKEN_MATH;
  if (!strcmp(s, "TOKEN_PUNCTUATION"))
    return TOKEN_PUNCTUATION;
  if (!strcmp(s, "TOKEN_PERIOD"))
    return TOKEN_PERIOD;
  if (!strcmp(s, "TOKEN_QUESTION_MARK"))
    return TOKEN_QUESTION_MARK;
  if (!strcmp(s, "TOKEN_EXCLAMATION_MARK"))
    return TOKEN_EXCLAMATION_MARK;
  if (!strcmp(s, "TOKEN_PROPER_NOUN"))
    return TOKEN_PROPER_NOUN;
  if (!strcmp(s, "TOKEN_PROPER_NOUN_GENITIVE"))
    return TOKEN_PROPER_NOUN_GENITIVE;
  if (!strcmp(s, "TOKEN_E_MAIL"))
    return TOKEN_E_MAIL;
  if (!strcmp(s, "TOKEN_URL"))
    return TOKEN_URL;
  if (!strcmp(s, "TOKEN_NEWLINE"))
    return TOKEN_NEWLINE;
  if (!strcmp(s, "TOKEN_SPACE"))
    return TOKEN_SPACE;
  if (!strcmp(s, "TOKEN_SILLY"))
    return TOKEN_SILLY;
  if (!strcmp(s, "TOKEN_DELIMITER_PERIOD"))
    return TOKEN_DELIMITER_PERIOD;
  if (!strcmp(s, "TOKEN_DELIMITER_QUESTION"))
    return TOKEN_DELIMITER_QUESTION;
  if (!strcmp(s, "TOKEN_DELIMITER_EXCLAMATION"))
    return TOKEN_DELIMITER_EXCLAMATION;
  if (!strcmp(s, "TOKEN_DELIMITER_HEADING"))
    return TOKEN_DELIMITER_HEADING;
  if (!strcmp(s, "TOKEN_DELIMITER_OTHER"))
    return TOKEN_DELIMITER_OTHER;
  if (!strcmp(s, "TOKEN_ERROR"))
    return TOKEN_ERROR;
  if (!strcmp(s, "TOKEN_UNKNOWN"))
    return TOKEN_UNKNOWN;
  return TOKEN_ERROR;
}

#ifndef _NOT_CPP
inline std::ostream &operator<<(std::ostream& out, Token t) {
  return out << Token2String(t);
}
#endif

#endif

