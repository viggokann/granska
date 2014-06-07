#ifndef OUTPUT_H
#define OUTPUT_H


#include <iosfwd>
#include "misc/xmloutput.h"


namespace Prob
{
    typedef Misc::XMLoutput Output;
	
    Output &output(const char *name = 0);
    Output &no_output(const char *name = 0);
    Output &output(std::ostream &);
    Output &no_output(std::ostream &);
}


#endif /* OUTPUT_H */
