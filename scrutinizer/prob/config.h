#ifndef PROB_CONFIG_H
#define PROB_CONFIG_H

#include "probdefines.h"


namespace Prob
{
    class Config;
}

class Prob::Config
{
public:
    Config()
	: model_c(0), decision(0), annot_file(0), repr_file(0),
	  pipe_name("mynamedpipe"), 
	  g_no(0), g_coeff(1), h_no(4), h_coeff(3), use_context(0),
	  multiple_detect(false), use_span(false),
	  clause_delimits_context(true),
	  xml_output(true), bio(false)
    {
	weights[0] = -1;
	thresh[0]  = .5;
	model[0]   = 6;
	model_c    = 1;
	decision   = "none";
    }

    int	    model[MAX_MODELS];
    float   thresh[MAX_MODELS];
    int	    model_c;
    const char  *decision;
    const char  *annot_file;
    const char  *repr_file;
    char   *pipe_name;
    int     g_no;
    double  g_coeff;
    int	    h_no;
    double  h_coeff;
    int     use_context;

    bool    multiple_detect;
    bool    use_span;
    bool    clause_delimits_context;
    bool    xml_output;
    bool    bio;		// begin/inside/outside parse format

    double  weights[BEST_COUNT];
    int	    output[6];		    // detected, known, etc. for prec/rec of prob. scrut.
};


#endif /* PROB_CONFIG_H */
