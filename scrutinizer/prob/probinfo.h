#ifndef PROB_INFO_H
#define PROB_INFO_H

class Scrutinizer;
class WordToken;
class Tag;
class TagLexicon;

namespace Prob
{
    class Info
    {
    public:
	Info(const Scrutinizer            *s,
             const WordToken * const      *tok,
             const char                   *w[],
             const Tag                    *t[]);
	Info(const Info &i, int cl_from, int cl_to, const Tag *t[] = 0);

    public:
	const Scrutinizer       *scrutinizer;
	const TagLexicon        *l;
	const char	       **words;
	const Tag	       **tags;
	const WordToken * const *tokens;
	int                      size;
	int                      model;
	int                      sent_offset;
	int                      clause_from;
	int                      clause_to;
    };

}


#endif /* PROB_INFO_H */

