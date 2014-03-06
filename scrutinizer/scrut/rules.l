/* rules.l
 * authors: Viggo Kann and Johan Carlberger
 * last Johan change: 2000-02-23
 * last Viggo change: 2000-09-15
 * comments: Lexikala regler för granskas regelspråk
 */

%{

#define YY_NEVER_INTERACTIVE 1
#include "stringbuf.h"

static StringBuf lexStringBuf;

%}
D         [0-9]
%x CODE STRING STRING2 COMMENT

%%

<INITIAL>^.*           { unsigned char *s,
                                       *t = (unsigned char *) linebuf + (yyleng / 4) * 4;
                         strcpy(linebuf, yytext); /* spara hela raden */
                         for (s = (unsigned char *) linebuf; s < t; s += 4)
				hashcode ^= (s[0] << 24) | (s[1] << 16) | 
					    (s[2] << 8)  | (s[3]);
                         switch (s - t) {
                           case 1: hashcode ^= (t[0] << 24);
                                   break;
                           case 2: hashcode ^= (t[0] << 24) | (t[1] << 16);
                                   break;
                           case 3: hashcode ^= (t[0] << 24) | (t[1] << 16) | 
  					       (t[2] << 8);
                                   break;
                         }
                         line++;
                         yyless(0); /* starta om läsningen */
                         BEGIN CODE;
                       }
<INITIAL>\n            line++;
<COMMENT>.	       ;
<COMMENT>"*)".*	       { strcpy(linebuf, yytext + 2); yyless(2); BEGIN CODE; }
<COMMENT>\n	       line++;

<STRING>\n             { yyerror("");
                         fprintf(stderr, "Radbyte inuti sträng.\n");
                       }
<STRING>\"             { char *t = lexStringBuf.NewString(yyleng);  /* jbfix: introduced stringbuf */
			 char *u = t;
                         int i;
			 for(i=0; i<yyleng-1; i++) {
			   if (yytext[i] == '\\' &&
                               (yytext[i+1] == '\\' || yytext[i+1] == '"'))
                                  i++;
                           *u++ = yytext[i];
                         }
                         *u = '\0'; /*
		         strncpy(t, yytext, yyleng - 1);
		         t[yyleng - 1] = '\0'; */
		         yylval.string = t;
		         BEGIN CODE;
		         return STRINGSYM;
		       }
<STRING>\\\"           { yymore(); }
<STRING>.              yymore();

<CODE>{D}+             { sscanf(yytext, "%d", &yylval.integer);
                         return INTEGERSYM;
                       }
<CODE>{D}+"."{D}+      { sscanf(yytext, "%lf", &yylval.real);
                         return REALSYM;
                       }
<CODE>"(*"                  BEGIN COMMENT;
<CODE>\"                    BEGIN STRING;
<CODE>[Ii][Ff]	            return IFSYM;
<CODE>[Tt][Hh][Ee][Nn]      return THENSYM;
<CODE>[Ee][Ll][Ss][Ee]      return ELSESYM;
<CODE>[Ee][Nn][Dd]          return ENDSYM;
<CODE>[Cc][Oo][Nn][Ss][Tt]  return CONSTSYM;
<CODE>[Gg][Oo][Tt][Oo]      return GOTOSYM; /* not used anymore */
<CODE>[Mm][Aa][Rr][Kk]      return MARKSYM;
<CODE>[Cc][Oo][Rr][Rr]      return CORRSYM;
<CODE>[Jj][Uu][Mm][Pp]      return JUMPSYM;
<CODE>[Ii][Nn][Ff][Oo]      return INFOSYM;
<CODE>[Cc][Aa][Tt][Ee][Gg][Oo][Rr][Yy]                 return CATEGORYSYM;
<CODE>[Aa][Cc][Tt][Ii][Oo][Nn]                         return ACTIONSYM;
<CODE>[Ss][Cc][Rr][Uu][Tt][Ii][Nn][Ii][Zz][Ii][Nn][Gg] return CHECKIDENTSYM;
<CODE>[Tt][Aa][Gg][Gg][Ii][Nn][Gg]		       return TAGIDENTSYM;
<CODE>[Ss][Ee][Aa][Rr][Cc][Hh][Ii][Nn][Gg]	       return LOOKUPIDENTSYM;
<CODE>[Ee][Dd][Ii][Tt][Ii][Nn][Gg]		       return EDITIDENTSYM;
<CODE>[Hh][Ee][Ll][Pp]				       return HELPIDENTSYM;
<CODE>[Aa][Cc][Cc][Ee][Pp][Tt][Ii][Nn][Gg]	       return ACCEPTIDENTSYM;
<CODE>[Aa][Cc][Cc][Ee][Pp][Tt]                         return ACCEPTSYM;
<CODE>[Dd][Ee][Tt][Ee][Cc][Tt]                         return DETECTSYM;
<CODE>[Ll][Ii][Nn][Kk]                                 return LINKSYM;
<CODE>"-"*[Ee][Nn][Dd][Ll][Ee][Ff][Tt][Cc][Oo][Nn][Tt][Ee][Xx][Tt]"-"* return ENDLEFTCONTEXTSYM;
<CODE>"-"*[Bb][Ee][Gg][Ii][Nn][Rr][Ii][Gg][Hh][Tt][Cc][Oo][Nn][Tt][Ee][Xx][Tt]"-"* return BEGINRIGHTCONTEXTSYM;
<CODE>[A-ZÅÄÖa-zåäö_][A-ZÅÄÖa-zåäö_0-9]*      { 
                   yylval.identifier = IdEntry::LookUp(yytext);		/* jb: who is responsible for the mem? */
		   switch (yylval.identifier->type) {
			case IdEntry::UndefinedId: return UNDEFIDENTSYM;
			case IdEntry::MethodId:    return METHIDENTSYM;
			case IdEntry::FunctionId:  return FUNCIDENTSYM;
			case IdEntry::RuleId:      return RULEIDENTSYM;
			case IdEntry::RuleElementId: return RULEELEMENTIDENTSYM;
			case IdEntry::ElementId:   return VARIDENTSYM;
			case IdEntry::LabelId:     return LABELIDENTSYM;
			case IdEntry::AttributeId: return ATTRIDENTSYM;
			case IdEntry::ConstantId:  return CONSTIDENTSYM;
			default: yyerror("Internt fel, okänd typ hos identifierare");
				 return UNDEFIDENTSYM;
		   }
                 }
<CODE>[A-ZÅÄÖa-zåäö_][A-ZÅÄÖa-zåäö_0-9]*"@"([A-ZÅÄÖa-zåäö_][A-ZÅÄÖa-zåäö_0-9]*)?      { 
                   if (yytext[yyleng - 1] == '@' && yyleng < 1000) {
                     char buf[1000];
                     strcpy(buf, yytext);
                     buf[yyleng - 1] = '\0';
                     yylval.identifier = IdEntry::LookUp(buf);
                   } else
                     yylval.identifier = IdEntry::LookUp(yytext);
		   switch (yylval.identifier->type) {
			case IdEntry::UndefinedId: return UNDEFIDENTSYMAT;
			case IdEntry::RuleId:      return RULEIDENTSYMAT;
			default: yyerror("Fel! En symbol kan inte innehålla tecknet @");
				 return UNDEFIDENTSYMAT;
		   }
                 }
<CODE>[ \t\r]          ;
<CODE>":="           return ASSIGNSYM;
<CODE>"!="           return NESYM;
<CODE>"<="	         return LESYM;
<CODE>">="	         return GESYM;
<CODE>"-->"          return ARROWSYM;
<CODE>[-+*?/()=<>.&|!~:;,@{}]   return *yytext;
<CODE>"["            return *yytext;
<CODE>"]"            return *yytext;
<CODE>"P"/"("        return PROBSYM;
<CODE>"A"/"("        return FORALLSYM;
<CODE>"E"/"("        return EXISTSYM;
<CODE>"%".*          ; /* kommentar */
<CODE>\n             { BEGIN INITIAL; }
<CODE>.              { yyerror(""); 
                   if (isprint(*yytext))
                     fprintf(stderr, "Okänd symbol: %c\n", *yytext);
                   else
                     fprintf(stderr, "Okänd symbol: %d\n", *yytext);
                 }
%%
int yywrap()
{ return 1; }
