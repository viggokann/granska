#ifndef ANNOT_H
#define ANNOT_H

#include <map>
#include <string>
#include <vector>


class AbstractSentence;

namespace Prob
{
    struct Annot
	{
		int	    value[3];
		int	    type[3];
		int	    count;
    };
    
    struct New_annot
    {	
		int offset;
		int from;
		int to;
		std::string text;
		std::string comment;
    };


    typedef std::map<int, Annot> annot_map; 
    typedef std::map<int, Annot>::const_iterator annot_iter; 
    
    extern annot_map annots;
    extern std::vector<New_annot> not_annot;

    enum 
	{
		PROBCHECK_OK = -11,
		FALSE_ALARM = -12,
		NO_CORRECTION_FOUND = -13
    };
    enum annot_t { 
		   ERR_TYPE         = 0x0000,
	       NO_TYPE			= 0x0001,
		   PH_INTERPOSED	= 0x0002,
		   HARD_TTT			= 0x0004,
		   ERR_TOK_SENT		= 0x0008,
		   BAD_TAG			= 0x0010,
		   FOREIGN			= 0x0020,
		   STYLE			= 0x0040,
		   BAD_PN			= 0x0080,
		   VERB				= 0x0100,
		   SPELL			= 0x0200,
		   SEM_GRAM			= 0x0400,
		   COMPOUND			= 0x0800,
		   MISSING_WORD		= 0x1000,
		   MISSING_COMMA	= 0x2000,
		   WORD_ORDER		= 0x4000,
		   LOOK_AT			= 0x8000 };

    void init_annot(annot_map &, std::string);

    void not_annotated(int                         from,
                       int                         to,
                       std::string                 comment,
                       const AbstractSentence     *s = 0);
                       
    void output_new_annotations();

}


#endif /* ANNOT_H */
