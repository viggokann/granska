#ifndef PROB_H
#define PROB_H


class TagLexicon;
class Scrutinizer;
class AbstractSentence;
class Rule;

namespace Prob
{
    class Config;

    // init, exit and output
    Config &config();
    void    load(const TagLexicon	&l);
    void    print(const Scrutinizer     *scrut);
    void    unload();
    void    reset();

    // used from scrutinizer
    int     prob_check(const Scrutinizer	 *scrut,
		       const AbstractSentence	 *sent);
    int	    last_prob_error();
    void    report(const AbstractSentence     *sent,
                   const Rule                 *rule,
                   int                         size,
		   const int		       start[],
		   const int		       stop[]);
    Config &config();
}


#endif // PROB_H
