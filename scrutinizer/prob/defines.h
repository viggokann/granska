
#ifdef DEVELOP_JOHNNY


/*
 *   first, undefine all possible defines to please 
 *   the VC++ precompiled header handler
 *
 */
#undef USE_AS_CLIENT
#undef TRY_PAROLE_TTT
#undef TRY_CONTEXT_REPRESENTATIVES
#undef IGNORE_TAG_FEATURES
#undef IMPROVE_TAGGER
#undef ONLY_ADDITIONAL_RULES
#undef DEVELOPER_OUTPUT
#undef DONT_USE_RULES
#undef NO_SENTENCE_DELIMITERS


//#define USE_AS_CLIENT			// get parameters from a server
//#define TRY_PAROLE_TTT			// try use the Parole trigrams
//#define TRY_CONTEXT_REPRESENTATIVES	// use context to choose representatives
//#define IGNORE_TAG_FEATURES		// context uses only POS, not features
//#define IMPROVE_TAGGER	    // replace phrases => better scope for tagger
//#define ONLY_ADDITIONAL_RULES	    // don't use granska's rules, just the hard-coded ones
//#define DONT_USE_RULES  	    // no rules whatsoever
#define DEVELOPER_OUTPUT	    // verbose output
//#define NO_SENTENCE_DELIMITERS	    // don't include delimiters surrounding the sentences: $. $. Hej hopp $. $.
//#define NO_MULTIPLE_DETECTS	    // count only one detection per sentence

#endif // DEVELOP_JOHNNY
