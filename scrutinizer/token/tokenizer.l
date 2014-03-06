/* tokenizer.l
 * last change: 2000-02-02
 * input: a normal text file
 * output: tokenized text
 */

%{
  #include "token.h"
%}

name tokenizerFlexLexer


DIGIT         [0-9]
DIGIT3        {DIGIT}{DIGIT}{DIGIT}
YE            (1[3-9]{DIGIT}{DIGIT})|(2[01]{DIGIT}{DIGIT})
YEA           {YE}|{YE}{DWS}{YE}|{YE}(\/|{DWS}|\/{DWS}){DIGIT}{DIGIT}
YEAR          ({YEA}|{YE})(\-talet)?|år{SPACE_NL}*({YEA}|{YE})
TIME1         ((klockan|kl\.|kl){SPACE_NL}*)?([0-1]?{DIGIT}|2[0-4])\.[0-6]{DIGIT}
TIME2         (klockan|kl\.|kl){SPACE_NL}*([0-1]?{DIGIT}|2[0-4])({PUNC}?{SPACE}*[0-6]{DIGIT})?
DIGITS        {DIGIT}+
DWS           {SPACE_NL}*\-{SPACE_NL}*
PARAGRAPH     §+{SPACE}*{DIGITS}({DWS}{DIGITS})?|{DIGITS}({DWS}{DIGITS})?{SPACE}*§+|§+
CARDINALSIN   1(,0+)?
CARDINAL_OK   -?(0|[1-9]{DIGIT}?{DIGIT}?({SPACE}{DIGIT3})*)(,{DIGITS})?
CARDINAL      -?{DIGITS}(({SPACE}+|{PUNC}|:{SPACE}+){DIGITS})*
PERCENTAGE    {CARDINAL}({DWS}{CARDINAL})?{SPACE_NL}*(\%(\-ig)?|procent|promille)
MATH          {CARDINAL}({SPACE}*[+/*=-]{SPACE}*{CARDINAL})+
ORDINAL       {CARDINAL}:?[eE]
LETTER        [a-zA-Zà-ÿÀ-ÿ]
LD            {LETTER}|{DIGIT}
ENDER         [\.!?]
DASH          \-\n?
LWORD         {LETTER}+|({LD}*{LETTER}{LD}*)
DWORD         {LWORD}({DASH}{LWORD})+
WORD          {LWORD}|{DWORD}
WORD_DOTS     {WORD}(\.{WORD})+
SPACE         [\ \t\r\225\240]
SPACE_NL      \n|{SPACE}
AMP           [&]
LT            [<]
GT            [>]
APOS          [']
QOUT          ["]
PUNC          [\/\.\-,:+=;\(\)\[\]_%;@~\\´\*$§#£\{\}`\^\|·»¾º]
SOME_PUNC     [\/\-+=;\(\)\[\]_%<>@~\\´\*$§#£\{\}`\^\|·»¾º]
EMAIL         ({WORD}|{WORD_DOTS})@{WORD_DOTS}
PROTOCOL      ({WORD}:?\/\/)|www\.
URL           {PROTOCOL}{WORD}([\.\/~]|{WORD})+{WORD}\/?
DOT           ((\.|:){SPACE}*)|{SPACE}+
CON           [b-df-hj-np-tv-xzB-DF-HJ-NP-TV-XZ]
C             {CON}+|[eouyäEOUYÄ]
CV            {CON}|{LETTER}
NL            [^a-zA-ZÀ-ÿà-ÿ]
MONTH         maj|((jan|feb|mar|apr|jun|jul|aug|agu|sep|okt|nov|dec)\.?{LETTER}*)

%%

"Content-Type: text/plain"   ;

TOKEN_.*                     {/* shorten yytext here */ return String2Token((const char *)yytext); }

{TIME1}                      return TOKEN_TIME;
{TIME2}                      return TOKEN_TIME;
{PARAGRAPH}                  return TOKEN_PARAGRAPH;

{C}({DOT}{CV})*{DOT}{C}\.?/{NL}   return TOKEN_ABBREVIATION;

({LWORD}\.{SPACE}*)*dr\.?/{NL}              return TOKEN_ABBREVIATION;
[eo]{DOT}?dyl\.?/{NL}       return TOKEN_ABBREVIATION;
f{DOT}?(ö|Ö)\.?/{NL}     return TOKEN_ABBREVIATION;
i{DOT}?st{DOT}?f\.?/{NL} return TOKEN_ABBREVIATION;
kungl{DOT}maj:t/{NL}     return TOKEN_ABBREVIATION;
({LWORD}\.{SPACE}*)*lic\.?/{NL}              return TOKEN_ABBREVIATION;
t{DOT}ex\.?/{NL}         return TOKEN_ABBREVIATION;

{YEAR}                      return TOKEN_YEAR;
(den{SPACE_NL}*)?({DIGIT}{DIGIT}?|{ORDINAL}){SPACE_NL}*{MONTH}({SPACE_NL}*{YEAR})? return TOKEN_DATE;

{WORD}\-({SPACE_NL}*,{SPACE_NL}*{WORD}\-)*{SPACE_NL}+(och(\/eller)?|eller|samt|som|till|resp|resp\.|respektive){SPACE_NL}+{WORD}   return TOKEN_SPLIT_WORD;
{EMAIL}                     return TOKEN_E_MAIL;
{URL}                       return TOKEN_URL;
\<HREF[^>]+\>             return TOKEN_URL;

{PERCENTAGE}                return TOKEN_PERCENTAGE;
{LWORD}                     return TOKEN_SIMPLE_WORD;
\"{WORD}\"-?{WORD}          return TOKEN_WORD;
\({WORD}\)-?{WORD}          return TOKEN_WORD;
{WORD}-?\({WORD}\)          return TOKEN_WORD;
{WORD}([:']{C})?            return TOKEN_WORD;
{CARDINAL}-{WORD}           return TOKEN_WORD;

{MATH}                      return TOKEN_MATH;
{CARDINALSIN}               return TOKEN_CARDINAL_SIN;
{CARDINAL_OK}               return TOKEN_CARDINAL;
{CARDINAL}                  return TOKEN_BAD_CARDINAL;
{ORDINAL}                   return TOKEN_ORDINAL;

{SPACE}*\n{SPACE}*(\n{SPACE}*)+  return TOKEN_BEGIN_PARAGRAPH;
{SPACE}*(\n{SPACE}*)+       return TOKEN_NEWLINE;
{SPACE}+                    return TOKEN_SPACE;
\.{ENDER}*                  return TOKEN_PERIOD;
\?{ENDER}*                  return TOKEN_QUESTION_MARK;
\!{ENDER}*                  return TOKEN_EXCLAMATION_MARK;
\(                          return TOKEN_LEFT_PAR;
\)                          return TOKEN_RIGHT_PAR;
\"                          return TOKEN_CITATION;  
{SOME_PUNC}+                return TOKEN_PUNCTUATION;
{PUNC}                      return TOKEN_PUNCTUATION;
.                           return TOKEN_PUNCTUATION;
AMP                         return TOKEN_PUNCTUATION;
LT                          return TOKEN_PUNCTUATION;
GT                          return TOKEN_PUNCTUATION;
APOS                        return TOKEN_PUNCTUATION;
QOUT                        return TOKEN_PUNCTUATION;          
%%

int tokenizerwrap() { return 1; }
