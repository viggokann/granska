#ifndef TRANSITIVITY_H
#define TRANSITIVITY_H


namespace Trans
{
    enum trans_t { T_NONE		= 0x00,
		   T_INTRANS		= 0x01,
		   T_TRANS		= 0x02,
		   T_BITRANS		= 0x04,
		   T_REFL_INTRANS	= 0x08,
		   T_REFL_TRANS		= 0x10,
		   T_REFL_BITRANS	= 0x20 };

    void	load();
    trans_t	lookup(const char *verb_lemma);
}


#endif /* TRANSITIVITY_H */
