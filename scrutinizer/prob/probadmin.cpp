#ifdef PROBCHECK

#pragma warning(disable: 4786)
#include "probadmin.h"
#include "taglexicon.h"
#include "tag.h"
#include "annot.h"
#include "config.h"
#include "defines.h"
#include "probdevelop.h"
#include "report.h"
#include "matching.h"

#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>

#ifdef TRY_PAROLE_TTT
#include "trigrams.h"
#endif // TRY_PAROLE_TTT

namespace Prob
{
    std::string	    tag_name[MAX_TAGS];
    int		    tag_freq[MAX_TAGS];		// jb: remove me
    double	    repr_prob[MAX_TAGS][MAX_TAGS];
    double	    repr_prob_clean[MAX_TAGS][MAX_TAGS];
    int		    repr_count[MAX_TAGS];
    int		    repr_best[MAX_TAGS][BEST_COUNT];
#ifdef TRY_CONTEXT_REPRESENTATIVES
    int		    repr_context_left[MAX_TAGS][MAX_TAGS][BEST_COUNT];
    int		    repr_context_right[MAX_TAGS][MAX_TAGS][BEST_COUNT];
    double	    prob_context_left[MAX_TAGS][MAX_TAGS][BEST_COUNT];
    double	    prob_context_right[MAX_TAGS][MAX_TAGS][BEST_COUNT];
  #ifdef IGNORE_TAG_FEATURES
    int		    class_count;
    int		    tag2class[MAX_TAGS];
    std::string	    class_name[MAX_TAGS];
  #endif
#endif

    double    repr_adhoc[BEST_COUNT] =  { 1.0,   1.0,   1.0,   2.0/3,
					1.0/2, 1.0/3, 1.0/4, 1.0/5,
					1.0/6, 1.0/7 };
    double    repr_recip[BEST_COUNT] =  { 1.0/1,
					1.0/2,
					1.0/3,
					1.0/4,
					1.0/5,
					1.0/6,
					1.0/7,
					1.0/8,
					1.0/9,
					1.0/10 };
    double    repr_pow2[BEST_COUNT] =   { 1.0/1,
					1.0/2,
					1.0/4,
					1.0/8,
					1.0/16,
					1.0/32,
					1.0/64,
					1.0/128,
					1.0/256,
					1.0/512 };
    double    repr_pow2a[BEST_COUNT] =  { 1.0/1,
					1.0/1,
					1.0/1,
					1.0/2,
					1.0/4,
					1.0/8,
					1.0/16,
					1.0/32,
					1.0/64,
					1.0/128 };
    double    repr_pow2b[BEST_COUNT] =  { 1.0/1,
					1.0/2,
					1.0/4,
					1.0/8,
					1.0/8,
					1.0/8,
					1.0/8,
					1.0/8,
					1.0/8,
					1.0/8 };
    double    repr_pow2c[BEST_COUNT] =  { 1.0/1,
					1.0/2,
					1.0/4,
					1.0/8,
					1.0/16,
					1.0/16,
					1.0/16,
					1.0/16,
					1.0/16,
					1.0/16 };
    double    repr_1_min[BEST_COUNT] =  { 1.0/1,
					9.0/10,
					8.0/9,
					7.0/8,
					6.0/7,
					5.0/6,
					4.0/5,
					3.0/4,
					2.0/3,
					1.0/2 };
    double    repr_unit[BEST_COUNT] =   { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    double    repr_four[BEST_COUNT] =   { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 };
    double    repr_id[BEST_COUNT]   =   { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    
    double apply_g(double);

    void load_repr_prob(const TagLexicon &l,
			double prob[MAX_TAGS][MAX_TAGS])
    {
	std::string file = config().repr_file;
	std::ifstream represent(file.c_str());

	if(!represent.is_open())
	{
	    std::cerr << "cluster: could not open file '" 
		      << file << "'" << std::endl;
	    //throw "file error";
	    std::exit(1);
	}

	for(int i = 0; i < MAX_TAGS; i++)
	    repr_count[i] = 0;

	double d;
	std::string from;
	std::string to;
	std::string old_from;
	int best_counter = 0;
	std::cout.precision(6);
	while(represent)
	{
	    char buf[128];
	    represent >> d;
	    represent >> buf;
	    from = buf;
	    represent >> buf;
	    to = buf;
	    if(from.empty() || to.empty())
		break;
	    if(from != old_from)
		best_counter = 0;
	    old_from = from;

	    if(best_counter >= BEST_COUNT)
		continue;

	    Tag *f = l.FindTag(from.c_str());
	    Tag *t = l.FindTag(to.c_str());

	    if(tag_name[f->Index()].empty())
		tag_name[f->Index()] = from;

	    // translate from error prob to fit prob
	    d = apply_g(d);

	    prob[f->Index()][t->Index()] = d;
	    repr_best[f->Index()][best_counter] = t->Index();
	    best_counter++;
	    repr_count[f->Index()]++;
	}
    }

    void load_context_repr()
    {
#ifdef TRY_CONTEXT_REPRESENTATIVES    
	// init context indexes
	{
	    std::string file = config().repr_file;
	    std::ifstream f(file.c_str());

	    if(!f.is_open())
	    {
		std::cerr << "cluster: could not open file '" 
			  << file << "'" << std::endl;
		//throw "file error";
		std::exit(1);
	    }

	    for(int i = 0; i < MAX_TAGS; i++)
	    {
		repr_count[i] = BEST_COUNT;
    		for(int j = 0; j < MAX_TAGS; j++)
		    for(int k = 0; k < BEST_COUNT; k++)
		    {
			repr_context_left[i][j][k]  = i;
			repr_context_right[i][j][k] = i;
		    }
	    }

	    int c = 0;
	    bool left = true;
	    int count = 0;
	    while(f)
	    {
		double prob;
		f >> prob;
		if(prob == -1)	    // switch read left/right context
		{
		    if(!left)
			break;
		    left = false;
		    c = 0;
		    f >> prob;	    // read next line
		}

		int tag, ctx, repr;
		f >> tag >> ctx >> repr;
		//std::cerr << prob << ", " << tag << ", " << ctx << ", " << repr << std::endl;
		if(repr != tag && c == 0)
		{
		    std::cerr << "First-choise representative differs from tag at line "
			      << count + 1 << std::endl;
		    std::cerr << "prob: " << prob << ", "
			      << "tag: " << tag << ", context: " << ctx
			      << ", repr: " << repr << std::endl;
		    std::exit(1);
		}

		if(left)
		{
		    repr_context_left[tag][ctx][c]  = repr;
		    prob_context_left[tag][ctx][c]  = prob;
		}
		else
		{
		    repr_context_right[tag][ctx][c] = repr;
		    prob_context_right[tag][ctx][c] = prob;
		}

		c++;
		if(c == BEST_COUNT)
		    c = 0;
		count++;
	    }
	}
#endif // TRY_CONTEXT_REPRESENTATIVES    

	// init all representatives
	{
	    std::cerr << "loading representative probabilities..." << std::endl;
	    char *tagger_lex = getenv("TAGGER_LEXICON");
	    std::string file = tagger_lex ? tagger_lex : "";	
#ifndef TRY_PAROLE_TTT
	    file += "tags/repr.pii";
	    //file += "tags/repr.pii";
#else
	    file += "tags/repr.parole.tagged.pii";
#endif
	    std::ifstream f(file.c_str());

	    if(!f.is_open())
	    {
		std::cerr << "cluster: could not open file '" 
			  << file << "'" << std::endl;
		//throw "file error";
		std::exit(1);
	    }

	    for(int i = 0; i < MAX_TAGS; i++)
	    {
		repr_count[i] = BEST_COUNT;
    		for(int j = 0; j < MAX_TAGS; j++)
		    repr_prob[i][j] = 0;
	    }

	    int old_tag = -1;
	    int best_counter = 0;
	    while(f)
	    {
		double prob;
		int tag, repr;
		f >> prob >> tag >> repr;

		double d = apply_g(prob);
		repr_prob[tag][repr] = d;
		repr_prob_clean[tag][repr] = prob;

		if(tag != old_tag)
		    best_counter = 0;
		if(best_counter < BEST_COUNT)
		    repr_best[tag][best_counter] = repr;
		best_counter++;
		old_tag = tag;
	    }
	}

#ifdef TRY_CONTEXT_REPRESENTATIVES
	// init context classes
	{
	    std::cerr << "loading context classes..." << std::endl;
	    char *tagger_lex = getenv("TAGGER_LEXICON");
	    std::string file = tagger_lex ? tagger_lex : "";	
	    file += "tags/classes";

	    std::ifstream f(file.c_str());

	    if(!f.is_open())
	    {
		std::cerr << "cluster: could not open file '" 
			  << file << "'" << std::endl;
		//throw "file error";
		std::exit(1);
	    }

	    int tmp;
	    std::string name;
	    class_count = 0;
	    while(f >> tmp >> name)
	    {
		tag2class[class_count] = tmp;
		class_name[tmp] = name;
		class_count++;
	    }
	}
#endif
    }

#ifdef TRY_PAROLE_TTT
    Trigrams *parole_trigrams;
#endif // TRY_PAROLE_TTT

    void load(const TagLexicon &l)
   {
#ifdef DEVELOPER_OUTPUT
	for(int j = 0; j < config().model_c; j++)
	{
	    std::cout << "model[" << j << "]: " << model_name(config().model[j])
		      << " (" << config().model[j] << "), factor "
		      << config().thresh[j] << std::endl;
	}
	std::cout << "decision_type = " << config().decision << std::endl;
	std::cout << "function g = " << config().g_no << std::endl;
	std::cout << "coeff g = " << config().g_coeff << std::endl;
	std::cout << "function h = " << config().h_no << std::endl;
	std::cout << "coeff h = " << config().h_coeff << std::endl;
#endif // DEVELOPER_OUTPUT
    
	for(int k = 0; k < l.Size(); k++)
	{
	    tag_name[k] = l.Element(k).String();
	    tag_freq[k] = l.t_freq(k);
	    //std::cerr << l.Element(k) << std::endl;
	}
	//std::exit(0);

#ifdef TRY_PAROLE_TTT
	parole_trigrams = new Trigrams(l);
	parole_trigrams->load("parole.tagged.ttt");
#endif // TRY_PAROLE_TTT

	if(config().repr_file)
	{
    	    std::cerr << "loading representatives from '" << config().repr_file << "'..." << std::endl;
#ifdef TRY_CONTEXT_REPRESENTATIVES
	    load_context_repr();
#else
	    load_repr_prob(l, repr_prob);
#endif
	}
	else
	    std::cerr << "no representative file specified (-Cr), not using representatives" 
		      << std::endl;
	init_probcheck(config());
    }

    static const char *model_names[] = 
    {
        "pow2", 
	"pow2a",
	"pow2b",
	"pow2c",
	"recip",
	"adhoc",
	"unit", 
	"four", 
	"1_min",
	"id",
    };

    const char *model_name(int i)
    {
	return model_names[i];
    }

    const char *model_name()
    {
	static char buf[1024];
	if(strcmp(config().decision, "none") == 0 ||
	   config().model_c == 1)
	   return model_names[config().model[0]];

	std::string s;
	s += config().decision;
	s += "(";
	for(int i = 0; i < config().model_c; i++)
	{
	    if(i != 0)
		s += " ";
	    s += model_names[config().model[i]];
	}
	s += ")";
	strcpy(buf, s.c_str());

	return buf;
    }

    int decision(int results[MAX_MODELS])
    {
	if(strcmp(config().decision, "maj") == 0)
	{
	    int index[MAX_MODELS];  // error index in sentence
	    int count[MAX_MODELS];  // count of each index
	    int max_index = 0;	    // count of different indexes
	    int i;

	    // count indexes
	    for(i = 0; i < config().model_c; i++)
	    {
		int j;
		for(j = 0; j < max_index; j++)
		    if(index[j] == results[i])
			break;
		if(j == max_index)  // not found, add new
		{
		    index[max_index] = results[i];
		    count[max_index] = 1;
		    max_index++;
		}
		else		    // found, increase count
		    count[j]++;
	    }

	    // index may belong to same trigram as other index
	    int triples[MAX_MODELS];
	    for(i = 0; i < max_index; i++)
	    {
		triples[i] = count[i];
		for(int j = 0; j < max_index; j++)
		{
		    if(index[i] == index[j] + 1)
		       triples[i] += count[j];
		    if(index[i] == index[j] - 1)
		       triples[i] += count[j];
		}
	    }

	    // find majority decision
	    int best = 0;
	    for(i = 1; i < max_index; i++)
		if(triples[i] > triples[best] ||
		   (triples[i] == triples[best] &&
		    count[i] > count[best]))
		    best = i;

	    // no alarm if all different
	    if(triples[best] == 1)
		return PROBCHECK_OK;
	    else
		return results[best];
	}
	else if(strcmp(config().decision, "none") == 0)
	    return results[0];
	else
	{
	    std::cout << "ERROR: illegal decision type in cluster flag -J";
	    throw "ERROR: illegal decision type in cluster flag -J";
	}
    }

    template<class T>
    T max(T t1, T t2)
    {
	return t2 > t1 ? t2 : t1;
    }

    double apply_g(double d)
    {
	int g = config().g_no;
	double ret = 0;
	switch(g)
	{
	case 0:	    ret = 1 / (1 + config().g_coeff * d); break;
	case 1:	    ret = 1 / (1 + max(0.0, d - config().g_coeff)); break;
	case 2:	    ret = max(0.0, 1 - config().g_coeff * d); break;
	case 3:	    ret = exp(-config().g_coeff * d);
	}

	return ret;
    }

    Config &config()
    {
	static Config config;
	return config;
    }

    void reset()
    {
#ifdef VERBOSE
	Message(MSG_STATUS, "resetting probchecker...");
#endif
	for(unsigned int i = 0; i < type_count + 1; i++)
	{
	    if(i < type_count)
	    {
		type_c[i] = 0;
		type_tot[i] = 0;
	    }	
	    for(int j = 0; j < MAX_BLOCK; j++)
		rules_used_count[i][j] = 0;

	    rules_used[i].clear();
	}

	found_by_prob.clear();
	found_by_scrut.clear();
	rules_scrut.clear();
	rules_avail.clear();
	not_annot.clear();
    
	sentence_count = 0;
	sentence_offset = -1;

	// report_reset();
    }

    void print(const Scrutinizer *s)
    { // This comment ?? Jonas
      // Without this comment, Wille's test fails
      // With this comment, tags do not get printed in normal runs

      /*
#ifdef DEVELOPER_OUTPUT
#if 0
      output_new_annotations();
#endif
#endif // DEVELOPER_OUTPUT
      output_tag_list(s);
      output_granska_rules(); // This call gives different results on the same input
      */
#ifndef DEVELOPER_OUTPUT
      output_tag_list(s);
#endif
      
      reset();
    }

    void unload()
    {
    }

    void init_probcheck(const Prob::Config &c)
    {
	annot_file  = c.annot_file;
	output_file = 0;
	//out->file(output_file);

	// init statistics
	using namespace Prob;

	init_annot(annots, annot_file ? annot_file : "");

    
	// init types
	unsigned int i;
	for(i = 0; i < type_count; i++)
	    type_c[i] = type_tot[i] = 0;

	type[0] = 0;	// unknown
	flag[0] = F_NORMAL;
	type[1] = SPELL | SEM_GRAM;
	flag[1] = F_NORMAL;
	type[2] = SPELL | VERB;
	flag[2] = F_NORMAL;
	type[3] = SPELL | COMPOUND;
	flag[3] = F_NORMAL;
	type[4] = SPELL;
	flag[4] = F_NORMAL;
	type[5] = STYLE;
	flag[5] = F_NORMAL;
	type[6] = MISSING_WORD;
	flag[6] = F_NOSTYLE;
	type[7] = WORD_ORDER;
	flag[7] = F_NOSTYLE;
	type[8] = FOREIGN;
	flag[8] = F_NORMAL;
	type[9] = ERR_TOK_SENT;
	flag[9] = F_NORMAL;
	type[10] = BAD_TAG;
	flag[10] = F_NORMAL;

	for(i = 0; i < type_count + 1; i++)
	    for(int j = 0; j < MAX_BLOCK; j++)
		rules_used_count[i][j] = 0;
	tags_in_false_alarms.resize(MAX_TAGS);
    }

    Tag *get_tag(const Info &info, const Matching *p)
    {
	ChangeableTag *cn =
	    p->GetElementMatching(p->NElements() - 1).GetHelpRuleTag();
	if(!cn)
	{
	    out->add("warning", "hittade inga särdrag");
	    return 0;
	}

	ChangeableTag *c = cn;
	std::string tags;
	for(int k = 0; k < 20; k++)
	    if(c->featureValue[k])
	    {
		tags += tags.empty() ? "" : ".";
		tags += info.l->GetFeature(c->featureValue[k]).Name();
	    }
	out->attr("features", tags.c_str());

	int n = 0;
	Tag *t = info.l->FindTag(tags.c_str());   // first, try the concatenatation
	if(!t)
	{
	    out->add("warning", "särdragen ger ingen giltig tag");
	    return 0;

	    // jb: sometimes nn.utr.sin.def.nom => nn.utr.sin.def.nom.dat!
	    t = c->FindMatchingTag(n);  
	    if(!t)
	    {
		out->add("warning", "särdragen ger ingen giltig tag "
			"(efter FindMatchingTag)");
		return 0;
	    }
	}
	return t;
    }

    void
    get_intervals(const std::vector<int> 	      &errs,
		  std::vector<std::pair<int, int> >   &intv)
    {
	intv.clear();
	if(errs.empty())
	    return;

	if(config().use_span)
	{
	    // get continuous intervals from error vector, e.g. (3 4 5) (7) (10 11) 
	    int last = 0;
	    for(unsigned int i = 0; i < errs.size(); i++)
		if(i == errs.size() - 1 || 
		   errs[i + 1] != errs[i] + 1)
		{
		    intv.push_back(std::make_pair(errs[last], errs[i]));
		    last = i + 1;
		}
	}
	else
	{
	    for(unsigned int i = 0; i < errs.size(); i++)
		intv.push_back(std::make_pair(errs[i], errs[i]));
	}
    }

} // end namespace


#endif // ifdef PROBCHECK (topmost)
