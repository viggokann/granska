#ifndef PROB_DEFINES_H
#define PROB_DEFINES_H


namespace Prob
{
    const int       BEST_COUNT = 10;
    const int	    MAX_MODELS = 10;
    const double    BIG_NUM = 999;
    const double    EPSILON_NUM = 0.001;

    const int MAX_MATCHINGS	= 32;	// max matchings covering a single word
    const int PROXIMITY		= 1;	// distance to matching to be included
    const int WINDOW_SIZE	= 3;	// 3 = trigram
#ifdef IMPROVE_TAGGER
    const int MAX_BLOCK	        = MAX_SENTENCE_LENGTH;
#else
    const int MAX_BLOCK	        = 10;
#endif
}

#endif /* PROB_DEFINES_H */



