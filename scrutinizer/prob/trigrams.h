#ifndef CLUSTER_TRIGRAMS_H
#define CLUSTER_TRIGRAMS_H


class TagLexicon;

namespace Prob
{
    // class for external trigrams, such as Parole
    class Trigrams
    {
    public:
	Trigrams(const TagLexicon &);
	~Trigrams();
	void load(std::string file);
	int ttt_freq(unsigned char t1, unsigned char t2, unsigned char t3) const;
	int tt_freq(unsigned char t1, unsigned char t2) const;
	int t_freq(unsigned char t1) const;

    protected:
	typedef int type[150][150];
	type *ttt;
	const TagLexicon &l;
	int index[150];
    };

}


#endif /* CLUSTER_TRIGRAMS_H */