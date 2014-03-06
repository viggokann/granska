#ifndef PROB_MATCH_H
#define PROB_MATCH_H

#include "probdefines.h"


class Tag;

namespace Prob
{
    enum represent_t
    {
	NOT_APPLICABLE,
	REPLACE,
	REMOVE
    };

    class Match
    {
    public:
	Match(int              b,
              int              e,
              const char      *n,
              Tag             *t,
              represent_t      r = NOT_APPLICABLE)
	    : begin(b), end(e), name(n),
	      tag_c(0), type(r)
	{
	    if(t)
		tag[tag_c++] = t;
	}
	bool operator<(const Match &m) const
	{
	    int len  = end - begin;
	    int lenm = m.end - m.begin;
	    return len < lenm ||
		   (len == lenm && begin < m.begin);
	}
	represent_t applicable(int	     from,
			       int	     to,
			       const Tag    *first,
			       bool	     silent = true);
#ifdef IMPROVE_TAGGER
	represent_t tagger_applicable(const Tag	    *first,
				      bool	     silent = true);
#endif // IMPROVE_TAGGER

	int		     begin;
	int		     end;
	const char	    *name;
	Tag		    *tag[3];
	int		     tag_c;
	represent_t	     type;

    };

}


#endif /* PROB_MATCH_H */

