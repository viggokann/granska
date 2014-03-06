#ifndef PROB_ADMIN_H
#define PROB_ADMIN_H

#include "prob.h"	    // convenience: load etc
#include "defines.h"	    // TRY_CONTEXT_REPRESENTATIVES etc
#include "tagdefines.h"	    // MAX_TAGS
#include "probdefines.h"    // MAX_MODELS, BEST_COUNT etc
#include <string>
#include <vector>

class TagLexicon;
class Tag;
class Matching;

namespace Prob
{
    typedef unsigned char uchar;
    class Config;
    class Info;

    // internal
    const char	   *model_name();
    const char	   *model_name(int i);
    int		    decision(int results[MAX_MODELS]);

    float   analyze(const TagLexicon	&l,
		    uchar		 index[5],
		    int			 model,
		    bool		 debug = false);

    void    init_probcheck(const Config &cfg);

    Tag	   *get_tag(const Info         &info,
                    const Matching     *match);
    void    get_intervals(const std::vector<int>	    &errs,
                          std::vector<std::pair<int, int> > &intv);


    extern std::string	    tag_name[MAX_TAGS];
    extern int		    tag_freq[MAX_TAGS];		// jb: remove me
    extern double	    repr_prob[MAX_TAGS][MAX_TAGS];
    extern int		    repr_count[MAX_TAGS];
    extern int		    repr_best[MAX_TAGS][BEST_COUNT];

#ifdef TRY_CONTEXT_REPRESENTATIVES
    extern int		    repr_context_left[MAX_TAGS][MAX_TAGS][BEST_COUNT];
    extern int		    repr_context_right[MAX_TAGS][MAX_TAGS][BEST_COUNT];
    extern double	    prob_context_left[MAX_TAGS][MAX_TAGS][BEST_COUNT];
    extern double	    prob_context_right[MAX_TAGS][MAX_TAGS][BEST_COUNT];
  #ifdef IGNORE_TAG_FEATURES
    extern int		    class_count;
    extern int		    tag2class[MAX_TAGS];
    extern std::string	    class_name[MAX_TAGS];
  #endif
#endif

    extern int sentence_offset;
    extern int sentence_count;
    extern const char *annot_file;
    extern const char *output_file;
    extern const AbstractSentence *current_sentence;

    extern double repr_adhoc[BEST_COUNT];
    extern double repr_recip[BEST_COUNT];
    extern double repr_pow2[BEST_COUNT];
    extern double repr_pow2a[BEST_COUNT];
    extern double repr_pow2b[BEST_COUNT];
    extern double repr_pow2c[BEST_COUNT];
    extern double repr_1_min[BEST_COUNT];
    extern double repr_unit[BEST_COUNT];
    extern double repr_four[BEST_COUNT];
    extern double repr_id[BEST_COUNT];
}


#ifdef USE_AS_CLIENT
#include "pipe_client.h"

class Scrutinizer;
class Prob::Client : public Misc::Pipe_client
{
public:
    Client(std::string pipe_name, Scrutinizer &, Prob::Config &);
    ~Client();

    virtual std::string	  send();
    virtual bool          received(std::string s);

protected:
    Scrutinizer	&scrut;
    Prob::Config &config;
    bool             calc;
};
#endif // USE_AS_CLIENT


#endif // PROB_ADMIN_H
