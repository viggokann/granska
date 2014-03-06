#include "probadmin.h"

#ifdef TRY_PAROLE_TTT

#include "taglexicon.h"
#include <string>
#include <iomanip>
#include <fstream>
#include "trigrams.h"

namespace Prob
{
    Trigrams::Trigrams(const TagLexicon &t) : l(t) 
    {
	ttt = new type[150];
    }
    Trigrams::~Trigrams()
    {
	delete [] ttt;
    }
    void Trigrams::load(std::string file)
    {
        char *tagger_lex = getenv("TAGGER_LEXICON");
	std::string fs = tagger_lex ? tagger_lex : "";	
	fs += "tags/";
	fs += file;
	std::ifstream f(fs.c_str());

	if(!f)
	{
	    std::cout << "cluster: could not open trigram file '" 
		      << fs << "'" << std::endl;
	    //throw "file error";
	    exit(1);
	}
	
	for(int i = 0; i < 150; i++)
	    for(int j = 0; j < 150; j++)
		for(int k = 0; k < 150; k++)
		    ttt[i][j][k] = 0;

	std::cerr << "loading trigrams..." << std::endl; 
	int count = 0;
	while(f)
	{
	    int freq;
	    f >> freq;

    	    unsigned int t1, t2, t3;
	    f >> t1 >> t2 >> t3;

	    if(t1 > 150 || t2 > 150 || t3 > 150 ||
	       t1 <= 0 || t2 <= 0 || t3 <= 0 ||
	       freq <= 0)
	    {
		std::cout << "cluster: read error in trigram file, row "
			  << count + 1 << std::endl;
		exit(1);
	    }
	    ttt[t1 - 1][t2 - 1][t3 - 1] = freq;
	    //std::cerr << freq << " " << t1 << " " << t2 << " " << t3 << " " << std::endl;
	    count++;
	}
	std::cerr << "read " << count << " trigrams..." << std::endl; 

#if 0
	// create .tt and .t files
	{
	    std::ofstream out_tt("out.tt");
	    std::ofstream out_t("out.t");
	    int data_tt[149][149];
	    int data_t[149];
	    int tot_tt = 0;
	    int tot_t = 0;
	    for(int i = 0; i < 149; i++)
	    {
		data_t[i] = 0;
		for(int j = 0; j < 149; j++)
		{
		    data_tt[i][j] = 0;
		    for(int k = 0; k < 149; k++)
		    {
			data_tt[i][j] += ttt[i][j][k];
			data_t[i] += ttt[i][j][k];
		    }
		    out_tt << std::setw(10) << data_tt[i][j] 
			   << "\t" << i << "\t" << j << std::endl;
		    tot_tt += data_tt[i][j];
		}
		out_t << std::setw(10) << data_t[i] 
		      << "\t" << i << std::endl;
		tot_t += data_t[i];
	    }
	    std::cerr << "tot_tt = " << tot_tt << std::endl;
	    std::cerr << "tot_t = " << tot_t << std::endl;
	}
#endif
    }
    int Trigrams::ttt_freq(unsigned char t1, unsigned char t2, unsigned char t3) const
    {
	if(t1 == 22 || t1 == 41 || t1 == 52 || t1 == 99)
	    t1 = 143;
	if(t2 == 22 || t2 == 41 || t2 == 52 || t2 == 99)
	    t2 = 143;
	if(t3 == 22 || t3 == 41 || t3 == 52 || t3 == 99)
	    t3 = 143;

	return ttt[t1][t2][t3];
    }
    int Trigrams::tt_freq(unsigned char t1, unsigned char t2) const
    {
	return l.tt_freq(t1, t2);
    }
    int Trigrams::t_freq(unsigned char t1) const
    {
	return l.t_freq(t1);
    }

}


#endif // TRY_PAROLE_TTT