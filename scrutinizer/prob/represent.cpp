#ifdef PROBCHECK

#pragma warning(disable: 4786)
#include "probadmin.h"
#include "taglexicon.h"
#include "tag.h"
#include "annot.h"
#include "config.h"

#include <fstream>
#include <iostream>
#include <string>

#ifdef TRY_PAROLE_TTT
#include "trigrams.h"
#endif // TRY_PAROLE_TTT



namespace Prob
{
    double apply_g(double);

#ifdef TRY_CONTEXT_REPRESENTATIVES
    void best_repr_from_context(uchar	     t[5],
				int	     i,
				int	     best[MAX_TAGS][BEST_COUNT])
    {
	static int    output_count = 0;
	const double  BIG_NUM = 1;
	extern double repr_prob_clean[MAX_TAGS][MAX_TAGS];
	//const bool    output = !(output_count % 571);
	//const bool    output = !(sentence_offset % 100);
	const bool    output = false;
	output_count++;

	double count_left[MAX_TAGS];
	double count_right[MAX_TAGS];
	double count_global[MAX_TAGS];
	int j;
	for(j = 0; j < MAX_TAGS; j++)
	    count_left[j] = count_right[j] = count_global[j] = BIG_NUM;

	if(output)
	{
	    std::cout << std::endl;
	    for(int j = 0; j < 5; j++)
	    {
		if(j == i)
		    std::cout << "*";
		std::cout << tag_name[t[j]] 
		          << "(" << (int)t[j] << ")";
		if(j == i)
		    std::cout << "*";
		std::cout << "  ";
	    }
	    std::cout << std::endl;
	    std::cout << "sentence offset: " << sentence_offset << std::endl;
	}

	// left
  #ifdef IGNORE_TAG_FEATURES
	if(output)
	    std::cout << "left context: " << class_name[tag2class[t[i - 1]]]
	              << "(" << tag2class[t[i - 1]] << ")" << std::endl;
  #endif // IGNORE_TAG_FEATURES
	for(j = 0; j < repr_count[t[i]]; j++)
	{
  #ifdef IGNORE_TAG_FEATURES
	    int    repr = repr_context_left[t[i]][tag2class[t[i - 1]]][j];
	    double prob = prob_context_left[t[i]][tag2class[t[i - 1]]][j] / 4;
  #else // IGNORE_TAG_FEATURES
	    int    repr = repr_context_left[t[i]][t[i - 1]][j];
	    double prob = prob_context_left[t[i]][t[i - 1]][j] / 4;
  #endif // IGNORE_TAG_FEATURES
	    if(count_left[repr] == BIG_NUM)
		count_left[repr] = prob;
	    if(output)
		std::cout << j << ": " << tag_name[repr] 
			  << "(" << repr << "), prob: " << prob << std::endl;
	}
	if(output)
	    std::cout << std::endl;

	// right
  #ifdef IGNORE_TAG_FEATURES
	if(output)
	    std::cout << "right context: " << class_name[tag2class[t[i + 1]]]
	              << "(" << tag2class[t[i + 1]] << ")" << std::endl;
  #endif // IGNORE_TAG_FEATURES
	for(j = 0; j < repr_count[t[i]]; j++)
	{
  #ifdef IGNORE_TAG_FEATURES
	    int    repr = repr_context_right[t[i]][tag2class[t[i + 1]]][j];
	    double prob = prob_context_right[t[i]][tag2class[t[i + 1]]][j] / 4;
  #else // IGNORE_TAG_FEATURES
	    int    repr = repr_context_right[t[i]][t[i + 1]][j];
	    double prob = prob_context_right[t[i]][t[i + 1]][j] / 4;
  #endif // IGNORE_TAG_FEATURES
	    if(count_right[repr] == BIG_NUM)
		count_right[repr] = prob;
	    if(output)
		std::cout << j << ": " << tag_name[repr] 
			  << "(" << repr << "), prob: " << prob << std::endl;
	}
	if(output)
	    std::cout << std::endl;

	// global
	if(output)
	    std::cout << "global:" << std::endl;
	for(j = 0; j < repr_count[t[i]]; j++)
	{
	    int    repr = repr_best[t[i]][j];
	    double prob = repr_prob_clean[t[i]][repr] / 6;
	    count_global[repr] = prob;
	    if(output)
		std::cout << j << ": " << tag_name[repr] 
			  << "(" << repr << "), prob: " << prob << std::endl;
	}
	if(output)
	    std::cout << std::endl;
	
	typedef std::multimap<double, int> map;
	map m;
	for(j = 0; j < MAX_TAGS; j++)
	{
	    double val = 0;
	    if(config().use_context == 4)		// jb: output to pos_context.4
	    {
		val += count_left[j];
		val += count_right[j];
		val += count_global[j] * 2;
		val /= 4;
	    }
	    //if(output)
	    //	std::cout << "j: " << val << std::endl;

	    m.insert(map::value_type(val, j));
	}
	map::const_iterator iter = m.begin();
	bool found = false;
	for(j = 0; j < BEST_COUNT && iter != m.end(); j++, ++iter)
	{
	    best[t[i]][j] = iter->second;
	    if(output)
	    {
		std::cout << j << ": " << tag_name[iter->second]
			  << "(" << iter->second << ")"
			  << ", rating: " << iter->first;
		//	  << "(" << iter->first * 3 << ")";
    		if(iter->second != repr_best[t[i]][j])
		    std::cout << " ***";
		std::cout << std::endl;
	    }
	}
	if(output)
	    std::cout << std::endl;
    }
#endif // TRY_CONTEXT_REPRESENTATIVES

#ifndef TRY_PAROLE_TTT
    static float
    analyze_helper(const TagLexicon   &l,
                   uchar               t[5],
                   const double	       repr_fact[BEST_COUNT],
                   bool                print/*,
                   scheme_t            scheme = S_NONE*/)
#else // TRY_PAROLE_TTT
    static float 
    analyze_helper(const Trigrams     &l, 
		   uchar	       t[5],
		   const double	       repr_fact[BEST_COUNT],
		   bool		       print/*,
		   scheme_t	       scheme = S_NONE*/)
#endif // TRY_PAROLE_TTT
    {
	double max_weight = 0;
	int    max_freq = 0;
	uchar  max_i[3];
	double max_prob[3];
	double max_factor[3];
	for(int j = 0; j < 3; j++)
	    max_prob[j] = max_factor[j] = 0;
	bool found = false;
	int    nonzero_count = 0;
	double max_weighted = 0;

	// bigram baseline
	if(config().h_no == 5)
	    return l.tt_freq(t[1], t[2]);
  #if 0
	// jb: performance much improved for higher thresholds (i.e. >= 10)
	if(l.ttt_freq(t[1], t[2], t[3]) >= 5)
	    return BIG_NUM;
  #endif

	// decide type of representatives (use of context or not)
	int (*best)[BEST_COUNT] = 0;
  #ifdef TRY_CONTEXT_REPRESENTATIVES
	int get_best[MAX_TAGS][BEST_COUNT];
	if(config().use_context != 0)
	{
	    // jb: wow! this one was ugly! (output at first use)
	    static void *p = &(std::cout << "using context info "
					 << config().use_context << std::endl);


	    // left and right context
	    if(t[0] == 255 || t[4] == 255)	    // no context available
	    {
		// jb: CHANGE BACK to best = repr_best
		best = repr_best;		    // use ordinary method
		//return BIG_NUM;
	    }
	    else
	    {
		best_repr_from_context(t, 1, get_best);
		best_repr_from_context(t, 2, get_best);
		best_repr_from_context(t, 3, get_best);
	 	best = get_best;
	    }
	}
	else
  #endif // TRY_CONTEXT_REPRESENTATIVES
	{
	    // jb: wow! this one was ugly! (output at first use)
	    static void *p = &(std::cout << "not using context info" << std::endl);

	    best = repr_best;
	}

	double res = 0;
	for(int i1 = 0; i1 < repr_count[t[1]]; i1++)
	    for(int i2 = 0; i2 < repr_count[t[2]]; i2++)
		for(int i3 = 0; i3 < repr_count[t[3]]; i3++)
		{
  #ifndef LEFT_ONLY
		    int r1 = best[t[1]][i1];
		    int r2 = best[t[2]][i2];
		    int r3 = best[t[3]][i3];
  #else
		    // left context only
		    int r1 = repr_context_left[t[1]][t[0]][i1];
		    int r2 = repr_context_left[t[2]][t[1]][i2];
		    int r3 = repr_context_left[t[3]][t[2]][i3];
  #endif

		    double prob1 = repr_prob[t[1]][r1];
		    double prob2 = repr_prob[t[2]][r2];
		    double prob3 = repr_prob[t[3]][r3];

		    double weight = prob1 * repr_fact[i1] *
				    prob2 * repr_fact[i2] *
				    prob3 * repr_fact[i3];
		    if(config().h_no == 4)
		    {
			if(i1 + 1 > config().h_coeff ||
			   i2 + 1 > config().h_coeff ||
			   i3 + 1 > config().h_coeff)
			   weight = 0;
		    }

		    int    freq = l.ttt_freq(r1, r2, r3);
		    if(freq > 0)
			nonzero_count++;

		    //if(freq <= 1)   // ignore noise frequencies below n
		    //	continue;

		    res += freq * weight;

		    if(weight * freq > max_weighted)
		       max_weighted = weight * freq;

		    // for print only
		    if(print && freq >= max_freq)
		    {
			found = true;
			max_freq = freq;
			max_weight = weight;
			max_i[0] = i1;
			max_i[1] = i2;
			max_i[2] = i3;
			max_prob[0] = prob1;
			max_prob[1] = prob2;
			max_prob[2] = prob3;
			max_factor[0] = repr_fact[i1];
			max_factor[1] = repr_fact[i2];
			max_factor[2] = repr_fact[i3];
		    }
		}

	// choose weight schema
	{
	    switch(config().h_no)
	    {
	    case 0: break;
	    case 1: res /= nonzero_count; break;
	    case 2: res = max_weighted; break;
	    case 3: res /= repr_count[t[1]] * repr_count[t[2]] * repr_count[t[3]]; res *= 1000; break;
	    }
	}

	if(print && found)
	{
	    std::cout.precision(6);
	    int r1 = repr_best[t[1]][max_i[0]];
	    int r2 = repr_best[t[2]][max_i[1]];
	    int r3 = repr_best[t[3]][max_i[2]];
	    std::cout << "Pr: " << res << ", freq("
		      << tag_name[r1] << ", "
		      << tag_name[r2] << ", "
		      << tag_name[r3] << ") = " << max_freq;
	    std::cout.precision(2);
	    std::cout << ", wt: ("
		      << max_prob[0] << "," << max_factor[0] << ")("
		      << max_prob[1] << "," << max_factor[1] << ")("
		      << max_prob[2] << "," << max_factor[2] << ") = ";
	    std::cout.precision(6);
	    std::cout << max_weight << " => "
		      << max_freq * max_weight << std::endl;
	}
	else if(print && !found)
	    std::cout << "Pr: 0 (no trigrams found)" << std::endl;

	return res;
    }

    inline static float
    analyze(const TagLexicon     &l,
            uchar                 t[5],
            const double          repr_fact[BEST_COUNT],
            bool                  print)
    {
	#ifndef TRY_PAROLE_TTT
	    return analyze_helper(l, t, repr_fact, print);
	#else // TRY_PAROLE_TTT
	    return analyze_helper(*parole_trigrams, t, repr_fact, print);
	#endif // TRY_PAROLE_TTT
    }

    float analyze(const TagLexicon &l, uchar t[5], int index, bool print)
    {
	double d = 0;
    #ifdef USE_AS_CLIENT
	if(true)
    #else // USE_AS_CLIENT
	if(config().weights[0] != -1)	// external weights given
    #endif // USE_AS_CLIENT
	{
	    d = analyze(l, t, config().weights, print);
	    return d;
	}
	switch(config().model[index])
	{
	case 0: d = analyze(l, t, repr_pow2, print); break;
	case 1: d = analyze(l, t, repr_pow2a, print); break;
	case 2: d = analyze(l, t, repr_pow2b, print); break;
	case 3: d = analyze(l, t, repr_pow2c, print); break;
	case 4: d = analyze(l, t, repr_recip, print); break;
	case 5: d = analyze(l, t, repr_adhoc, print); break;
	case 6: d = analyze(l, t, repr_unit, print); break;
	case 7: d = analyze(l, t, repr_four, print); break;
	case 8: d = analyze(l, t, repr_1_min, print); break;
	case 9: d = analyze(l, t, repr_id, print); break;
	default: std::cout << "ERROR: unknown prob test " << index << std::endl;
	}

	return d;
    }
} // end namespace


#endif // ifdef PROBCHECK (topmost)
