/* eval.cc
 * author: Viggo Kann (Johan Carlberger)
 * last Johan change: 2000-04-25
 * last Viggo change: 1999-12-14
 * comments:
 */

#include "re.h"
#include <vector>

static bool evalEverything = 0; /* Eval will not shortcircuit anything if =1 */

Matching *Expr::matching = NULL;
int Expr::elementIndex = 0;
int Expr::tokenIndex = 0;
enum Expr::EvalEnvironment Expr::evalEnvironment = Expr::NoEnvironment;
int *Expr::occurrences;

semantictype matrix[NSemanticTypes][NSemanticTypes];

void InitSemantics() {
  for (int i=0; i<NSemanticTypes; i++)
    for (int j=0; j<NSemanticTypes; j++)
      matrix[i][j] = (i == j) ? (enum semantictype) i : SemanticError;
}

int Expr::Eval(union value &res, Matching *m, int ei, int ti, enum Expr::EvalEnvironment env) const {
  Timer timer;
  if (xTakeTime) timer.Start();
  Matching *oldMatching = matching;
  int oldElementIndex = elementIndex;
  int oldTokenIndex = tokenIndex;
  enum Expr::EvalEnvironment oldEvalEnvironment = evalEnvironment;

  matching = m;
  elementIndex = ei;
  tokenIndex = ti;
  evalEnvironment = env;

  int e = Eval(res);

  matching = oldMatching;
  elementIndex = oldElementIndex;
  tokenIndex = oldTokenIndex;
  evalEnvironment = oldEvalEnvironment;
  if (xTakeTime) evalTime += timer.Get();
  return e;
}

void Expr::CorrectionEval(union value val) const {
  //  std::cout << "correctionEval" << std::endl;
  if (semtype == Interval) return;
  Matching *m = CurrentMatching();
  DynamicSentence *ds = m->GetAltSentence();
  if (!ds) {
    Message(MSG_WARNING, "matching has no alt-sentence");
    return;
  }
  if (semtype != CorrT) {
    Message(MSG_WARNING, "CorrctionEval(), bad semtype", int2str(semtype));
    return;
  }  
  CorrThing *next;
  for (CorrThing *ct = val.corrThing; ct; ct = next) {
    //    std::cout << ct << std::endl;
    int pos = 0;
    for (int i = 2; i<m->GetSentence()->NTokens()-1; i++)
      if (ct->GetWordToken() == m->GetSentence()->GetWordToken(i)) {
	pos = i; break;
      }
    if (pos == 0) {
      Message(MSG_WARNING, "CorrctionEval(), token not found");
      return;
    }
    switch(ct->Operation()) {
    case CORR_DO_NOTHING:
      break;
    case CORR_JOIN: {
      char s[MAX_WORD_LENGTH];
      const char *s1 = ct->GetWordToken()->RealString();
      const char *s2 = ct->String(0);
      const int len0 = strlen(ct->GetWordToken()->RealString());
      if (len0 > 1 && IsConsonant(s2[0]) &&
	  s1[len0-1] == s2[0] &&
	  s1[len0-1] == s1[len0-2])
	sprintf(s, "%s%s", s1, s2+1);
      else
	sprintf(s, "%s%s", s1, s2);
      for (DynamicSentence *d = ds; d; d=d->Next())
	if (!d->Replace(scrutinizer->FindMainOrNewWordAndAddIfNotPresent(s), s, pos))
	  Message(MSG_WARNING, "CorrctionEval(), replace failed");
      pos ++;
    }
    case CORR_DELETE: {
      for (DynamicSentence *d = ds; d; d=d->Next())
	if (!d->Delete(pos))
	  Message(MSG_WARNING, "CorrctionEval(), delete failed");
      break;
    }
    case CORR_REPLACE: {
      ct->ds[0] = ds;
      int i;
      for (i=1; i<ct->NStrings(); i++)
	ct->ds[i] = m->DuplicateAltSentences();
      for (i=0; i<ct->NStrings(); i++) {

	// if the replacement string looks like "/.../" we dig out the
	// original text and the original regular expression, then
	// look for references on the form "\1" etc. in the
	// replacement string and match these to the regular
	// expression matching subgroups to build a correction string
	const char *temp = ct->String(i);
	bool regexReplace = 0;
	if(temp[0] == '/') {
	  int l = strlen(temp);
	  if(temp[l - 1] == '/') {
	    if(m->GetElementMatching(0).GetElement()->expr->type == Expr::Operation
	       && m->GetElementMatching(0).GetElement()->expr->c.op.Right()->semtype == Regexp) {
	      
	      regexReplace = 1;	      
	      
	      std::string repl = "";
	      std::vector<std::string> groups;
	      bool haveGroups = false;
	      
	      for(int ii = 1; ii < l - 1; ii++) {

		if(temp[ii] == '\\' && isdigit(temp[ii + 1])) {
		  unsigned int nn = 0, jj = 0;
		  for(jj = ii+1; isdigit(temp[jj]); jj++) {
		    nn = nn*10 + (temp[jj] - '0');
		  }
		  ii = jj - 1;

		  if(!haveGroups) {
		    const char *orgStr = ct->GetWordToken()->RealString();
		    const char *reStr = (m->GetElementMatching(0).GetElement()->expr->c.op.Right()->c).regexp.regexp;
		    haveGroups = true;
		    groups = regexGroups(orgStr, reStr);
		  }

		  if(nn < groups.size()) {
		    repl += groups[nn];
		  } else {
		    repl += "[okänd regexpgrupp]";
		  }
		} else {
		  repl += temp[ii];
		}
	      }
	    
	      for (DynamicSentence *d = ct->ds[i]; d; d=d->Next())
		if (!d->Replace(ct->GetWord(i), repl.c_str(), pos))
		  Message(MSG_WARNING, "CorrctionEval(), replace failed");
	    }
	  }
	}
	
	if(!regexReplace) {
	  for (DynamicSentence *d = ct->ds[i]; d; d=d->Next())
	    if (!d->Replace(ct->GetWord(i), ct->String(i), pos))
	      Message(MSG_WARNING, "CorrctionEval(), replace failed");
	}
      }
      for (i=1; i<ct->NStrings(); i++)
	m->AddAltSentences(ct->ds[i]);
      break;
    }
    case CORR_INSERT: {
      for (int i=0; i<ct->NStrings(); i++) {
	for (DynamicSentence *d = ds; d; d=d->Next())
	  if (!d->Insert2(pos, ct->GetWord(i), ct->String(i)))
	    Message(MSG_WARNING, "CorrctionEval(), insert failed");
      }
      break;
    }
    }
    next = ct->Next();
    delete(ct);
  }
  return;
}

/* Eval computes the value of the expression and stores the result in the
   right place in the union res. Before eval is called the following variables
   have to be set: Matching::currentElements, 
   Matching::currentNoOfElement, Matching::currentOccurrence */

int Expr::Eval(union value &res) const {
  union value leftval, rightval;
  Expr *tmp;
  int occ;
  switch (type) {
  case Operation:
    // xCase[0]++;
    if (c.op.IsUnary()) {
      if (!c.op.Left()) {
	if (c.op.Op() == HELPIDENTSYM)
	  return 1;
	Message(MSG_WARNING, "Eval: operation without argument");
	return 0;
      }
      if (!c.op.Left()->Eval(leftval)) return 0;
      switch (c.op.Op()) {
      case HELPIDENTSYM:
	// xCase[0]++;
	return 1;        // already returned above
      case ',':
	//xCase[1]++;
	return 1;
      case '!':
	//xCase[2]++;
	if (semtype == Boolean) { res.boolean = !leftval.boolean; return 1; }
	else Message(MSG_WARNING, "Eval: Okänd typ:", int2str(semtype));
	break;
      case ' ':
	//xCase[3]++;
	if (evalEnvironment == CorrectionEnvironment)
	  c.op.Left()->CorrectionEval(leftval);
	else if (evalEnvironment == MarkEnvironment)
	  leftval.evalVar.Mark();
	res.string = "";
	return 1;
      case TAGIDENTSYM:
	//xCase[4]++;
	return 1;
      case NEG:
	//xCase[5]++;
	if (semtype == Integer) { res.integer = -leftval.integer; return 1; }
	else if (semtype == Real) { res.real = -leftval.real; return 1; }
	else Message(MSG_WARNING, "Eval: Okänd typ:", int2str(semtype));
	break;
      default: Message(MSG_WARNING, "Okänd operator"); // << c.op << std::endl;
      }
      return 0;
    } else { // i.e. c.op is binary
      if (c.op.Op() == ASSIGNSYM) {
	if (evalEnvironment == CorrectionEnvironment &&
	    c.op.Left()->type == Expr::Attribute) // Assignment in correction
	  return c.op.Right()->Eval(res);
      } else {
	if (!c.op.Left()->Eval(leftval)) return 0;
      }
      switch (c.op.Op()) {
      case '&':
	//xCase[0]++;
	if (!leftval.boolean) {
	  res.boolean = 0; 
	  if (evalEverything) c.op.Right()->Eval(rightval);
	  return 1;
	}
	if (!c.op.Right()->Eval(rightval)) return 0;
	res.boolean = rightval.boolean; return 1;
      case '|':
	//xCase[1]++;
	if (leftval.boolean) {
	  res.boolean = 1;
	  if (evalEverything) c.op.Right()->Eval(rightval);
	  return 1;
	}
	if (!c.op.Right()->Eval(rightval)) return 0;
	res.boolean = rightval.boolean; return 1;
      case '.': {
	//xCase[2]++;
	EvaluatedVariable *obj = NULL;
	Expr *arg = c.op.Right(); /* method or attribute */
	enum semantictype leftsemtype = c.op.Left()->semtype;
	if (leftsemtype == Variable || leftsemtype == LexVariable)
	  obj = &leftval.evalVar;
	if (arg->type == Method) {
	  union value argval[MAXNOOFARGUMENTS];
	  Expr *argp;
	  int i;
	  for (i = 0, argp = arg->c.method.actuals; argp;
	       i++, argp = argp->c.op.Right())
	    if (!argp->c.op.Left()->Eval(argval[i])) return 0;
	  MethodOrFunctionEntry *mp =
	    (MethodOrFunctionEntry *) arg->c.method.id;
	  (*mp->func.method)(mp, obj, (obj ? NULL : leftval.string),
			     arg->c.method.actuals, argval, res);
	  return 1;
	} 
	/* Attribute: */
	if (!obj) { /* text */
	  if (arg->c.id == constantLength) {
	    res.integer = strlen(leftval.string); return 1; }
	  Message(MSG_WARNING, "Eval: Otillåtet attribut för textobjekt");
	  return 0;
	}
	switch (arg->semtype) { // arg not evaluated
	case SemFeatureClass:
	  //	    xCase[0]++;
	  if (!obj->IsLexical())
	  {
	      if(!obj->GetTag())
		res.feature = UNDEF;
	      else
	        res.feature = obj->GetTag()->FeatureValue(arg->c.id->FeatureClass());
	  } else switch (Lex::mode) {
	  case Lex::NotInLexMode: Message(MSG_WARNING, "Eval: Användning av lex på otillåten plats."); return 0;
	  case Lex::InitLexMode:  Lex::AddToken(obj->GetWordToken()); 

	    res.feature = 0;
	    break;
	  case Lex::EvalLex: 
	    res.feature = 
	      obj->GetWordToken()->CurrentInterpretation()->GetTag()->FeatureValue(arg->c.id->FeatureClass()); 

	    break;
	  }
	  return 1;
	case Integer:
	  //	    xCase[1]++;
	  if (arg->c.id == constantLength)
	    { res.integer = strlen(obj->GetWordToken()->LexString()); return 1; }
	  else if (arg->c.id == constantNoOfTokens)
	    { res.integer = obj->GetElementMatching()->Occurrences(); return 1; }
	  else if (arg->c.id == constantToken)
	    { res.integer = obj->GetWordToken()->GetToken(); return 1; }
	  else if (arg->c.id == constantVerbtype)
	    { res.integer = obj->GetWordToken()->GetWordTag()->VerbType(); return 1; }
	  else break;
	case String:
	  //	    xCase[2]++;
	  if (arg->c.id == constantText) {
	    res.string = obj->LexString();
	    return 1;
	  } else if (arg->c.id == constantRealText) {
	    res.string = obj->RealString();		
	    return 1;
	  } else if (arg->c.id == constantLemma) {
	    if (obj->GetWordToken()->GetWordTag()->Lemma(0))
	      res.string = obj->GetWordToken()->GetWordTag()->Lemma(0)->String();
	    else
	      res.string = "";
	    // if (xVerbose) std::cout << xCurrentRule << " lemma of " << obj->GetWordToken()->GetWordTag() << " is "
	    //		   << obj->GetWordToken()->GetWordTag()->Lemma(0) << std::endl;
	    return 1;
	  } else if (arg->c.id == constantGetReplacement) {
	    const Word *w = obj->GetWordToken()->GetWord();
	    // std::cerr << std::endl << w << " style? " << std::endl;
	    if (w->HasStyleRemark()) {
	      //std::cerr << "style" << std::endl;
	      const StyleWord *sw = scrutinizer->Words().GetStyleWord(w);
	      if (sw && sw->Alternative(0)) {
		  res.string = sw->Alternative(0)->String();
		  return 1; }
	    }
	    res.string = obj->GetWordToken()->LexString();// w->String(); // no replacement, word unchanged
	    return 1;
	  }
	  else break;
	case Boolean:
	  ensure(0); // never seem to happen
	  //	    xCase[3]++;
	  if (arg->c.id == constantCap) {
	    res.boolean = obj->GetWordToken()->IsFirstCapped(); return 1; }
	  else if (arg->c.id == constantAllCap) {
	    res.boolean = obj->GetWordToken()->IsAllCapped(); return 1; }
	  else if (arg->c.id == constantManyCap) {
	    res.boolean = obj->GetWordToken()->IsManyCapped(); return 1; }
	  else if (arg->c.id == constantHyphen) {
	    res.boolean = obj->GetWordToken()->IsHyphened(); return 1; }
	  else if (arg->c.id == constantSpellOK) {
	    res.boolean = obj->GetWordToken()->IsSpellOK(); return 1; }
	  else if (arg->c.id == constantBeginOK) {
	    res.boolean = obj->GetWordToken()->IsBeginOK(); return 1; }
	  else if (arg->c.id == constantEndOK) {
	    res.boolean = obj->GetWordToken()->IsEndOK(); return 1; }
	  else if (arg->c.id == constantIsForeign) {
	    res.boolean = obj->GetWordToken()->GetWord()->IsForeign(); return 1; }
	  else if (arg->c.id == constantIsRepeated) {
	    res.boolean = obj->GetWordToken()->IsRepeated(); return 1; }
	  break;
	case Variable:
	case LexVariable:
	  ensure(0); // never seem to happen
	  //	    xCase[4]++;
	  if (arg->c.id == constantLex) {
	    obj->lex = true;
	    res.evalVar = *obj;

	    return 1; 
	  } else break;
	default: break;
	}
	Message(MSG_WARNING, "Eval: Okänt attribut");
	return 0;
      }
      case PROBSYM:
	//xCase[3]++;
	res.boolean = (Lex::EvalExpr(c.op.Right()) >= leftval.real);
	return 1;
      case IFSYM:
	//xCase[4]++;	
	if (leftval.boolean) {
	  tmp = c.op.Right()->c.op.Left();
	  if (evalEverything) c.op.Right()->c.op.Right()->Eval(rightval);
	} else {
	  tmp = c.op.Right()->c.op.Right();
	  if (evalEverything) c.op.Right()->c.op.Left()->Eval(rightval);
	}
	if (tmp->Eval(res)) return 1;
	return 0;
      }
      if (!c.op.Right()->Eval(rightval)) return 0;
      switch (c.op.Op()) {
      case '=':
	// xCase[0]++;
	switch (c.op.Left()->semtype) {
	case Feature:
	case SemFeatureClass: // xCase[0]++;
	  res.boolean = /* leftval.feature == rightval.feature; */ scrutinizer->Tags().IsCompatible(leftval.feature, rightval.feature); return 1;
	case String:  // xCase[1]++;
	  res.boolean = (strcmp(leftval.string, rightval.string) == 0); return 1;
	case Integer:  //xCase[2]++;
	  res.boolean = (leftval.integer == (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	case Variable: // xCase[3]++;
	  res.boolean = (&leftval.evalVar == &rightval.evalVar); return 1;
	case Boolean: // xCase[4]++;
	  res.boolean = (leftval.boolean == rightval.boolean); return 1;
	case Real: // xCase[5]++;
	  res.boolean = (leftval.real == (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	default: Message(MSG_WARNING, "Eval: Okänd typ i =: ", int2str(c.op.Left()->semtype)); return 0;
	}
	ensure(0);
      case NESYM:
	// xCase[1]++;
	switch (c.op.Left()->semtype) {
	case Feature:
	case SemFeatureClass: // xCase[0]++;
	  res.boolean = /* leftval.feature != rightval.feature; */ !scrutinizer->Tags().IsCompatible(leftval.feature, rightval.feature); return 1;
	case String: // xCase[1]++;
	  res.boolean = (strcmp(leftval.string, rightval.string) != 0); return 1;
	case Boolean: // xCase[2]++;
	  res.boolean = (leftval.boolean != rightval.boolean); return 1;
	case Integer: // xCase[3]++;
	  res.boolean = (leftval.integer != (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	case Real: // xCase[4]++;
	  res.boolean = (leftval.real != (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	case Variable:  // xCase[5]++;
	  res.boolean = (&leftval.evalVar != &rightval.evalVar); return 1;
	default: Message(MSG_WARNING, "Eval: Okänd typ i !=:", int2str(c.op.Left()->semtype)); return 0;
	}
	ensure(0);
      case ASSIGNSYM: {
	// xCase[2]++;
	const bool onlyattribute = (c.op.Left()->type == Expr::Attribute);
	const Expr *attr = onlyattribute ? c.op.Left() : c.op.Left()->c.op.Right();
	if (onlyattribute) {
	  if (evalEnvironment != HelpruleEnvironment) {
	    Message(MSG_WARNING, "Förbjuden tilldelning i hjälpregel");
	    return 0;
	  }
	} else if (evalEnvironment != ActionEnvironment) {
	  Message(MSG_WARNING, "Förbjuden tilldelning vid omtaggning");
	  std::cout << c.op.Left() << std::endl; //Viggotest
	  return 0;
	}
	if (attr->semtype != SemFeatureClass) {
	  Message(MSG_WARNING, "cannot do assignments fully yet");
	  return 0;
	}
	Tag *tag;
	if (onlyattribute) {
	  ElementMatching &m = matching->GetElementMatching(elementIndex);
	  tag = m.GetTag();
	} else {
	  if (!c.op.Left()->c.op.Left()->Eval(leftval))
	    return 0;
	  tag = leftval.evalVar.GetTag();
	}
	int fClass = attr->GetFeatureClass();
	int fValue = rightval.feature;
	tag->SetFeature(fClass, fValue);
	return 1;
      }
      case ',':
	// xCase[3]++;
	return 1;
      case '+':
	// xCase[4]++;
	switch (semtype) {
	case Integer: res.integer = leftval.integer + rightval.integer; return 1;
	case Real:    res.real = (c.op.Left()->semtype == Integer ? leftval.integer : leftval.real) +
			(c.op.Right()->semtype == Integer ? rightval.integer : rightval.real); return 1;
	default: Message(MSG_WARNING, "Eval: Okänd typ i +:", int2str(semtype)); return 0;
	}
	ensure(0);
      case '-':
	// xCase[5]++;
	switch (semtype) {
	case Integer: res.integer = leftval.integer - rightval.integer; return 1;
	case Real:    res.real = (c.op.Left()->semtype == Integer ? leftval.integer : leftval.real) -
			(c.op.Right()->semtype == Integer ? rightval.integer : rightval.real); return 1;
	default: Message(MSG_WARNING, "Eval: Okänd typ i -:", int2str(semtype)); return 0;
	}
	ensure(0);
      case '>':
	// xCase[6]++;
	switch (c.op.Left()->semtype) {
	case Integer: res.boolean = (leftval.integer > (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	case Real:    res.boolean = (leftval.real > (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	default: Message(MSG_WARNING, "Eval: Okänd typ i >:", int2str(c.op.Left()->semtype)); return 0;
	}
	ensure(0);
      case ' ':
	// xCase[7]++;
	if (Expr::evalEnvironment == CorrectionEnvironment) {
	  c.op.Right()->CorrectionEval(rightval);
	  c.op.Left()->CorrectionEval(leftval);
	  res.string = "";
	} else if (Expr::evalEnvironment == MarkEnvironment) {
	  if (c.op.Left()->semtype == Variable)
	    leftval.evalVar.Mark();
	  if (c.op.Right()->semtype == Variable)
	    rightval.evalVar.Mark();
	  res.string = "";
	} else {
	  const char *s1 = c.op.Left()->semtype != Variable ?
	    leftval.string : leftval.evalVar.RealString();
	  const char *s2 = c.op.Right()->semtype != Variable ?
	    rightval.string : rightval.evalVar.RealString();
	  char *s = infoStringBuf.NewString(strlen(s1)+strlen(s2)+2);
	  sprintf(s, "%s %s", s1, s2);
	  res.string = s;
	}
	return 1;
      case LESYM:
	// xCase[8]++;
	switch (c.op.Left()->semtype) {
	case Integer: res.boolean = (leftval.integer <= (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	case Real:    res.boolean = (leftval.real <= (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	default: Message(MSG_WARNING, "Eval: Okänd typ i <=:", int2str(c.op.Left()->semtype)); return 0;
	}
	ensure(0);
      case GESYM:
	// xCase[9]++;
	switch (c.op.Left()->semtype) {
	case Integer: res.boolean = (leftval.integer >= (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	case Real:    res.boolean = (leftval.real >= (c.op.Right()->semtype ==  Integer ? rightval.integer : rightval.real)); return 1;
	default: Message(MSG_WARNING, "Eval: Okänd typ i >=:", int2str(c.op.Left()->semtype)); return 0;
	}
	ensure(0);
      case '<':
	// xCase[10]++;
	switch (c.op.Left()->semtype) {
	case Integer: res.boolean = (leftval.integer < (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	case Real:    res.boolean = (leftval.real < (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	default: Message(MSG_WARNING, "Eval: Okänd typ i <:", int2str(c.op.Left()->semtype)); return 0;
	}
	ensure(0);
      case '~':
	// xCase[11]++;
	switch (c.op.Left()->semtype) {
	case String:
	  res.boolean = RegexpCheck(leftval.string, rightval, c.op.Right()->semtype); return 1;
	case Variable:
	  res.boolean = RegexpCheck(leftval.evalVar.GetWordToken()->GetWord()->String(), rightval, c.op.Right()->semtype); return 1;
	default: Message(MSG_WARNING, "Eval: Okänd typ i ~:", int2str(c.op.Left()->semtype)); return 0;
	}
	ensure(0);
      case '[':
	// xCase[12]++;
	if (leftval.evalVar.IsIndexed())
	  { Message(MSG_WARNING, "Kan inte indexera redan indexerad variabel"); return 0; }
	occ = rightval.integer;
	if (occ < 0) { Message(MSG_WARNING, "Negativt index vid indexering"); return 0; }
	if (occ >= leftval.evalVar.GetElementMatching()->Occurrences())
	  { Message(MSG_WARNING, "För högt index vid indexering:", int2str(occ)); return 0; }
	res.evalVar = leftval.evalVar;
	res.evalVar.index = occ;
	return 1;  
      case ELSESYM:	
	Message(MSG_WARNING, "Eval: Felaktig operation: ELSE"); return 0;
      default:
	Message(MSG_WARNING, "Eval: Felaktig operation:"); //, c.op.Op());
	return 0;
      }
    }
    break;
  case Leaf:
    // xCase[1]++;
    if (semtype == Variable) {
      res.evalVar.Init(false, -1, &matching->GetElementMatching(c.id->ElementIndex()));
    } else if (semtype == LexVariable) {
      Message(MSG_WARNING, "Eval: Hittade LexVariable-löv! (internt fel, ska inte uppkomma)");
      return 0;
      //	Matching &m = Matching::currentElements[Matching::currentNoOfElement];
      // 	res.evalVar.Init(true, m.Elt()->MultiOcc() ? Matching::currentOccurrence : -1, &m);
      } else res = c;
    return 1;
  case Attribute: {
    // xCase[2]++;
    ElementMatching &m = matching->GetElementMatching(elementIndex);
    const Element *e = m.GetElement();
    if (semtype == LexVariable) { // the attribute lex
      res.evalVar.Init(true, e->MultiOcc() ? tokenIndex : -1, &m);
      return 1;
    }
    if (semtype == SemFeatureClass) {
      const Tag *tag = (e && e->MultiOcc()) ? m.GetWordToken(tokenIndex)->SelectedTag() : m.GetTag();
      res.feature = tag->FeatureValue(c.id->FeatureClass()); 
      return 1;
    }
    const WordToken *t = e->MultiOcc() ? m.GetWordToken(tokenIndex) : m.GetWordToken(0);
    switch (semtype) {
    case String:
      if (c.id == constantText) {
	res.string = t->LexString();
	return 1;
      } else if (c.id == constantRealText) {
	res.string = t->RealString();
	return 1;
      }
      else if (c.id == constantLemma) {
	if (t->GetWordTag() && t->GetWordTag()->Lemma(0))
	  res.string = t->GetWordTag()->Lemma(0)->String();
	else
	  res.string = "";
	// if (xVerbose) std::cout << xCurrentRule << " lemma of " << t->GetWordTag() << " is "
	//		   << t->GetWordTag()->Lemma(0) << std::endl;
	return 1;
      } else break;
    case Integer:
      if (c.id == constantLength)
	{ res.integer = strlen(t->LexString()); return 1; }
      else if (c.id == constantNoOfTokens)
	{ res.integer = tokenIndex; return 1; }
      else if (c.id == constantToken)
	{ res.integer = t->GetToken(); return 1; }
      else if (c.id == constantVerbtype)
	{ res.integer = t->GetWordTag()->VerbType(); return 1; }
      else break;
    case Boolean:
      if (c.id == constantCap)
	{
	  if(scrutinizer->GetMatchingSet().CheckMode()) {
	    res.boolean = t->FirstCappedAgain();
	  } else {
	    res.boolean = t->IsFirstCapped();
	  }
	  return 1;
	}
      if (c.id == constantAllCap)
	{
	  if(scrutinizer->GetMatchingSet().CheckMode()) {
	    res.boolean = t->AllCappedAgain();
	  } else {
	    res.boolean = t->IsAllCapped();
	  }
	  return 1;
	}
      if (c.id == constantManyCap)
	{
	  if(scrutinizer->GetMatchingSet().CheckMode()) {
	    res.boolean = t->ManyCappedAgain();
	  } else {
	    res.boolean = t->IsManyCapped();
	  }
	  return 1;
	}
      if (c.id == constantHyphen) 
	{ res.boolean = t->IsHyphened(); return 1; }	
      if (c.id == constantSpellOK)
	{ res.boolean = t->IsSpellOK(); return 1; }
      if (c.id == constantBeginOK) {
	res.boolean = t->IsBeginOK(); return 1; }
      if (c.id == constantEndOK) {
	res.boolean = t->IsEndOK(); return 1; }
      if (c.id == constantIsForeign)
	{ res.boolean = t->GetWord()->IsForeign(); return 1; }
      if (c.id == constantIsRepeated)
	{ res.boolean = t->IsRepeated(); return 1; }
      break;
    default: break;
    }
    Message(MSG_WARNING, "Eval: Okänt attribut", c.id->Name());
    return 0;
  }
  case Function:
  case Method: {
    // xCase[3]++;
    union value argval[MAXNOOFARGUMENTS];
    Expr *argp;
    int i;
    for (i = 0, argp = c.method.actuals; argp;
	 i++, argp = argp->c.op.Right())
      if (!argp->c.op.Left()->Eval(argval[i])) return 0;
    MethodOrFunctionEntry *fp = (MethodOrFunctionEntry *) c.method.id;
    if (type == Function)
      (*fp->func.function)(fp, c.method.actuals ? c.method.actuals : this, argval, res);
    else {
      EvaluatedVariable tmp;
      ElementMatching &m = matching->GetElementMatching(elementIndex);
      ensure(m.GetElement()); // this must be an lhs element
      tmp.Init(false, m.GetElement()->MultiOcc() ? tokenIndex : -1, &m);
      (*fp->func.method)(fp, &tmp, NULL, c.method.actuals, argval, res);
    }
    return 1;
  }
  case Constant: // xCase[4]++;
    Message(MSG_WARNING, "Eval: Hittade konstant! (internt fel, ska inte uppkomma)"); return 0;
  default: Message(MSG_WARNING, "Eval: Okänd typ på uttrycksnod");
  }
  return 0;
}

Lex::lexmode Lex::mode; 
int Lex::currentindex; /* current index in lexTokens */
WordToken *Lex::lexTokens[MAXNOOFLEXTOKENS];
double Lex::sumprob;

void Lex::AddToken(WordToken *p) {
  ensure(currentindex >= 0);
  if (currentindex >= MAXNOOFLEXTOKENS) return; /* skip overflow */
  for (int i = 0; i < currentindex; i++)
    if (lexTokens[i] == p) return;
  lexTokens[currentindex++] = p;
}

double Lex::EvalExpr(const Expr *p) {
  mode = InitLexMode;
  union value res;
  bool oldEvalEverything = evalEverything;
  currentindex = 0;
  evalEverything = 1;
  p->Eval(res);
  evalEverything = oldEvalEverything;
  if (currentindex == 0) {
    mode = NotInLexMode;
    Message(MSG_WARNING, "Eval: Ingen lex-variabel ingår i PROB/EXIST/FORALL\n");
    return 1.0;
  }
  mode = EvalLex;
  sumprob = 0.0;
  CheckTokensFrom(p, 0, 1.0);
  mode = NotInLexMode;
  return sumprob;
}

void Lex::CheckTokensFrom(const Expr *p, int start, double prob) {
  float lexprob; /* probability of current interpretation */
  union value res;
  WordToken *t = lexTokens[start];
  for (const WordTag *w = t->FirstInterpretation(); w; w = t->NextInterpretation()) {
    lexprob = float(w->TagFreq()) / t->GetWord()->Freq();

    if (start < currentindex - 1)
      CheckTokensFrom(p, start + 1, prob * lexprob);
    else {
      p->Eval(res);
      if (res.boolean) sumprob += prob * lexprob;
    }
  }
}

/* Optimeringskod: */

int Expr::formerIndex = 0;
int Expr::*occurrences = NULL;
Tag *Expr::formerTag = NULL;
Tag *Expr::currentTag = NULL;
int Expr::anchor = 0;

int Expr::OptEval(bool &unknown, union value &res, int *o, int fi, Tag *ft, 
  int ei, Tag *ct, int anchor_) const {
  int *oldOccurrences = occurrences;
  int oldElementIndex = elementIndex;
  int oldFormerIndex = formerIndex;
  Tag *oldCurrentTag = currentTag;
  Tag *oldFormerTag = formerTag;
  int oldAnchor = anchor;

  occurrences = o;
  elementIndex = ei;
  //  tokenIndex = ti;
  formerIndex = fi;
  currentTag = ct;
  formerTag = ft;
  anchor = anchor_;

  int e = OptEval(unknown, res);

  occurrences = oldOccurrences;
  elementIndex = oldElementIndex;
  //  tokenIndex = oldTokenIndex;
  formerIndex = oldFormerIndex;
  currentTag = oldCurrentTag;
  formerTag = oldFormerTag;
  anchor = oldAnchor;
  return e;
}

/* Eval computes the value of the expression and stores the result in the
   right place in the union res. Before eval is called some variables
   have to be set */

int Expr::OptEval(bool &unknown, union value &res) const {
  union value leftval, rightval;
  Expr *tmp;
  bool rightunknown;
  unknown = false;
  switch (type) {
    case Operation:
      if (c.op.IsUnary()) {
	if (!c.op.Left()) {
	  if (c.op.Op() == HELPIDENTSYM)
	    return 1;
	  std::cerr << "OptEval: operation " << c.op << " utan argument" << std::endl; 
	  return 0;
	}
        if (!c.op.Left()->OptEval(unknown, leftval)) return 0;
	if (unknown) return 1;
        switch (c.op.Op()) {
	case NEG:
	  if (semtype == Integer) { res.integer = -leftval.integer; return 1; }
	  else if (semtype == Real) { res.real = -leftval.real; return 1; }
	  else Message(MSG_WARNING, "OptEval: Okänd typ:", int2str(semtype));
	  break;
	case '!':
	  if (semtype == Boolean) { res.boolean = !leftval.boolean; return 1; }
	  else Message(MSG_WARNING, "OptEval: Okänd typ:", int2str(semtype));
	  break;
	case HELPIDENTSYM:
	  return 1;        // already returned above
	case ',':
	  return 1;
	case TAGIDENTSYM:
	  return 1;
	case ' ':
	  res.string = "";
	  return 1;
	default: std::cerr << "Okänd operator i " << c.op << std::endl;
	}
	return 0;
      } else { // i.e. c.op is binary
        if (!c.op.Left()->OptEval(unknown, leftval)) return 0;
        switch (c.op.Op()) {
	case '&': 
	  if (!unknown && !leftval.boolean) {
	    res.boolean = 0; 
	    return 1;
	  }
	  if (!c.op.Right()->OptEval(rightunknown, rightval)) return 0;
	  if (rightunknown) unknown = true;
	  else if (unknown && !rightval.boolean) unknown = false;
	  res.boolean = rightval.boolean; return 1;
	case '|': 
	  if (!unknown && leftval.boolean) {
	    res.boolean = 1;
	    return 1;
	  }
	  if (!c.op.Right()->OptEval(rightunknown, rightval)) return 0;
	  if (rightunknown) unknown = true;
	  else if (unknown && rightval.boolean) unknown = false;
	  res.boolean = rightval.boolean; return 1;
	case IFSYM:
	  if (unknown) return 1;
	  if (leftval.boolean) {
	    tmp = c.op.Right()->c.op.Left();
	  } else {
	    tmp = c.op.Right()->c.op.Right();
	  }
	  if (tmp->OptEval(unknown, res))
	    return 1;
	  return 0;
	case '.':
	  if (unknown) return 1;
	  { int obj = -1;
	    Expr *arg = c.op.Right(); /* method or attribute */
	    enum semantictype leftsemtype = c.op.Left()->semtype;
	    if (leftsemtype == Variable || leftsemtype == LexVariable)
	      obj = leftval.integer;
	    if (arg->type == Method) {
              unknown = true;
	      return 1;
	    } 
	    /* Attribute: */
	    if (obj == -1) { /* text */
	      if (arg->c.id == constantLength) {
		res.integer = strlen(leftval.string); return 1; }
	      Message(MSG_WARNING, "OptEval: Otillåtet attribut för textobjekt");
	      return 0;
	    }
	    switch (arg->semtype) { // arg not evaluated
	    case SemFeatureClass:
	      if (obj == elementIndex)
		res.feature = currentTag->FeatureValue(arg->c.id->FeatureClass());
	      else if (obj == formerIndex)
                res.feature = formerTag->FeatureValue(arg->c.id->FeatureClass());
	      else unknown = true;
	      return 1;
	    case String:
                unknown = true;
		return 1;
	    case Integer:
	      if (arg->c.id == constantLength
		  || arg->c.id == constantToken)
		{ unknown = true; return 1; }
	      else if (arg->c.id == constantNoOfTokens)
		{ if (obj >= anchor) res.integer = occurrences[obj];
		  else unknown = true;
		  return 1; }
	      else break;
	    case Boolean:
	      if (arg->c.id == constantSpellOK || arg->c.id == constantBeginOK
		  || arg->c.id == constantEndOK) // johan, rätt?
		{ unknown = true; return 1; }
	      break;
	    default: break;
	    }
	    Message(MSG_WARNING, "OptEval: Okänt attribut");
	    return 0;
	  }
	case PROBSYM:	
	  unknown = true;
	  return 1;
	}
        if (unknown) return 1;
        if (!c.op.Right()->OptEval(unknown, rightval)) return 0;
        if (unknown) return 1;
        switch (c.op.Op()) {
	case '[': /* detta skulle kunna göras bättre ibland! /Viggo */
	  unknown = true; return 1;
	case '+':
	  switch (semtype) {
	  case Integer: res.integer = leftval.integer + rightval.integer; return 1;
	  case Real:    res.real = (c.op.Left()->semtype == Integer ? leftval.integer : leftval.real) +
			  (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real); return 1;
	  default: fprintf(stderr, "OptEval: Okänd typ i +: %d\n", semtype); return 0;
	  }
	case '-':
	  switch (semtype) {
	  case Integer: res.integer = leftval.integer - rightval.integer; return 1;
	  case Real:    res.real = (c.op.Left()->semtype == Integer ? leftval.integer : leftval.real) -
			  (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real); return 1;
	  default: fprintf(stderr, "OptEval: Okänd typ i -: %d\n", semtype); return 0;
	  }
	case '=':
	  switch (c.op.Left()->semtype) {
	  case Boolean: res.boolean = (leftval.boolean == rightval.boolean); return 1;
	  case Integer: res.boolean = (leftval.integer == (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	  case Real:    res.boolean = (leftval.real == (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	  case String:  res.boolean = (strcmp(leftval.string, rightval.string) == 0); return 1;
	  case Variable:res.boolean = (leftval.integer == rightval.integer); return 1;
	  case Feature:
	  case SemFeatureClass: res.boolean = /* leftval.feature == rightval.feature; */ scrutinizer->Tags().IsCompatible(leftval.feature, rightval.feature); return 1;
	  default: fprintf(stderr, "OptEval: Okänd typ i =: %d\n", c.op.Left()->semtype); return 0;
	  }
	case '~':
	  switch (c.op.Left()->semtype) {
	  case String:
	    res.boolean = RegexpCheck(leftval.string, rightval, c.op.Right()->semtype); return 1;
	  case Variable:
	    unknown = true; return 1;
	  default: fprintf(stderr, "OptEval: Okänd typ i ~: %d\n", c.op.Left()->semtype); return 0;
	  }
	case NESYM:
	  switch (c.op.Left()->semtype) {
	  case Boolean: res.boolean = (leftval.boolean != rightval.boolean); return 1;
	  case Integer: res.boolean = (leftval.integer != (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	  case Real:    res.boolean = (leftval.real != (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	  case String:  res.boolean = (strcmp(leftval.string, rightval.string) != 0); return 1;
	  case Variable:res.boolean = (leftval.integer != rightval.integer); return 1;
	  case Feature:
	  case SemFeatureClass: res.boolean = /* leftval.feature != rightval.feature; */ !scrutinizer->Tags().IsCompatible(leftval.feature, rightval.feature); return 1;
	  default: fprintf(stderr, "OptEval: Okänd typ i !=: %d\n", c.op.Left()->semtype); return 0;
	  }
	case '<':
	  switch (c.op.Left()->semtype) {
	  case Integer: res.boolean = (leftval.integer < (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	  case Real:    res.boolean = (leftval.real < (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	  default: fprintf(stderr, "OptEval: Okänd typ i <: %d\n", c.op.Left()->semtype); return 0;
	  }
	case '>':
	  switch (c.op.Left()->semtype) {
	  case Integer: res.boolean = (leftval.integer > (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	  case Real:    res.boolean = (leftval.real > (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	  default: fprintf(stderr, "OptEval: Okänd typ i >: %d\n", c.op.Left()->semtype); return 0;
	  }
	case LESYM:
	  switch (c.op.Left()->semtype) {
	  case Integer: res.boolean = (leftval.integer <= (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	  case Real:    res.boolean = (leftval.real <= (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	  default: fprintf(stderr, "OptEval: Okänd typ i <=: %d\n", c.op.Left()->semtype); return 0;
	  }
	case GESYM:
	  switch (c.op.Left()->semtype) {
	  case Integer: res.boolean = (leftval.integer >= (c.op.Right()->semtype == Integer ? rightval.integer : rightval.real)); return 1;
	  case Real:    res.boolean = (leftval.real >= (c.op.Right()->semtype ==  Integer ? rightval.integer : rightval.real)); return 1;
	  default: fprintf(stderr, "OptEval: Okänd typ i: %d\n", c.op.Left()->semtype); return 0;
	  }
	case ' ':
	  unknown = true;
	  return 1;
	case ',':
	  return 1;	  
	case ELSESYM:	
	  fprintf(stderr, "OptEval: Felaktig operation: ELSE\n"); return 0;
	default:
	  fprintf(stderr, "OptEval: Felaktig operation: %c\n", c.op.Op());
	  return 0;
	}
      }
      break;
    case Constant: fprintf(stderr, "OptEval: Hittade konstant! (internt fel, ska inte uppkomma)"); return 0;
    case Leaf:
      if (semtype == Variable)
	res.integer = c.id->ElementIndex();
      else if (semtype == LexVariable) {
	Message(MSG_WARNING, "OptEval: Hittade LexVariable-löv! (internt fel, ska inte uppkomma)");
	return 0;
      } else res = c;
      return 1;
    case Function:
    case Method:
      unknown = true;
      return 1;
  case Attribute: {
    unknown = true;
    if (semtype == LexVariable) { // the attribute lex
      return 1;
    }
    switch (semtype) {
    case SemFeatureClass: res.feature = currentTag->FeatureValue(c.id->FeatureClass()); unknown = false; return 1;
    case String:
      if (c.id == constantText || c.id == constantRealText || c.id == constantLemma) { //johan, rätt?
	return 1;
      } else break;
    case Integer:
      if (c.id == constantLength || c.id == constantToken
	  || c.id == constantVerbtype) // johan, rätt?
	{ return 1; }
      else if (c.id == constantNoOfTokens)
	{ res.integer = occurrences[elementIndex]; return 1; }
      else break;
    case Boolean:
      if (c.id == constantSpellOK || constantBeginOK || constantEndOK || c.id == constantCap
	  || c.id == constantAllCap || c.id == constantManyCap || c.id == constantIsForeign || c.id == constantHyphen
	  || c.id == constantIsRepeated) { return 1; } // johan, rätt? 
      break;
    default: break;
    }
    Message(MSG_WARNING, "OptEval: Okänt attribut:", c.id->Name());
    return 0;
  }
  default: Message(MSG_WARNING, "OptEval: Okänd typ på uttrycksnod");
  }
  return 0;
}

//DefObj(EvaluatedVariable);

const char *EvaluatedVariable::LexString() const {
  if (!IsIndexed() && (GetElement()->MultiOcc() || GetElement()->IsHelpRule())) {
    // this should rarely happen
    int occ = GetElementMatching()->Occurrences();
    if (occ == 0)
      return "";
    int length = occ;
    WordToken **t = GetElementMatching()->GetWordTokensAt();
    for (int i = 0; i < occ; i++)
      if (t[i]->GetWord())
	length += strlen(t[i]->LexString());
      else {
	Message(MSG_WARNING, "evaluatedVariable", int2str(i), int2str(occ), "no word");
	return "";
      }
    char *s = evalStringBuf.NewString(length);
    strcpy(s, t[0]->LexString());
    for (int j = 1; j < occ; j++) {
      strcat(s, " ");
      strcat(s, t[j]->LexString());
    }
    //    std::cout << "LexString: " << s << std::endl;
    return s;
  }
  return GetWordToken()->LexString();
}

const char *EvaluatedVariable::RealString() const {
  if (!IsIndexed() && (GetElement()->MultiOcc() || GetElement()->IsHelpRule())) {
    // this happens more often
    int occ = GetElementMatching()->Occurrences();
    if (occ == 0)
      return "";
    int length = occ;
    WordToken **t = GetElementMatching()->GetWordTokensAt();
    for (int i = 0; i < occ; i++)
      if (t[i]->GetWord())
	length += strlen(t[i]->RealString());
      else {
	Message(MSG_WARNING, "evaluatedVariable", int2str(i), int2str(occ), "no word");
	return "";
      }
    char *s = infoStringBuf.NewString(length);
    strcpy(s, t[0]->RealString());
    for (int j = 1; j < occ; j++) {
      strcat(s, " ");
      strcat(s, t[j]->RealString());
    }
    //    std::cout << "RealString: " << s << std::endl;
    return s;
  }
  return GetWordToken()->RealString();
}

void EvaluatedVariable::Mark() {
  if (!IsIndexed() && (GetElement()->MultiOcc() || GetElement()->IsHelpRule()))
    for (int i=0; i<GetElementMatching()->Occurrences(); i++)
      GetElementMatching()->GetWordToken(i)->SetMarked();
  else
    GetWordToken()->SetMarked();
}


