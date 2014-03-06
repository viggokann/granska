/* gramerror.cc
 * author: Johan Carlberger
 * last Viggo change:
 * last Johan change: 2000-03-27
 * comments: GramError class
 */

#include "expr.h"
#include "rulesettings.h"
#include "gramerror.h"
#include "scrutinizer.h"
#include "matchingset.h"
#include <sstream>

#ifdef PROBCHECK
#include "prob.h"
#include "output.h"
#endif // PROBCHECK


StringBuf GramError::stringBuf;
Scrutinizer *GramError::scrutinizer = NULL;
MatchingSet *GramError::matchingSet = NULL;

/* settings to check:
extern bool xAcceptNonImprovingCorrections;
extern bool xAcceptRepeatedSuggestions;
extern bool xSuggestionSameAsOriginalMeansFalseAlarm;
*/

DefObj(GramError);

/* this is an ugly hack to make xml tags
   have spaces on both sides of the whole
   xml-object, i.e. we want 
   "En <emph>liten</emph> bil." instead of
   "En <emph>liten </emph>bil." The reason
   is that it is easier to put back the marked
   words in the sentence when using an xml parser
   in Java if there are spaces in this way. This
   should perhaps be done in a nice clean way
   instead?

   /Jonas
*/

std::string move_xmltag_and_space(std::string s) {
  int pos = s.find(" </emph>");
  while(pos >= 0) {
    s.replace(pos, 8, "</emph> ");
    pos = s.find(" </emph>", pos+8);
  }
  return s;
}

bool GramError::Evaluate() {
  ensure(scrutinizer); ensure(matchingSet);
  ensure(matching);
  Matching *m = matching;
  matching = NULL;
  ruleTerm = m->GetRuleTerm();
  const AbstractSentence *s = sentence = m->GetSentence();
  bool falseAlarm = false;

  // create alt sentences:
  m->SetAltSentence(NULL);
  for (Expr* c = ruleTerm->GetCorr(); c; c = c->Next()) {
    DynamicSentence *ds = new DynamicSentence(s);
    DynamicSentence *prevAlt = m->GetAltSentence();
    m->SetAltSentence(ds);
    if (!m->GetRuleTerm()->EvaluateCorr(m, c))
      Message(MSG_WARNING, "evaluation of corr failed", m->GetRule()->Name());     
    if (prevAlt) m->AddAltSentences(prevAlt);
  }

  // set status info:
  DynamicSentence *d;
  for (d = m->GetAltSentence(); d; d = d->Next()) {
    if (d->Status() == FORM_NOT_FOUND)
      continue;
    if (s->IsEqual(d)) {
      d->status = SAME_AS_ORIGINAL;
      if (xSuggestionSameAsOriginalMeansFalseAlarm) {
	falseAlarm = true;
	break;
      }
    }
    for (DynamicSentence *d2 = m->GetAltSentence(); d2 && d2 != d; d2 = d2->Next())
      if (d2->Status() != FORM_NOT_FOUND && d->IsEqual(d2)) {
	d->status = SAME_AS_ANOTHER;
	break;
      }
  }

  if (!falseAlarm) {
    // extract info:
    info = stringBuf.NewString(ruleTerm->EvaluateInfo(m));
    if (*info == '\0')
      Message(MSG_WARNING, "no info from", ruleTerm->GetRule()->Name());
    
    // extract mark:
    std::ostringstream out2;
    if (ruleTerm->GetMark())
      ruleTerm->EvaluateMark(m);
    else
      for (int i=m->Start(); i<= m->End(); i++)
	s->GetWordToken(i)->SetMarked();
    out2 << xRed;
    bool gap = false, anyMark = false;
    nMarkedSections = 0;
    start[0] = stop[0] = -1;
    int firstWTpos = (m->Start() >= 2) ? m->Start() : 2;
    int lastWTpos = (m->End() >= s->NTokens()-2) ? s->NTokens()-3 : m->End();
    for (int i=2; i<s->NTokens()-2; i++)
	if (s->GetWordToken(i)->IsMarked()) {
	    if (!anyMark) firstWTpos = i;
	    lastWTpos = i;
	    anyMark = true;
	    s->GetWordToken(i)->SetMarked(0);
	    s->GetWordToken(i)->SetMarked2();
	    if (gap) { out2 << " ... "; gap = false; }
	    out2 << s->GetWordToken(i);
	    if (start[nMarkedSections] < 0)
		start[nMarkedSections] = i;
	    stop[nMarkedSections] = i;
	} else {
	    gap = anyMark;
	    if (start[nMarkedSections] >= 0) {
		if (nMarkedSections >= MAX_MARKED_SECTIONS-1)
		    Message(MSG_WARNING, "too many marked sections");
		nMarkedSections++;
		start[nMarkedSections] = stop[nMarkedSections] = -1;
	    }
	}
    // jbfix: if the marked area ended in the last position
    // nMarkedSections was not incremented
    if(start[nMarkedSections] != -1)
    {
	if(nMarkedSections >= MAX_MARKED_SECTIONS-1)
	    Message(MSG_WARNING, "too many marked sections");
	nMarkedSections++;
    }

    if (!anyMark) {
      Message(MSG_WARNING, "nothing marked with", ruleTerm->GetRule()->Name());
      start[0] = firstWTpos;
      stop[0] = lastWTpos;
      nMarkedSections = 1;
    } else if (firstWTpos <= 2 && lastWTpos >= s->NTokens()-3) {
      allMarked = true;
      markedArea = "";
      start[0] = firstWTpos;
      stop[0] = lastWTpos;
      nMarkedSections = 1;
    } else {
      out2 << xNoColor << '\0';
      markedArea = stringBuf.NewString(out2.str().c_str());
    }

    // jb: mem is now handled by std::string
    //if (str) delete str; else delete out2.str();
    
    // create suggestions:
    for (d = m->GetAltSentence(); d; d = d->Next()) {
      if (nSuggestions >= MAX_SUGGESTIONS) {
	Message(MSG_WARNING, "too many suggestions");
	break;
      }
      switch (d->Status()) {
      case FORM_NOT_FOUND:
	nErrors[nSuggestions] = 0;
	suggestion[nSuggestions++] = stringBuf.NewString("FORM NOT FOUND");
	continue;
      case SAME_AS_ANOTHER:
	if (xAcceptRepeatedSuggestions) break;
	continue;
      case NOT_IMPROVING: ensure(0);
      case SAME_AS_ORIGINAL:
      case SEEMS_OK:
	break;
      }
      d->TagMe();
      matchingSet->SetCheckMode(true);
      scrutinizer->Scrutinize(d);
      nErrors[nSuggestions] = (char) matchingSet->NCheckModeFound();
      matchingSet->SetCheckMode(false);
      if (nErrors[nSuggestions] >= sentence->NGramErrors())
	d->status = NOT_IMPROVING;
      if (xPrintMatchings) {
	std::cout << tab;
	switch(d->Status()) {
	case SEEMS_OK: std::cout << "(OK)"; break;
	case SAME_AS_ORIGINAL: std::cout << "(NO CHANGES)"; break;
	case SAME_AS_ANOTHER: std::cout << "(REPEATED)"; break;
	case NOT_IMPROVING: std::cout << "(NOT IMPROVING)"; break;
	case FORM_NOT_FOUND: std::cout << "(FORM NOT FOUND)"; break;
	}
	std::cout << std::endl;
      }
      if (!xAcceptNonImprovingCorrections && d->Status() == NOT_IMPROVING)
	continue;
      std::ostringstream out;
      d->PrintOrgRange(firstWTpos, lastWTpos, out);  
      switch(d->Status()) {
      case SEEMS_OK: break;
      case SAME_AS_ORIGINAL: out << " (NO CHANGES)"; break;
      case SAME_AS_ANOTHER: out << " (REPEATED)"; break;
      case NOT_IMPROVING: out << " (NOT IMPROVING, gives " << (int)nErrors[nSuggestions]
			      << " errors, " << sentence->NGramErrors() << " before)"; break;
      case FORM_NOT_FOUND: ensure(0); break;
      }
      out << '\0';

      //      suggestion[nSuggestions++] = stringBuf.NewString(out.str().c_str());
      suggestion[nSuggestions++] = stringBuf.NewString(move_xmltag_and_space(out.str()).c_str());
    }
  }
  // delete alt sentences:
  DynamicSentence *nxt = NULL;
  for (d = m->GetAltSentence(); d; d = nxt) {  
    nxt = d->Next();
    delete d;
  }
  m->SetAltSentence(NULL);
  //  std::cout << this << std::endl;

  return !falseAlarm;
}

#ifdef PROBCHECK
#include <iostream>
#include <string>
#endif

void GramError::Print(std::ostream& out) const
{
    const char *marked = MarkedArea();
    out << xTab << (marked ? marked : "(NULL)") << xTab
	<< xBlue << Info()
	<< xSmall << " (" << ruleTerm->GetRule() << ')' << xNoSmall
	<< xNoColor;
    if (xPrintHTML)
	out << " <A HREF=\"" << ruleTerm->GetLinkURL()
	<< "\">" << ruleTerm->GetLinkText() << "</A>";
    out << xEndl;
    //  for (int j=0; j<NMarkedSections(); j++)
    //    out << xTab << Start(j) << ',' << Stop(j) << xEndl;
    for (int i=0; i<NSuggestions(); i++)
	out << xTab << Suggestion(i) << xEndl;
}

void GramError::Report() const
{
#ifdef PROBCHECK    
    Prob::report(GetSentence(), ruleTerm->GetRule(),
		    NMarkedSections(), start, stop);
#endif // PROBCHECK
}

bool GramError::IsError() const
{
#ifdef PROBCHECK
    std::string r = ruleTerm->GetRule()->Name();
    if(r.length() >= 5 &&
       r.substr(r.length() - 5) == "recog")
	return false;
#endif // PROBCHECK

    return true;
}

void GramError::Output() const
{
#ifdef PROBCHECK    
    Prob::Output &o = Prob::output();

    o.push("gramerror");
    o.add("marked", MarkedArea());
    o.add("rule", ruleTerm->GetRule()->Name());
    o.add("info", Info());
#ifdef DEVELOPER_OUTPUT
    o.add("category", ruleTerm->GetCategory()->Name());
    o.add("url", ruleTerm->GetLinkURL());
    o.add("urltext", ruleTerm->GetLinkText());
#endif // DEVELOPER_OUTPUT
    if(NSuggestions() > 0)
    {
	o.push("suggestions");
	for(int i = 0; i < NSuggestions(); i++)
	    o.add("sugg", Suggestion(i));
	o.pop();  // push("suggestions");
    }
    if(NMarkedSections() > 0)
    {
	o.push("marked_section");
	for(int i = 0; i < NMarkedSections(); i++)
	{
	    o.add("mark");
	    o.attr("begin", Start(i));
	    o.attr("end", Stop(i));
	}
	o.pop();  // push("suggestions");
    }
    o.pop();  // push("gramerror");
#endif // PROBCHECK
}
