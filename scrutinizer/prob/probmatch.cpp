#pragma warning(disable: 4786)
#include "probmatch.h"
#include "probdevelop.h"
#include "tag.h"
#include <sstream>
#include <cstring>


static bool check_tags(Tag *tag[], int tag_c)
{
    if(tag_c == 0)
	return false;
    for(int i = 0; i < tag_c; i++)
	if(!tag[i])
	    return false;
    return true;
}

Prob::represent_t
Prob::Match::applicable(int            from,
                           int            to,
                           const Tag     *first_tag,
                           bool           silent)
{
    if(type != NOT_APPLICABLE)	// @additional
	return type;

    const char *category = strchr(name, '@');
    if(!category)
    {
	if(!silent)
	    out->add("warning", "regeln har ingen typ");
	return NOT_APPLICABLE;
    }
    category++;	// step past '@'

    if(strcmp(name, "ie@vbrecog") == 0)
    {
	if(!check_tags(tag, tag_c))
	{
	    if(!silent)
		out->add("warning", "tag för ersättning saknas");
	    return NOT_APPLICABLE;
	}
	return REPLACE;
    }
    else if(strcmp(name, "clbegin@clrecog") == 0)
    {
	return NOT_APPLICABLE;
    }
    else if(strcmp(name, "clend@clrecog") == 0)
    {
	return NOT_APPLICABLE;
    }
    else if(strcmp(category, "nprecog") == 0)
    {
	if(!check_tags(tag, tag_c))
	{
	    if(!silent)
		out->add("warning", "tag för ersättning saknas");
	    return NOT_APPLICABLE;
	}
	else if(begin == end &&
		first_tag->String()[0] == tag[0]->String()[0] &&
		first_tag->String()[1] == tag[0]->String()[1])
	{
	    if(!silent)
		out->add("note",
		        "singleton-NP ändrar inte ursprunglig ordklass");
	    return NOT_APPLICABLE;
	}
	return REPLACE;
    }
    else if(strcmp(category, "vbrecog") == 0)
    {
	if(begin == end)
	{
	    if(!silent)
		out->add("note", "singleton-VP ersätts inte");
	    return NOT_APPLICABLE;
	}
	else if(!check_tags(tag, tag_c))
	{
	    if(!silent)
		out->add("warning", "tag för ersättning saknas");
	    return NOT_APPLICABLE;
	}
	return REPLACE;
    }
    else if(strcmp(category, "pprecog") == 0 || 
	    strcmp(category, "abrecog") == 0)
    {
	return REMOVE;
    }
    else if(strcmp(category, "jjrecog") == 0)
    {
#if 0	// leaving the external jj:s gives a lot of false alarms
	if(strcmp(name, "jjexternal@jjrecog") == 0)
	    return NOT_APPLICABLE;  // leave
	else
#endif
	    return REMOVE;
    }
    else if(strcmp(category, "qnrecog") == 0)
    {
	return NOT_APPLICABLE;
    }
    else
    {
	if(!silent)
	{
	    std::ostringstream s;
	    s << "'" << category << "' är en okänd kategori";
	    out->add("warning", s.str().c_str());
	}
	return NOT_APPLICABLE;
    }
}

#ifdef IMPROVE_TAGGER
Prob::represent_t
Prob::Match::tagger_applicable(const Tag *first_tag, bool silent)
{
    if(type != NOT_APPLICABLE)	// @additional
	return type;

    const char *category = strchr(name, '@');
    if(!category)
    {
	if(!silent)
	    out->add("warning", "regeln har ingen typ");
	return NOT_APPLICABLE;
    }
    category++;	// step past '@'

    if(strcmp(category, "pprecog") == 0 || 
       strcmp(category, "abrecog") == 0/* ||
       strcmp(category, "jjrecog") == 0*/)
    {
	return REMOVE;
    }
    else
    {
	return NOT_APPLICABLE;
    }
}
#endif // IMPROVE_TAGGER
