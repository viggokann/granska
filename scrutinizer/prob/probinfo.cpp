#include "probinfo.h"
#include "scrutinizer.h"


Prob::Info::Info(const Scrutinizer           *s,
                    const WordToken * const     *tok,
                    const char                  *w[],
                    const Tag                   *t[]) 
  : scrutinizer(s), l(&s->Tags()),
    words(w), tags(t), tokens(tok), size(0),
    model(0), sent_offset(0),
    clause_from(0), clause_to(0)
{}
	
Prob::Info::Info(const Info &i, int cl_from, int cl_to, const Tag *t[])
{
    *this = i;			// copy all
    clause_from = cl_from;	// change clause boundaries
    clause_to   = cl_to;
    if(t)
	tags    = t;
}
