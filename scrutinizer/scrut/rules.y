%{
/* rules.y
 * authors: Viggo Kann and Johan Carlberger
 * last Johan change: 2000-03-24
 * last Viggo change: 1999-11-28
 * comments: Syntaxregler för granskas regelspråk
 */
  
// #define LEXTEST    /* testutskrifter vid användning av lex-attributet */
// #define TESTGLOBAL /* skriv ut alla globala symboler */
// #define TESTEVAL   /* test av uttrycksevaluering */

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libstava.h"
}
#include "scrutinizer.h"
#include "rules.h"
#include "ruleset.h"
#include "rulesettings.h"
//#include "matching.h"
#include "letter.h"

#define MAXNOOFARGUMENTS 10 /* max number of arguments to any method or function in the rule language */
#define MAXLINELENGTH 1000  /* max length of line in rules */


Scrutinizer *scrutinizer;       // slightly faster if not pointer

static int yylex(void);                /* lexical analyzer */
static int yyerror(const char *s);           /* automatically called error reporting function */

/* Definierade i regexps.cc: */
/* CompileRegexpHelp compiles valregexp */
char *CompileRegexpHelp(const char *valregexp);

/* RegexpCheckHelp checks if regular expression in valcompiled matches s */
int RegexpCheckHelp(const char *s, const char *valcompiled);

/* exprUse tells in which context expr is used */
static enum exprusetype {InConst, InLHS, InMark, InCorr, InJump, InInfo, 
			 InAction } exprUse;

			 /*
static bool IsNumber(enum semantictype semtype)
{ return semtype == Integer || semtype == Real; }

static bool IsFeature(enum semantictype semtype)
{ return semtype == SemFeatureClass || semtype == Feature; }

static enum semantictype HighestType(enum semantictype t1, enum semantictype t2)
{
 if (t1 == Real && t2 == Integer) return Real;
 if (t2 == Real && t1 == Integer) return Real;
 return t1;
}
*/

static bool Compatible(const Expr *a, const Expr *b);

int hashcode = 0L;
static int line = 0; /* radnummer */
static char linebuf[MAXLINELENGTH]; /* senaste raden */
static int errorLine = 0; /* radnummer vid senaste anrop av yyerror */
static char errorLineBuf[MAXLINELENGTH]; /* raden vid senaste anrop av yyerror */
RuleSet ruleSet;
/* Variabler som används för uppbyggnad av listor: */
static RuleTerm *firstAltRule = NULL;
static RuleTerm *firstlhs = NULL;
static Element tempElements[MAXNOOFELEMENTS];

static char *currentElementName = NULL;
int currentNoOfElement = 0; /* nr på aktuellt element inom regeln under parsning */
int endLeftContext = -1, beginRightContext = -1; /* kontextgränser inom regeln under parsning */
int noCodeGeneration = 0; /* 1 om kodgenerering inte ska ske */

static Expr *constantAlmostOne, *constantAlmostZero;
Expr *constantTrue, *constantFalse;
static IdEntry *endlabel; /* the label end: */
IdEntry *constantRealText;
IdEntry *constantText;
IdEntry *constantVerbtype;
IdEntry *constantReplace;
IdEntry *constantLemma;

static IdEntry *constantNoOfTokens, *constantToken;
static IdEntry *constantSpellOK, *constantBeginOK, *constantEndOK, *constantIsRepeated;
static IdEntry *constantLength, *constantGetReplacement, *constantGetValues;
 static IdEntry *constantLex, *constantCap, *constantAllCap, *constantManyCap, *constantIsForeign, *constantHyphen;

static Expr *DotExpr(Expr *left, Expr *right);
static Expr *AssignExpr(Expr *lhs, Expr *rhs);
static Expr *IfExpr(Expr *cond, Expr *trueExpr, Expr *elseExpr);

StringBuf infoStringBuf;
StringBuf evalStringBuf;

/* CompileRegexp compiles the regexp in val.string */


static int CompileRegexp(union value&) { return 0; }
static bool RegexpCheck(const char*, union value, enum semantictype) {
  return false; }


%}

%union { 
  double real;
  int integer;
  IdEntry *identifier;
  MethodOrFunctionEntry *methodorfunction;
  GotoEntry *gotoentry;
  char *string;
  Expr *node;
  Rule *rule;
  RuleTerm *ruleTerm;
  Element *element;
}

%start program

%token <integer> INTEGERSYM
%token <real> REALSYM
%token <string> STRINGSYM 
%token <identifier> UNDEFIDENTSYM VARIDENTSYM ATTRIDENTSYM CONSTIDENTSYM
%token <identifier> TAGIDENTSYM CHECKIDENTSYM LOOKUPIDENTSYM LABELIDENTSYM
%token <identifier> EDITIDENTSYM HELPIDENTSYM ACCEPTIDENTSYM RULEIDENTSYM
%token <identifier> RULEELEMENTIDENTSYM UNDEFIDENTSYMAT RULEIDENTSYMAT
%token <methodorfunction> FUNCIDENTSYM METHIDENTSYM 
%token ARROWSYM PROBSYM FORALLSYM EXISTSYM
%token IFSYM THENSYM ELSESYM ENDSYM CONSTSYM GOTOSYM
%token MARKSYM CORRSYM JUMPSYM INFOSYM CATEGORYSYM ACTIONSYM DETECTSYM ACCEPTSYM LINKSYM
%token ENDLEFTCONTEXTSYM BEGINRIGHTCONTEXTSYM
%nonassoc ASSIGNSYM
%left '|'
%left '&'
%left '!'
%nonassoc '=' '<' '>' NESYM LESYM GESYM
%nonassoc '~'
%left '+' '-'
%left NEG
%left '['
%left '.'

%type <node> expr maybeexpr mark corr info action args methodorattribute arglist 
%type <node> exprseq
%type <element> lhs 
%type <ruleTerm> altrule altrules gbglhslist
%type <integer> maybeocc actionident
%type <identifier> maybename elementname helprule 
%type <gotoentry> jump
%%


program	: constdeclarations rules
	;

constdeclarations : /* nothing */
	| constdeclarations CONSTSYM UNDEFIDENTSYM ASSIGNSYM { exprUse = InConst; }
	  expr ';'
	    { $3->IntoGlobalTable(IdEntry::ConstantId);
	      $3->semtype = $6->semtype;
	      $3->u.expr = $6;
	    }
	| constdeclarations CONSTSYM error ';' { ParseError("Fel i konstantdeklaration"); }
	| constdeclarations CONSTSYM error CONSTSYM { ParseError("Konstantdeklarationen måste avslutas med ';'"); YYABORT; }
	;


rules	: category
        | rule
        | rules category
        | rules rule
        ;

category : CATEGORYSYM UNDEFIDENTSYM '{' INFOSYM '(' STRINGSYM ')' LINKSYM '(' STRINGSYM STRINGSYM ')' '}' { ruleSet.AddCategory($2->Name(), $6, $10, $11); }
         ;

rule	: maybename '{' altrules '}' {
           if (firstAltRule->IsHelp())
	     ruleSet.Add(new HelpRule($1, firstAltRule));
	   else
	     ruleSet.Add(new Rule($1, firstAltRule));
	   IdEntry::NewScope();
           }
	| maybename '{' '{' gbglhslist { $<ruleTerm>$ = firstlhs; }
	    '~' gbglhslist ARROWSYM INFOSYM '(' { exprUse = InInfo; } info '}' '}'
          {
	    ruleSet.Add(new GbgRule($1, $<ruleTerm>5, firstlhs, $12));
	    IdEntry::NewScope();
	  }
	| maybename '{' '/' STRINGSYM '/'
	  ARROWSYM { $<ruleTerm>$ = new RuleTerm(NULL, 0); }
          rhslist
           '}'
          {
	    tempElements[0].Init(Element::Regexp, $4, NULL, NULL, ExactlyOne);
	    RuleTerm *r = $<ruleTerm>7;
	    Expr *corr = r->GetCorr();
	    if (corr && corr->semtype == Interval) corr = corr->c.op.Left();
	    if (corr && (corr->semtype != String || corr->type != Expr::Leaf)) {
	      ParseError("Corr-instruktionen i en teckenmatchningsregel måste vara en textsträng");
	      corr = NULL;
	    }
	    ruleSet.Add(new RegExpRule(NewElements(tempElements, 1), $4,
				       $1, r->GetMark(), 
				       corr ? corr->c.string : NULL,
				       r->GetJump(), r->GetInfo(),
				       r->GetAction(),
				       r->GetDetect(), r->GetAccept(), r));
	    IdEntry::NewScope();
	  }
	| maybename '{' '/' error '}' { ParseError("Fel i reguljärt uttrycks-regel"); }
	| maybename '{' error '}' { ParseError("Fel syntax i regelns högerled"); }
	;

newscope : { IdEntry::NewScope(); }
	;

maybename: /* nothing */ { $$ = NULL; }
	| UNDEFIDENTSYM ':' maybename { $1->IntoGlobalTable(IdEntry::LabelId);
					$1->u.ruleNo = ruleSet.NRules();
					$$ = $3; }
	| LABELIDENTSYM ':' maybename { if ($1->u.ruleNo >= 0)
					  ParseErrorArg("Läget %s är redan definierat", $1->name);
					else $1->u.ruleNo = ruleSet.NRules();
					$$ = $3; }
	| UNDEFIDENTSYMAT { $1->IntoGlobalTable(IdEntry::RuleId); 
			      $1->u.ruleNo = ruleSet.NRules();
			      $$ = $1; }
	| RULEIDENTSYMAT  { if ($1->u.ruleNo >= 0) {
			        ParseError("@ ska föregås av ett inte tidigare definierat namn"); $$ = NULL; }
			      else { $1->u.ruleNo = ruleSet.NRules(); $$ = $1; }}
	| '@' { ParseError("@ ska föregås av ett inte tidigare definierat namn"); $$ = NULL; }
	;


altrules: altrule { firstAltRule = $$ = $1; }
	| altrules ';' newscope altrule { $1->SetNext($4); $$ = $4; }
	| error ';' { ParseError("Fel syntax i regelns högerled"); }
                    altrule { $$ = $4; }
	;

altrule	: lhs ARROWSYM { currentElementName = NULL; 
                         $<ruleTerm>$ = new RuleTerm($1, currentNoOfElement, endLeftContext, beginRightContext); }
          rhslist { $$ = $<ruleTerm>3; currentNoOfElement = 0; 
		    if (!$$->IsHelp() && (endLeftContext >= 0 || beginRightContext >= 0))
		      ParseError("Kontext får bara förekomma i hjälpregler.");
	  	    endLeftContext = beginRightContext = -1;
     	            if (!$$->GetAction()) ParseError("Ingen åtgärdsdel i högerledet."); }
	| error ARROWSYM { ParseError("Fel syntax i regelns vänsterled");
                           $<ruleTerm>$ = new RuleTerm(NULL, currentNoOfElement, endLeftContext, beginRightContext); }
		rhslist
	            { currentNoOfElement = 0; 
	  	      endLeftContext = beginRightContext = -1; }
        ;

rhslist : /* nothing */
        | rhslist rhs
        ;

rhs     : MARKSYM '(' setmark mark { $<ruleTerm>-1->SetMark($4->semtype == Interval ?
                                                            $4 : new Expr(' ', Interval, $4, NULL) ); }
        | CORRSYM '(' setcorr corr { $<ruleTerm>-1->SetCorr($4); }
        | JUMPSYM '(' setjump jump { $<ruleTerm>-1->SetJump($4); }
        | INFOSYM '(' setinfo info { $<ruleTerm>-1->SetInfo($4); }
        | ACTIONSYM '(' setaction action { $<ruleTerm>-1->SetAction($4); }
        | DETECTSYM '(' STRINGSYM ')' { if (xCheckAccept) $<ruleTerm>-1->SetDetect($3); }
        | ACCEPTSYM '(' STRINGSYM ')' { if (xCheckAccept) $<ruleTerm>-1->SetAccept($3); }
        | LINKSYM '(' STRINGSYM STRINGSYM ')' { $<ruleTerm>-1->SetLink($3, $4); }
        | error ')' { ParseError("Felaktigt fält i högerledet."); }
	;

setmark : { exprUse = InMark; }
        ;

setcorr : { exprUse = InCorr; }
        ;

setjump : { exprUse = InJump; }
        ;

setinfo : { exprUse = InInfo; }
        ;

setaction : { exprUse = InAction; }
        ;

gbglhslist	: lhs { firstlhs = $$ = new RuleTerm($1, currentNoOfElement); 
		if (endLeftContext >= 0 || beginRightContext >= 0) ParseError("Kontextgränser kan inte ges i denna typ av regel.");
                currentNoOfElement = 0;
		endLeftContext = beginRightContext = -1; }
	| gbglhslist ';' lhs {
		$1 = $$ = new RuleTerm($3, currentNoOfElement); 
		if (endLeftContext >= 0 || beginRightContext >= 0) ParseError("Kontextgränser kan inte ges i denna typ av regel.");
                currentNoOfElement = 0;
		endLeftContext = beginRightContext = -1; }
	;

lhs	: elements { $$ = NewElements(tempElements, currentNoOfElement); }
	;

elements: element
	| elements ',' element 
	| elements elementname { ParseError("Kommatecken utelämnat mellan element i vänsterledet"); }
	  maybeexpr ')'
	| error ',' { ParseError("Fel i elementspecifikationen"); }
	;

element	: elementname 
	  { tempElements[currentNoOfElement].Init(Element::Word, NULL, $1, NULL, ExactlyOne);
	  }
	  maybeexpr ')'
          { if ($3->semtype != Boolean) ParseError("Matchningskravsuttrycket måste vara ett booleskt uttryck"); }
	  maybeocc 
	    {
	      if (currentNoOfElement >= MAXNOOFELEMENTS) {
		ParseError("För många element i samma regel.");
	      } else {
		if ($1->type == IdEntry::RuleElementId && // HelpRule
		    ($6 != ExactlyOne && $6 != 1 && $6 != ZeroOrOne)) {
		  ParseError("Bara ? får förekomma som antal förekomster i hjälpregel.");
		}
		tempElements[currentNoOfElement].Init(Element::Word, NULL, $1, $3, $6);
		currentNoOfElement++; // måste göras efter anropet till Init
	      }
	    }
	| elementname error ')' 
            { ParseError("Fel i matchningskravsuttrycket"); }
	  maybeocc {}
	| ENDLEFTCONTEXTSYM { 
               if (endLeftContext >= 0) ParseError("Flera ENDLEFTCONTEXT i samma vänsterled.");
	       else if (beginRightContext >= 0) ParseError("ENDLEFTCONTEXT måste komma före BEGINRIGHTCONTEXT.");
	       else endLeftContext = currentNoOfElement;
          }
	| BEGINRIGHTCONTEXTSYM { 
               if (beginRightContext >= 0) ParseError("Flera BEGINRIGHTCONTEXT i samma vänsterled.");
	       else beginRightContext = currentNoOfElement;
          }
	;

elementname : UNDEFIDENTSYM '(' { $1->IntoLocalTable();
			          currentElementName = $1->name; 
				  exprUse = InLHS; 
				  $$ = $1; }
	| '(' helprule ')' '(' { $2->IntoLocalTable();
				 $2->type = IdEntry::RuleElementId;
			         currentElementName = $2->name; 
				 exprUse = InLHS;
				 $$ = $2; }
	| '(' error ')' '(' { ParseError("En hjälpregel måste anges inom (...)"); $$ = NULL; }
	| error '(' { ParseError("Elementets namn måste vara ett ännu inte definierat namn"); $$ = NULL; }
	;

helprule : UNDEFIDENTSYM { $1->IntoGlobalTable(IdEntry::RuleId);
			   $1->u.ruleNo = -1; 
			   $$ = new IdEntry($1->name); // jb: who is responsible for the mem?
			   $$->u.re.ruleId = $1;
			 }
	| RULEIDENTSYM   {
			   $$ = new IdEntry($1->name);
			   $$->u.re.ruleId = $1;
			 }
	| RULEIDENTSYMAT {
			   $$ = new IdEntry($1->name);
			   $$->u.re.ruleId = $1;
			 }
	| UNDEFIDENTSYM '/' UNDEFIDENTSYM {
			   $1->IntoGlobalTable(IdEntry::RuleId);
			   $1->u.ruleNo = -1; 
			   $$ = $3;
			   $$->u.re.ruleId = $1;
			 }
	| RULEIDENTSYM '/' UNDEFIDENTSYM { 
			   $$ = $3;
			   $$->u.re.ruleId = $1;
			 }
	| RULEIDENTSYMAT '/' UNDEFIDENTSYM { 
			   $$ = $3;
			   $$->u.re.ruleId = $1;
			 }
	;

maybeocc : /* nothing */ { $$ = ExactlyOne; }
	| INTEGERSYM { if ($1 == 0) 
	                 ParseError("noll förekomster av ett element är orimligt");
	               $$ = $1; }
	| '*' { $$ = ZeroOrMore; }
	| '+' { $$ = OneOrMore; }
	| '?' { $$ = ZeroOrOne; }
	;

mark	: exprseq ')'
	| error ')' { ParseError("Fel i högerledets märkningsdel"); $$ = NULL; }
	;

corr	: ')' { $$ = NULL; }
        | exprseq ')' { if ($1->semtype != Interval) $$ = new Expr(' ', Interval, $1, NULL); }
	| error ')' { ParseError("Fel i högerledets korrektionsdel"); $$ = NULL; }
	;

jump	: ')' { $$ = NULL; }
	| UNDEFIDENTSYM ')' { $1->IntoGlobalTable(IdEntry::LabelId);
					$1->u.ruleNo = -1;
					$$ = new GotoEntry($1, NULL); }
	| LABELIDENTSYM ')' {
		if ($1->u.ruleNo >= 0 && $1->u.ruleNo <= ruleSet.NRules())
		  ParseError("Förbjudet att hoppa bakåt i regelsamlingen");
		$$ = new GotoEntry($1, NULL); }
	| UNDEFIDENTSYM ',' expr ')' { 
	                                $1->IntoGlobalTable(IdEntry::LabelId);
					$1->u.ruleNo = -1;
					if ($3->semtype != Integer)
					  ParseError("Andra parametern till GOTO TOKEN måste vara ett heltal.");
					$$ = new GotoEntry($1, $3); }
	| LABELIDENTSYM ',' expr ')' {
	        if ($3->semtype != Integer)
		  ParseError("Andra parametern till GOTO TOKEN måste vara ett heltal.");
		$$ = new GotoEntry($1, $3); }
	| error ')' { ParseError("Fel i högerledets hoppdel"); $$ = NULL; }
	;

info	: ')'  { $$ = NULL; }
	| exprseq ')'
	| error ')' { ParseError("Fel i högerledets kommentardel"); $$ = NULL; }
	;

action	: ')' { $$ = NULL; }
	| actionident ')' { $$ = new Expr($1, NoType, NULL, NULL); }
	| actionident ',' args ')' { if ($1 != HELPIDENTSYM && $1 != TAGIDENTSYM) 
ParseError("Argument kan bara ges till åtgärderna HJÄLP och TAGGNING");
                                 $$ = new Expr($1, NoType, $3, NULL); }
	| error ')' { ParseError("Fel i högerledets åtgärdsdel"); $$ = NULL; }
	;

actionident : TAGIDENTSYM { $$ = TAGIDENTSYM; }
	| CHECKIDENTSYM   { $$ = CHECKIDENTSYM; }
	| LOOKUPIDENTSYM  { $$ = LOOKUPIDENTSYM; }
	| EDITIDENTSYM    { $$ = EDITIDENTSYM; }
	| HELPIDENTSYM    { $$ = HELPIDENTSYM; }
	| ACCEPTIDENTSYM  { $$ = ACCEPTIDENTSYM; }
	;

maybeexpr: /* nothing */ { $$ = constantTrue; }
	| expr
	;

expr    : REALSYM            { $$ = new Expr(Expr::Leaf, Real);
                               $$->c.real = $1; }
	| INTEGERSYM	     { $$ = new Expr(Expr::Leaf, Integer);
                               $$->c.integer = $1; }
	| STRINGSYM	     { $$ = new Expr(Expr::Leaf, String);
                               $$->c.string = $1; }
        | VARIDENTSYM        { $$ = new Expr(Expr::Leaf, Variable);
                               $$->c.id = $1; }
        | RULEELEMENTIDENTSYM  { $$ = new Expr(Expr::Leaf, Variable);
                                 $$->c.id = $1; }
	| methodorattribute
        | FUNCIDENTSYM arglist { Expr *p = new Expr(Expr::Function, $1->semtype);
                                 p->c.method.id = $1;
                                 p->c.method.actuals = $2;
				 $1->ParametersOK($2);
				 $$ = p; }
        | CONSTIDENTSYM	     { $$ = $1->u.expr; }
	| UNDEFIDENTSYM	     { ParseErrorArg("Namnet %s är inte definierat", $1->name); }
        | expr '.' methodorattribute { $$ = DotExpr($1, $3); }
	| expr '.' error     { ParseError("Okänd metod eller attribut efter ."); }
        | expr '+' expr      { $$ = new Expr('+', $1, $3); }
        | expr '-' expr      { if ($1->semtype == Variable && 
				   $3->semtype == Variable && 
				   (exprUse == InMark || exprUse == InCorr)) {
				 $$ = new Expr('-', Interval, $1, $3);
			       } else {
				 if (!IsNumber($1->semtype) ||
				     !IsNumber($3->semtype)) {
				   ParseError("Båda operanderna till - ska vara tal eller variabler.");
			         }
			         $$ = new Expr('-', HighestType($1->semtype,
				             $3->semtype), $1, $3); }
			       }
        | expr '=' expr      { if (!Compatible($1, $3)) 
				 ParseError("Båda operanderna till = måste ha samma typ.");
			       $$ = new Expr('=', Boolean, $1, $3); }
        | expr '~' expr      { if (($1->semtype == String || $1->semtype == Variable)
				   && $3->semtype == String) {
				 if ($3->type == Expr::Leaf) {
				   if (CompileRegexp($3->c)) $3->semtype = Regexp;
				   else ParseErrorArg("Felaktigt reguljärt uttryck: %s", $3->c.string);
			         }
			       } else {
				 ParseError("Båda operanderna till ~ måste ha strängtyp.");
			       }
			       $$ = new Expr('~', Boolean, $1, $3); }
        | expr NESYM expr      { if (!Compatible($1, $3))
	                           ParseError("Båda operanderna till != måste ha samma typ.");
 			         $$ = new Expr(NESYM, Boolean, $1, $3); }
        | expr LESYM expr    { $$ = new Expr(LESYM, $1, $3); 
			       $$->semtype = Boolean; }
        | expr GESYM expr    { $$ = new Expr(GESYM, $1, $3);
			       $$->semtype = Boolean; }
        | expr '<' expr      { $$ = new Expr('<', $1, $3);
			       $$->semtype = Boolean; }
        | expr '>' expr      { $$ = new Expr('>', $1, $3);
			       $$->semtype = Boolean; }
        | expr '&' expr      { if (!($1->semtype == Boolean &&
				     $3->semtype == Boolean)) {
				 ParseError("Båda operanderna till & måste vara booleska."); }
			       $$ = new Expr('&', Boolean, $1, $3); }
        | expr '|' expr      { if (!($1->semtype == Boolean &&
				     $3->semtype == Boolean)) {
				 ParseError("Båda operanderna till | måste vara booleska."); }
			       $$ = new Expr('|', Boolean, $1, $3); }
        | expr ASSIGNSYM expr { $$ = AssignExpr($1, $3); }
	| '!' expr	     { if ($2->semtype != Boolean) {
				 ParseError("Operanden till ! måste vara boolesk."); }
			       $$ = new Expr('!', Boolean, $2, NULL); }
        | '(' '-' expr ')'   { if (!(IsNumber($3->semtype))) {
				 ParseError("Operanden till - måste vara ett tal."); }
			       $$ = new Expr(NEG, $3->semtype, $3, NULL); }
        | '(' expr ')'       { $$ = $2; }
	| expr '[' expr ']'  { if (!($1->semtype == Variable &&
				     $3->semtype == Integer)
				   || $1->c.id->Elt(tempElements) == NULL) {
				 ParseError("Felaktiga operander till arrayindexeringen."); }
				else if (!($1->c.id->Elt(tempElements)->MultiOcc() ||
					  $1->c.id->type == IdEntry::RuleElementId)) {
	     			 ParseError("Bara hjälpregelelement och element med + eller * kan indexeras."); }
			       $$ = new Expr('[', Variable, $1, $3); }
	| PROBSYM '(' expr ',' expr ')' { if (!(IsNumber($3->semtype) &&
				     $5->semtype == Boolean)) {
				 ParseError("Felaktiga parametrar till P-funktionen."); }
			       $$ = new Expr(PROBSYM, Boolean, $3, $5); }
	| FORALLSYM '(' expr ')' { if ($3->semtype != Boolean) {
				 ParseError("Felaktig parameter till FÖR ALLA-funktionen."); }
				   $$ = new Expr(PROBSYM, Boolean, constantAlmostOne, $3); }
	| EXISTSYM '(' expr ')' { if ($3->semtype != Boolean) {
				    ParseError("Felaktig parameter till EXISTERAR-funktionen."); }
				  $$ = new Expr(PROBSYM, Boolean, constantAlmostZero, $3); }
	| IFSYM expr THENSYM { if ($2->semtype != Boolean)
				 ParseError("Ett villkor väntades efter IF."); }
	  exprseq ELSESYM exprseq ENDSYM { $$ = IfExpr($2, $5, $7); }
	| IFSYM error ENDSYM { ParseError("Fel i IF-sats."); }
        ;

methodorattribute:
	  ATTRIDENTSYM	     { Expr *p = new Expr(Expr::Attribute, $1->semtype);
                               p->c.id = $1;
			       if (IsFeature($1->semtype)) 
				 p->featureClass = $1->u.featureClass;
			       $$ = p; }
        | METHIDENTSYM arglist { Expr *p = new Expr(Expr::Method, $1->semtype);
                                 p->c.method.id = $1;
                                 p->c.method.actuals = $2;
				 $1->ParametersOK($2);
			         if (IsFeature($1->semtype)) 
				   p->featureClass = $1->featureClass;
				 $$ = p; }
	;

arglist	: '(' args ')' { $$ = $2; }
        | '(' ')' { $$ = NULL; }
	| error ')'    { ParseError("Fel i parameterlistan"); $$ = NULL; }
	;

args	: expr          { $$ = new Expr(',', NoType, $1, NULL); }
	| expr ',' args { $$ = new Expr(',', NoType, $1, $3); }
	;

exprseq	: expr
	| expr exprseq { if (exprUse != InMark && exprUse != InInfo && exprUse != InCorr)
				 ParseError("Flera uttryck kan inte radas upp");
			       else if (($1->semtype != Variable &&
                                         $1->semtype != CorrT && 
				         $1->semtype != Interval &&
				         $1->semtype != String) ||
				        ($2->semtype != Variable && 
				         $2->semtype != Interval &&
                                         $2->semtype != CorrT && 
				         $2->semtype != String))
				 ParseError("Bara variabler och texter kan användas här");
			       $$ = new Expr(' ', Interval, $1, $2);
			     }
	;
%%

#include "lex.yy.c"

	/* Semantic functions used as actions above */
static Expr *DotExpr(Expr *left, Expr *right) {
  if (left->semtype != Variable &&
      left->semtype != LexVariable &&
      left->semtype != String) {
    ParseError("En variabel måste ges före .");
    return NULL;
  }
  if (currentElementName && 
      left->type==Expr::Leaf && left->semtype == Variable &&
      strcmp(left->c.id->Name(), currentElementName) == 0) {
    errorLine = line;
    ParseWarningArg("Varning: %s är aktuell variabel och behöver inte anges i punkt-uttrycket\n", currentElementName);
    return right;
  }
  enum semantictype semtype = right->semtype;
  if (semtype == LexVariable) semtype = Variable;
  Expr *p = new Expr('.', semtype, left, right); 
  if (IsFeature(right->semtype))
    p->featureClass = right->GetFeatureClass();
  return p;
}

static Expr *AssignExpr(Expr *lhs, Expr *rhs) {
  if (exprUse != InCorr && exprUse != InAction)
    ParseError("Tilldelning är inte tillåtet här");
  else if (exprUse == InCorr && lhs->type != Expr::Attribute)
    ParseError("Bara attribut tillåts i vänsterledet i en tilldelning.");
  else if (exprUse == InAction && lhs->type != Expr::Attribute &&
	       !(lhs->type == Expr::Operation && lhs->c.op.Op() == '.'
	       && lhs->c.op.Right()->type == Expr::Attribute))
    ParseError("Bara attribut tillåts i vänsterledet i en tilldelning.");
  else if (!Compatible(lhs, rhs))
    ParseError("Olika typ i höger- och vänsterled i tilldelning.");
  return new Expr(ASSIGNSYM, lhs->semtype, lhs, rhs); 
}

static Expr *IfExpr(Expr *condition, Expr *trueExpr, Expr *elseExpr) {
  int fclass = 0;
  if (trueExpr->type == Expr::Operation && trueExpr->c.op.Op() == ' ' &&
      exprUse != InMark && exprUse != InCorr)
    ParseError("Uppradning av uttryck är inte tillåtet efter THEN");
  else if (elseExpr->type == Expr::Operation && elseExpr->c.op.Op() == ' ' &&
	   exprUse != InMark && exprUse != InCorr)
    ParseError("Uppradning av uttryck är inte tillåtet efter ELSE");
  else if (IsFeature(trueExpr->semtype) && IsFeature(elseExpr->semtype)) {
    if (trueExpr->GetFeatureClass() == 0) fclass = elseExpr->GetFeatureClass(); else
      if (elseExpr->GetFeatureClass() == 0) fclass = trueExpr->GetFeatureClass(); else
	if (trueExpr->GetFeatureClass() == elseExpr->GetFeatureClass()) fclass = trueExpr->GetFeatureClass(); else
	  ParseError("Uttrycken efter THEN och ELSE har olika särdragsklasser.");
  } else {
    enum semantictype type = CoerseTypes(trueExpr->semtype, elseExpr->semtype);
    if (type == SemanticError)
      ParseError("Uttrycken efter THEN och ELSE har olika typ.");
    else {
      Expr *p = new Expr(IFSYM, type, condition,
			 new Expr(ELSESYM, type, trueExpr, elseExpr));
      p->featureClass = fclass;
      return p; 
    }
  }
  return NULL;
}


/* Semantiska rutiner som bygger upp den intermediära strukturen: */

static bool Compatible(const Expr *a, const Expr *b) {
  if (!a || !b) return false;
  if (IsFeature(a->semtype) && IsFeature(b->semtype)) {
    if (a->GetFeatureClass() != b->GetFeatureClass() &&
	a->GetFeatureClass() && b->GetFeatureClass()) return false;
    else return true;
  }
  if ((IsNumber(a->semtype) && IsNumber(b->semtype)) ||
      a->semtype == b->semtype) return true;
  else return false;
}
				 
char *value::ToString(enum semantictype semtype) const {
  char buf[100], *s;
  buf[0] = '\0';
  switch (semtype) {
  case Boolean:	sprintf(buf, "%s", boolean == 0 ? "false" : "true"); break;
  case Integer:	sprintf(buf, "%d", integer); break;
  case Real:	sprintf(buf, "%f", real); break;
  case String:	sprintf(buf, "%s", string); break;
  case Regexp:	sprintf(buf, "%s", regexp.regexp); break;
  case Variable:	sprintf(buf, "%s", evalVar.RealString()); break;
  case LexVariable:	sprintf(buf, "lex-variabel"); break;
  case Interval:	sprintf(buf, "intervall"); break;
  case SemFeatureClass:
  case Feature:	sprintf(buf, "%s", scrutinizer->Tags().GetFeature(feature).Name()); break;
  default: sprintf(buf, "okänd semantisk typ");
  }
// jbfix: purify claims memory leak
#if 0
  s = new char[strlen(buf) + 1];
#else
  s = infoStringBuf.NewString(strlen(buf) + 1);	
#endif
  strcpy(s, buf);
  return s;
}

void value::Print(enum semantictype semtype) const {
  switch (semtype) {
  case Boolean:	printf("%s", boolean == 0 ? "false" : "true"); break;
  case Integer:	printf("%d", integer); break;
  case Real:	printf("%f", real); break;
  case String:	printf("\"%s\"", string); break;
  case Regexp:	printf("\'%s\'", regexp.regexp); break;
  case Variable:	id->Print(); break;
  case LexVariable:	printf("lex-variabel"); break;
  case Interval:	printf("intervall"); break;
  case Feature:	printf("%s", scrutinizer->Tags().GetFeature(feature).Name()); break;
  case SemFeatureClass:	printf("särdragsklass (borde inte uppkomma)"); break;
  default: printf("okänd semantisk typ");
  }
}

Element *IdEntry::Elt(Element *environment) const { // ElementId, element {
  if (environment == NULL) return NULL;
  int index;
  switch (type) {
    case ElementId: index = u.elementIndex; break;
    case RuleElementId: index = u.re.elementIndex; break;
    default: return NULL;
  }
  return &environment[index];
}

/* RulesOK checks help rules and checks that labels and help rules that are
   used also are defined */
bool IdEntry::RulesOK(void) {
  bool OK = ruleSet.FixRules();
  endlabel->u.ruleNo = ruleSet.NRules();
  const IdEntry *s;
  for (int i=0; (s = (*globalsTable)[&i]); i++)
    if (s->type == LabelId && s->u.ruleNo < 0) {
      ParseErrorArg("Läget %s definieras aldrig", s->name);
      OK = false;
    }
  return OK;
}

/* Felmeddelandeutskrift, förhindrar kodgenerering: */
void ParseErrorArg(const char *s, const char *arg) {
  ParseWarningArg(s, arg);
  noCodeGeneration = 1;
}

void ParseError(const char *s) {
  ParseErrorArg(s, NULL);
}

/* Varningsmeddelandeutskrift, förhindrar inte kodgenerering: */
void ParseWarningArg(const char *s, const char *arg) {
  char buf[10000];
  sprintf(buf, s, arg);
  if (errorLine) {
    Message(MSG_WARNING, "row", int2str(errorLine), errorLineBuf);
    errorLine = 0;
  } else
    Message(MSG_WARNING, "row", int2str(line), ":", linebuf);
  Message(MSG_CONTINUE, buf);
}

static int yyerror(const char*) {
  errorLine = line;
  strcpy(errorLineBuf, linebuf);
  return 0;
}

/* ParametersOK checks whether the number and types of parameters are
correct. If the parameters match 1 is returned, otherwise 0 is returned */
int MethodOrFunctionEntry::ParametersOK(Expr *actuals) const {
  int i;
  char buf[MAXLINELENGTH];
  if (noofargs == variablenoofargs) return 1;
  for (i = 0; i < noofargs && actuals;
	i++, actuals = actuals->c.op.Right()) {
    if (argtypes[i] != AnyType && 
	actuals->c.op.Left()->semtype != argtypes[i]) {
      sprintf(buf, "Fel på parameter %d i anropet av %s", i+1, Name());
      ParseError(buf);
      return 0;
    }
  }
  if (i != noofargs) {
    sprintf(buf, "För få parametrar (%d istället för %d) i anropet av %s", i, noofargs, Name());
    ParseError(buf);
    return 0;
  }
  if (actuals) {
    ParseErrorArg("För många parametrar i anropet av %s", Name());
    return 0;
  }
  return 1;
}

Expr *CreateConstant(const char *name, enum semantictype semtype) {
  IdEntry *p = IdEntry::LookUp(name);
  if (p->type != IdEntry::UndefinedId) return NULL;
  p->IntoGlobalTable(IdEntry::ConstantId);
  p->semtype = semtype;
  p->u.expr = new Expr(Expr::Leaf, semtype);
  return p->u.expr;
}

Expr *CreateFeature(const char *name, int fclass, int value) { 
  Expr *p = CreateConstant(name, Feature);
  if (!p)
    Message(MSG_ERROR, name, "is undefined");
  p->featureClass = fclass;
  p->c.feature = value;
  return p;
}

IdEntry *CreateFeatureClass(const char *name, int fclass) {
  IdEntry *id = new IdEntry(name, IdEntry::AttributeId, SemFeatureClass);
  id->u.featureClass = fclass;
  return id;
}

GotoEntry::GotoEntry(IdEntry *id_, Expr *nooftokens_) :
     id(id_), nooftokens(nooftokens_) {
}

void GotoEntry::Print() const {
  std::cout << "(" << id << "," << nooftokens << ")";
}

MethodOrFunctionEntry::MethodOrFunctionEntry(const char *name_, enum idtype type_,
			  enum semantictype semtype_, int noofargs_)
  : IdEntry(name_), argtypes(0)
{
  type = type_;
  semtype = semtype_;
  noofargs = noofargs_;
  if (noofargs_ == variablenoofargs)
    argtypes = NULL;
  else {
    if (noofargs == 0)
      argtypes = NULL;
    else if (noofargs_ < 1 || noofargs > 100)
      Message(MSG_ERROR, Name(), "has ", int2str(noofargs), "arguments");
    else
      argtypes = new semantictype[noofargs];
  }
  featureClass = 0;
  IntoGlobalTable(type_);
}

MethodOrFunctionEntry::~MethodOrFunctionEntry()
{
    delete [] argtypes;
}

#include "eval.cpp"

static void colorfunc(MethodOrFunctionEntry*, EvaluatedVariable *p, const char *t,
		      const Expr*, union value[], union value &res) {
  if (p != NULL)
    t = p->GetWordToken()->LexString();
  res.string = t;
}

static void subfunc(MethodOrFunctionEntry*, EvaluatedVariable *p, const char *t, const Expr*,
		    union value argval[], union value &res) {
  //  cout << "subfunc called" << endl;
  int startpos = argval[0].integer;
  int len = argval[1].integer, tlen;
  if (p != NULL) t = p->GetWordToken()->LexString(); // GetWord()-> ?? johan
  tlen = strlen(t);
  if (startpos >= tlen) len = 0; else
  if (startpos + len > tlen) len = tlen - startpos;
  if (len > 0) {
    char *s = evalStringBuf.NewString(len+1); //new char[len + 1];
    strncpy(s, t + startpos, len);
    s[len] = '\0';
    res.string = s;
  } else
    res.string = "";
}

static bool AddForms(CorrThing *ct, WordTag *wt, ChangeableTag &tag) {
  const Tag *tag2;
  bool tagFound = false;
  for (int n=-1; (tag2 = tag.FindMatchingTag(n));) {
    tagFound = true;
    for (int j=0; j<wt->NLemmas(); j++)
      for (int k=0; k<wt->Lemma(j)->NInflectRules(); k++) {
	const WordTag *wt2 = wt->GetForm(tag2, j, k);
	if (wt2)
	  ct->Add(wt2->GetWord(), wt2->String());
      }
  }
  return tagFound;
}

static void formFunc(MethodOrFunctionEntry*, EvaluatedVariable *p, const char *t,
		     const Expr *args, union value argval[], union value &res) {
    res.corrThing = NULL;
    for (int wordTokenIterator=0; wordTokenIterator<p->NMatchedWordTokens(); wordTokenIterator++) {
	WordTag *wt;
	if (p == NULL) {
	    wt = scrutinizer->FindMainOrNewWord(t);
	    ensure(wt);
	} else
	    wt = p->GetWordToken(wordTokenIterator)->GetWordTag();
	//    cout << p->GetWordToken(i) << endl;
	WordToken *tok = p->GetWordToken(wordTokenIterator);
	CorrThing *ct = new CorrThing(CORR_REPLACE, tok); // new OK
	if (res.corrThing)
	    ct->SetNext(res.corrThing);
	res.corrThing = ct;
	ChangeableTag tag(*tok->SelectedTag()); // hur gör vi när p==NULL undrar Viggo
	int argIterator = 0;
	for (const Expr *argp = args; argp; argp = argp->c.op.Right(), argIterator++) {
	    const Expr *arg = argp->c.op.Left();
	    if (arg->type != Expr::Operation || arg->c.op.Op() != ASSIGNSYM) {
		Message(MSG_WARNING, "a parameter to form() is not an assignment");
	    } else {
		const Expr *attr = arg->c.op.Left();
		if (attr->semtype == String && attr->c.id == constantLemma) {
		    ensure(0);
		    //	wt = scrutinizer->FindMainOrNewWord(argval[argIterator].string);
		    for (int j=0; j<wt->NLemmas(); j++) {
			Word *wt2 = wt->Lemma(j)->GetWord();
			ct->Add(wt2, wt2->String());
		    }
		    return;	  
		} else if (attr->semtype == SemFeatureClass) {
		    const int fClass = attr->GetFeatureClass();
		    const int fValue = argval[argIterator].feature;
		    tag.SetFeature(fClass, fValue);
		} else
		    Message(MSG_WARNING, "non-feature assigned in form()");
	    }
	}
	bool tagFound = AddForms(ct, wt, tag);
	if (!tagFound) {
	    Message(MSG_WARNING, "form() cannot find the wanted tag",
		tag.String());
	    continue;
	} else if (ct->NStrings() == 0) // test using other word-tags of same word:
	    for (WordTag *wt2 = wt->GetWord(); wt2; wt2 = wt2->Next())
		if (wt2 != wt)
		    AddForms(ct, wt2, tag);
		if (ct->NStrings() == 0) {
		    Message(MSG_MINOR_WARNING, "form() cannot find the", tag.String(), "form of");
		    Message(MSG_CONTINUE, wt->String(), wt->GetTag()->String(),
			int2str(wt->GetWord()->InflectRule(0)));
		}
    }
}

static void concatfunc(MethodOrFunctionEntry*, const Expr*,
		       union value argval[], union value &res) {
  char *s = infoStringBuf.NewString(strlen(argval[0].string) + strlen(argval[1].string) + 1);
  sprintf(s, "%s%s", argval[0].string, argval[1].string);
  res.string = s;
}

static void smart_concatfunc(MethodOrFunctionEntry*, const Expr*,
		       union value argval[], union value &res) {
  // concatenates "jobb beskrivning" to "jobbeskrivning"
  const int len0 = strlen(argval[0].string);
  char *s = infoStringBuf.NewString(len0 + strlen(argval[1].string) + 1);
  if (len0 > 1 && IsConsonant(argval[1].string[0]) &&
      argval[0].string[len0-1] == argval[1].string[0] &&
      argval[0].string[len0-1] == argval[0].string[len0-2])
    sprintf(s, "%s%s", argval[0].string, argval[1].string+1);
  else
    sprintf(s, "%s%s", argval[0].string, argval[1].string);
  //  cout << argval[0].string << " + " <<  argval[1].string << " = " << s << endl;
  res.string = s;
}

static void tolowerfunc(MethodOrFunctionEntry*, const Expr*,
			union value argval[], union value &res) {
  char *s = infoStringBuf.NewString(argval[0].string);
  ToLower(s);
  res.string = s;
}

static void toupperfunc(MethodOrFunctionEntry*, const Expr*,
			union value argval[], union value &res) {
  char *s = infoStringBuf.NewString(argval[0].string);
  ToUpper(s);
  res.string = s;
}

static void firsttoupperfunc(MethodOrFunctionEntry*, const Expr*,
			     union value argval[], union value &res) {
  char *s = infoStringBuf.NewString(argval[0].string);
  *s = Upper(*s);
  res.string = s;
}

static void italicfunc(MethodOrFunctionEntry *, const Expr*,
		       union value argval[], union value &res) {
  if (argval[0].string[0] == '\0')
    res.string = "";
  else {
    char *s = infoStringBuf.NewString(strlen(argval[0].string) + strlen(xItalic) + 
				      strlen(xNoItalic) + 1);
    sprintf(s, "%s%s%s", xItalic, argval[0].string, xNoItalic);
    res.string = s;
  }
}

static void boldfunc(MethodOrFunctionEntry *, const Expr*,
		     union value argval[], union value &res) {
  char *s = infoStringBuf.NewString(strlen(argval[0].string) + 8);
  sprintf(s, "<B>%s</B>", argval[0].string);
  res.string = s;
}

static void tostringfunc(MethodOrFunctionEntry*, const Expr* args,
			 union value argval[], union value &res) {
  res.string = argval[0].ToString(args->c.op.Left()->semtype);
}

static void strangesentencefunc(MethodOrFunctionEntry*, const Expr *args,
				union value [], union value &res) { 
  const AbstractSentence *s = args->CurrentMatching()->GetSentence();
  res.boolean = s->SeemsForeign();
}

static void containsrepeatedwordsfunc(MethodOrFunctionEntry*, const Expr *args,
				union value [], union value &res) { 
  const AbstractSentence *s = args->CurrentMatching()->GetSentence();
  res.boolean = s->ContainsRepeatedWords();
}

static void containsStyleWordFunc(MethodOrFunctionEntry*, const Expr *args,
				union value [], union value &res) { 
  const AbstractSentence *s = args->CurrentMatching()->GetSentence();
  res.boolean = s->ContainsStyleWord();
}

static void CollectSpellSuggestions(CorrThing *ct, char *corr) {
  if (!corr) return;
  int n = 0;

  /* This strtok call needs to be consistent with the version of Stava being used */
  /* for (const char *s = strtok(corr, "\t"); s; s = strtok(NULL, "\t")) { */
  for (const char *s = strtok(corr, ","); s; s = strtok(NULL, ",")) { 
    ct->Add(NULL, s);
    if (++n >= xMaxSpellSuggestions)
      break;
  }
}

static void correctlyspelledfunc(MethodOrFunctionEntry*, const Expr*,
				 union value argval[], union value &res) {
  res.boolean = scrutinizer->IsSpellOK(argval[0].string, (Token)argval[1].integer);
  return;
}

static void spellCorrectFunc(MethodOrFunctionEntry*, EvaluatedVariable *ev, const char*, 
			     const Expr*, union value argval[], union value &res) {
  char *corr = scrutinizer->SpellOK(argval[0].string, (Token)argval[1].integer);
  res.corrThing = new CorrThing(CORR_REPLACE, ev->GetWordToken()); // new OK
  CollectSpellSuggestions(res.corrThing, corr);
  return;
}

static void correctlyspelledsoundfunc(MethodOrFunctionEntry*, const Expr*,
				 union value argval[], union value &res) {
  res.boolean = scrutinizer->IsSoundSpellOK(argval[0].string, (Token)argval[1].integer);
  return;
}

static void spellcompoundcheckfunc(MethodOrFunctionEntry*, EvaluatedVariable*, const char*, 
				   const Expr*, union value argval[], union value &res) {
  //  if (xVerbose)
  //    cout << xCurrentRule << " compound: '" << argval[0].string << "'" << endl;
  /*
  const char *string = NULL;
  if (argval[1].integer == TOKEN_SPLIT_WORD) {
    for (int p = strlen(argval[0].string)-2; p>0; p--)
      if (IsSpace(argval[0].string[p])) {
	string = argval[0].string+p+1;
	break;
      }
    ensure(string);
  } else if (argval[1].integer == TOKEN_ABBREVIATION) {
    res.boolean = true;
    return;
  } else
    string = argval[0].string;
  */
  if (argval[1].integer == TOKEN_SIMPLE_WORD)
    res.boolean = (StavaCorrectCompound((unsigned char *) argval[0].string)) ? true : false;
  else
    res.boolean = true;
}

static void spellCorrectCompoundFunc(MethodOrFunctionEntry*, EvaluatedVariable *ev, const char*, 
				     const Expr*, union value argval[], union value &res) {
  char *corr = NULL;
  if (argval[1].integer == TOKEN_SPLIT_WORD) {
    for (int p = strlen(argval[0].string)-2; p>0; p--)
      if (IsSpace(argval[0].string[p])) {
	corr = (char *) StavaCorrectCompound((unsigned char *) argval[0].string+p+1);
	break;
      }
  } else
    corr = (char *) StavaCorrectCompound((unsigned char *) argval[0].string);
  res.corrThing = new CorrThing(CORR_REPLACE, ev->GetWordToken()); // new OK
  CollectSpellSuggestions(res.corrThing, corr);
}

static void spellinfofunc(MethodOrFunctionEntry*, EvaluatedVariable*, const char*, 
			  const Expr*, union value argval[], union value &res) {
  switch ((Token)argval[0].integer) {
  case TOKEN_SPLIT_WORD:
    res.string = "Okänt ord bland uppdelade ord"; break;
  case TOKEN_TIME:
    res.string = "Okonventionellt eller felaktigt skrivsätt av klockslag"; break;
  case TOKEN_DATE:
    res.string = "Okonventionellt eller felaktigt skrivsätt av datum"; break;
  case TOKEN_YEAR:
    res.string = "Okonventionellt eller felaktigt skrivsätt av årtal (nyrad?)"; break;
  case TOKEN_PARAGRAPH:
    res.string = "Okonventionellt eller felaktigt skrivsätt av paragraf"; break;
  case TOKEN_BAD_CARDINAL:
    res.string = "Okonventionellt eller felaktigt skrivsätt av tal"; break;
  case TOKEN_PERCENTAGE:
    res.string = "Okonventionellt eller felaktigt skrivsätt av procentuttryck"; break;
  case TOKEN_ABBREVIATION:
    res.string = "Okänd förkortning eller förkortning av oförkortat ord i texten"; break;
  default:
    res.string = "Okänt ord";
  }
}

static void joinFunc(MethodOrFunctionEntry*, EvaluatedVariable *ev, const char*, 
		     const Expr*, union value argval[], union value &res) {
  res.corrThing = new CorrThing(CORR_JOIN, ev->GetWordToken()); // new OK
  res.corrThing->Add(NULL, argval[0].string);
}

static void replaceFunc(MethodOrFunctionEntry*, EvaluatedVariable *ev, const char*, 
			const Expr*, union value argval[], union value &res) {
  res.corrThing = new CorrThing(CORR_REPLACE, ev->GetWordToken()); // new OK
  res.corrThing->Add(NULL, argval[0].string);
}

static void insertFunc(MethodOrFunctionEntry*, EvaluatedVariable *ev, const char*, 
		       const Expr*, union value argval[], union value &res) {
  res.corrThing = new CorrThing(CORR_INSERT, ev->GetWordToken()); // new OK
  res.corrThing->Add(NULL, argval[0].string);
}

static void deleteFunc(MethodOrFunctionEntry*, EvaluatedVariable *ev, const char*, 
		       const Expr*, union value argval[], union value &res) {
  res.corrThing = new CorrThing(CORR_DELETE, ev->GetWordToken()); // new OK
}

static void doNothingFunc(MethodOrFunctionEntry*, EvaluatedVariable *ev, const char*, 
			  const Expr*, union value argval[], union value &res) {
  res.corrThing = new CorrThing(CORR_DO_NOTHING, ev->GetWordToken()); // new OK
}


static void token_start(MethodOrFunctionEntry     *me,
                        EvaluatedVariable         *ev,
                        const                      char*,
                        const Expr                *args,
                        union value                argval[],
                        union value               &res)
{
    res.integer = 0;
}


#ifdef PROBCHECK
// jb: added the probabilistic checker 2001-04-23
#include "prob.h"
static void prob_check(MethodOrFunctionEntry    *,
                       EvaluatedVariable        *ev,
                       const                     char*,
                       const Expr               *args,
                       union value               argval[],
                       union value              &res)
{
#if 1
    res.integer =
    Prob::prob_check(scrutinizer, args->CurrentMatching()->GetSentence());
#else
    static int count = 0;
    res.integer = count++;
#endif

}

static void last_prob_err(MethodOrFunctionEntry     *w,
                          const Expr                *args,
                          union value                argval[],
                          union value               &res)
{
    res.integer = Prob::last_prob_error();
}

#endif // PROBCHECK


#ifdef TRANSITIVITY
// jb: added transitivity 2001-06-18
#include "trans.h"
static void intrans(MethodOrFunctionEntry     *w,
                    const Expr                *args,
                    union value                argval[],
                    union value               &res)
{
    res.boolean =
	(Trans::lookup(argval[0].string) & Trans::T_INTRANS) != 0;
}

static void trans(MethodOrFunctionEntry     *w,
                  const Expr                *args,
                  union value                argval[],
                  union value               &res)
{
    res.boolean =
	(Trans::lookup(argval[0].string) & Trans::T_TRANS) != 0;
}

static void bitrans(MethodOrFunctionEntry     *w,
                    const Expr                *args,
                    union value                argval[],
                    union value               &res)
{
    res.boolean =
	(Trans::lookup(argval[0].string) & Trans::T_BITRANS) != 0;
}

static void refl_intrans(MethodOrFunctionEntry     *w,
                         const Expr                *args,
                         union value                argval[],
                         union value               &res)
{
    res.boolean =
	(Trans::lookup(argval[0].string) & Trans::T_REFL_INTRANS) != 0;
}

static void refl_trans(MethodOrFunctionEntry     *w,
                       const Expr                *args,
                       union value                argval[],
                       union value               &res)
{
    res.boolean =
	(Trans::lookup(argval[0].string) & Trans::T_REFL_TRANS) != 0;
}

static void refl_bitrans(MethodOrFunctionEntry     *w,
                         const Expr                *args,
                         union value                argval[],
                         union value               &res)
{
    res.boolean =
	(Trans::lookup(argval[0].string) & Trans::T_REFL_BITRANS) != 0;
}

#endif // TRANSITIVITY



static int CompareEntries(const IdEntry &e1, const IdEntry &e2) {
  return strcmp(e1.Name(), e2.Name());
}
static uint KeyIdEntry(const IdEntry &e) {
  return Hash(e.Name());
}

static void InitTables(void) {
  InitSemantics();
  IdEntry::globalsTable = new HashTable<IdEntry>(KeyIdEntry, CompareEntries, NULL, NULL);
}

void DefinePredefined(void) {
  MethodOrFunctionEntry *pm;
  if (1 == 2) yyunput(0, NULL); /* just to avoid warning of unused function */
  endlabel = IdEntry::LookUp("endlabel");
  endlabel->IntoGlobalTable(IdEntry::LabelId);
  endlabel->u.ruleNo = MAX_RULES;
  IdEntry *beginlabel = IdEntry::LookUp("beginlabel");
  beginlabel->IntoGlobalTable(IdEntry::LabelId);
  beginlabel->u.ruleNo = 0;
  constantAlmostOne = CreateConstant("0.9999", Real); 
  constantAlmostOne->c.real = 0.9999;
  constantAlmostZero = CreateConstant("0.0001", Real); 
  constantAlmostZero->c.real = 0.0001;
  constantFalse = CreateConstant("false", Boolean);
  constantFalse->c.boolean = false;
  constantTrue = CreateConstant("true", Boolean);
  constantTrue->c.boolean = true;
  CreateConstant("red", Integer)->c.integer = 1;
  CreateConstant("blue", Integer)->c.integer = 2;
  CreateConstant("green", Integer)->c.integer = 3;
  CreateConstant("yellow", Integer)->c.integer = 4;
  //  CreateFeature("undef", 0, FEATURE_UNDEF);
  for (int i = 0; i < scrutinizer->Tags().NFeatureClasses(); i++) {
    const FeatureClass &fc = scrutinizer->Tags().GetFeatureClass(i);
    // printf("Ny särdragsklass %d: %s\n", i, fc.Name());
    CreateFeatureClass(fc.Name(), i);
    for (int j = 0; j < fc.NFeatures(); j++) {
      int f = fc.GetFeature(j);
      // printf("Nytt särdrag %d \n", f);
      // printf("med namnet %s\n", scrutinizer->Tags().GetFeature(f).Name());
      CreateFeature(scrutinizer->Tags().GetFeature(f).Name(), i, f);
    }
  }
  for (int k=0; k < N_TOKENS; k++)
    CreateConstant(Token2String((Token)k), Integer)->c.integer = k;
  constantSpellOK = new IdEntry("spellOK", IdEntry::AttributeId, Boolean);
  constantBeginOK = new IdEntry("beginOK", IdEntry::AttributeId, Boolean);
  constantEndOK = new IdEntry("endOK", IdEntry::AttributeId, Boolean);
  constantIsRepeated = new IdEntry("is_repeated", IdEntry::AttributeId, Boolean);
  constantToken = new IdEntry("token", IdEntry::AttributeId, Integer);
  constantText = new IdEntry("text", IdEntry::AttributeId, String);
  constantRealText = new IdEntry("real_text", IdEntry::AttributeId, String);
  constantVerbtype = new IdEntry("verbtype", IdEntry::AttributeId, Integer);
  constantLemma = new IdEntry("lemma", IdEntry::AttributeId, String);
  constantNoOfTokens = new IdEntry("no_of_tokens", IdEntry::AttributeId, Integer);
  constantLength = new IdEntry("length", IdEntry::AttributeId, Integer);
  constantGetReplacement = new IdEntry("get_replacement", IdEntry::AttributeId, String);
  constantGetValues = new IdEntry("get_values", IdEntry::AttributeId, Boolean);
  constantLex = new IdEntry("lex", IdEntry::AttributeId, LexVariable);
  constantCap = new IdEntry("is_cap", IdEntry::AttributeId, Boolean);
  constantAllCap = new IdEntry("is_all_cap", IdEntry::AttributeId, Boolean);
  constantManyCap = new IdEntry("is_many_cap", IdEntry::AttributeId, Boolean);
  constantHyphen = new IdEntry("is_hyphen", IdEntry::AttributeId, Boolean);
  constantIsForeign = new IdEntry("is_foreign", IdEntry::AttributeId, Boolean);
  pm = new MethodOrFunctionEntry("substr", IdEntry::MethodId, String, 2);
  pm->func.method = &subfunc;
  pm->argtypes[0] = pm->argtypes[1] = Integer;
  pm = new MethodOrFunctionEntry("color", IdEntry::MethodId, String, 1);
  pm->func.method = &colorfunc;
  pm->argtypes[0] = Integer;
  pm = new MethodOrFunctionEntry("form", IdEntry::MethodId, 
       CorrT, MethodOrFunctionEntry::variablenoofargs);
  pm->func.method = &formFunc;
  pm = new MethodOrFunctionEntry("concat", IdEntry::FunctionId, String, 2);
  pm->func.function = &concatfunc;
  pm->argtypes[0] = pm->argtypes[1] = String;
  pm = new MethodOrFunctionEntry("smart_concat", IdEntry::FunctionId, String, 2);
  pm->func.function = &smart_concatfunc;
  pm->argtypes[0] = pm->argtypes[1] = String;
  pm = new MethodOrFunctionEntry("tolower", IdEntry::FunctionId, String, 1);
  pm->func.function = &tolowerfunc;
  pm->argtypes[0] = String;
  pm = new MethodOrFunctionEntry("toupper", IdEntry::FunctionId, String, 1);
  pm->func.function = &toupperfunc;
  pm->argtypes[0] = String;
  pm = new MethodOrFunctionEntry("firsttoupper", IdEntry::FunctionId, String, 1);
  pm->func.function = &firsttoupperfunc;
  pm->argtypes[0] = String;
  pm = new MethodOrFunctionEntry("italics", IdEntry::FunctionId, String, 1);
  pm->func.function = &italicfunc;
  pm->argtypes[0] = String;
  pm = new MethodOrFunctionEntry("bold", IdEntry::FunctionId, String, 1);
  pm->func.function = &boldfunc;
  pm->argtypes[0] = String;
  pm = new MethodOrFunctionEntry("tostring", IdEntry::FunctionId, String, 1);
  pm->func.function = &tostringfunc;
  pm->argtypes[0] = AnyType;
  pm = new MethodOrFunctionEntry("spell_OK", IdEntry::FunctionId, Boolean, 2);
  pm->func.function = &correctlyspelledfunc;
  pm->argtypes[0] = String;
  pm->argtypes[1] = Integer;
  pm = new MethodOrFunctionEntry("spell_corr", IdEntry::MethodId, CorrT, 2);
  pm->func.method = &spellCorrectFunc;
  pm->argtypes[0] = String;
  pm->argtypes[1] = Integer;
  pm = new MethodOrFunctionEntry("spell_OK_sound", IdEntry::FunctionId, Boolean, 2);
  pm->func.function = &correctlyspelledsoundfunc;
  pm->argtypes[0] = String;
  pm->argtypes[1] = Integer;
  pm = new MethodOrFunctionEntry("spell_corr_compound", IdEntry::MethodId, CorrT, 2);
  pm->func.method = &spellCorrectCompoundFunc;
  pm->argtypes[0] = String;
  pm->argtypes[1] = Integer;
  pm = new MethodOrFunctionEntry("spell_compound_OK", IdEntry::MethodId, Boolean, 2);
  pm->func.method = &spellcompoundcheckfunc;
  pm->argtypes[0] = String;
  pm->argtypes[1] = Integer;
  pm = new MethodOrFunctionEntry("spell_info", IdEntry::MethodId, String, 1);
  pm->func.method = &spellinfofunc;
  pm->argtypes[0] = Integer;
  pm = new MethodOrFunctionEntry("strange_sentence", IdEntry::FunctionId, Boolean, 0);
  pm->func.function = &strangesentencefunc;
  pm = new MethodOrFunctionEntry("contains_repeated_words", IdEntry::FunctionId, Boolean, 0);
  pm->func.function = &containsrepeatedwordsfunc;
  pm = new MethodOrFunctionEntry("contains_style_word", IdEntry::FunctionId, Boolean, 0);
  pm->func.function = &containsStyleWordFunc;
  pm = new MethodOrFunctionEntry("join", IdEntry::MethodId, CorrT, 1);
  pm->func.method = &joinFunc;
  pm->argtypes[0] = String;
  pm = new MethodOrFunctionEntry("replace", IdEntry::MethodId, CorrT, 1);
  pm->func.method = &replaceFunc;
  pm->argtypes[0] = String;
  pm = new MethodOrFunctionEntry("insert", IdEntry::MethodId, CorrT, 1);
  pm->func.method = &insertFunc;
  pm->argtypes[0] = String;
  pm = new MethodOrFunctionEntry("delete", IdEntry::MethodId, CorrT, 0);
  pm->func.method = &deleteFunc;
  pm = new MethodOrFunctionEntry("donothing", IdEntry::MethodId, CorrT, 0);
  pm->func.method = &doNothingFunc;



  pm = new MethodOrFunctionEntry("token_start", IdEntry::MethodId, Integer, 0);
  pm->func.method = &token_start;

#ifdef PROBCHECK
  pm = new MethodOrFunctionEntry("probcheck", IdEntry::MethodId, Integer, 0);
  pm->func.method = &prob_check;
  pm = new MethodOrFunctionEntry("lastproberr", IdEntry::FunctionId, Integer, 0);
  pm->func.function = &last_prob_err;
#endif // PROBCHECK

#ifdef TRANSITIVITY
  pm = new MethodOrFunctionEntry("intrans", IdEntry::FunctionId, Boolean, 1);
  pm->func.function = &intrans;
  pm->argtypes[0] = String;
  pm = new MethodOrFunctionEntry("trans", IdEntry::FunctionId, Boolean, 1);
  pm->func.function = &trans;
  pm->argtypes[0] = String;
  pm = new MethodOrFunctionEntry("bitrans", IdEntry::FunctionId, Boolean, 1);
  pm->func.function = &bitrans;
  pm->argtypes[0] = String;
  pm = new MethodOrFunctionEntry("intrans_refl", IdEntry::FunctionId, Boolean, 1);
  pm->func.function = &refl_intrans;
  pm->argtypes[0] = String;
  pm = new MethodOrFunctionEntry("trans_refl", IdEntry::FunctionId, Boolean, 1);
  pm->func.function = &refl_trans;
  pm->argtypes[0] = String;
  pm = new MethodOrFunctionEntry("bitrans_refl", IdEntry::FunctionId, Boolean, 1);
  pm->func.function = &refl_bitrans;
  pm->argtypes[0] = String;
#endif // TRANSITIVITY
}

/* Memory handling */

static Element *Ebuf = NULL;
static int EfirstUnused = 0;

Element *NewElements(Element *p, int n) {
  if (n == 0) return NULL;
  if (Ebuf == NULL || EfirstUnused + n > NEW_ELEMENTS_BUF_SIZE) {
    Message(MSG_STATUS, "allocating Elements...");
    if (Ebuf)
      Message(MSG_WARNING, "allocating Elements, increase NEW_ELEMENTS_BUF_SIZE");
    int size = (n > NEW_ELEMENTS_BUF_SIZE) ? n : NEW_ELEMENTS_BUF_SIZE;
    Ebuf = new Element[size];
    if (!Ebuf) return NULL; // No memory left!
    EfirstUnused = 0;
  }
  Element *start = Ebuf + EfirstUnused;
  memcpy(start, p, n * sizeof(Element));
  EfirstUnused += n;
  return start;
}

void DeleteElements() {
  delete[] Ebuf;
  Ebuf = NULL;   // old Ebufs not deleted
}

RuleSet *ReadRules(Scrutinizer *s, const char *ruleFile) {
  if (!s->IsLoaded()) {
    Message(MSG_ERROR, "trying to load rules with unloaded scrutinizer");
    return NULL;
  }
  scrutinizer = s;
  InitTables();
  Message(MSG_STATUS, "rule tables initialized");
  DefinePredefined();
  Message(MSG_STATUS, "rules initialized");
  yyin = fopen(ruleFile, "r");
  if(!yyin)		// jbfix: non-existing file caused infinite loop
  {
    Message(MSG_ERROR, "rules file", ruleFile, "could not be found");
    return NULL;
  }
  yyparse();
  Message(MSG_STATUS, "loading", ruleFile, "...");
  if (errorLine)
    ParseError("Syntaxfel i regeldefinitionerna");
  if (!noCodeGeneration && !IdEntry::RulesOK())
    noCodeGeneration = 1;
  if (noCodeGeneration) {
    Message(MSG_ERROR, "no code generated");
    return NULL;
  }

  ruleSet.ComputeScope();
  Message(MSG_STATUS, "rules read");
  return &ruleSet;
}


