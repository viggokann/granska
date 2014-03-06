#ifdef TRANSITIVITY

#include "trans.h"
#pragma warning(disable: 4786)
#include <map>
#include <string>
#include <iostream>
#include <fstream>


namespace Trans
{
    typedef std::map<std::string, trans_t> trans_map;

    static trans_map trans;
}


void
Trans::load()
{
    char *tagger_lex = getenv("TAGGER_LEXICON");
    std::string file = tagger_lex ? tagger_lex : "";	
    file += "words/transitivitet";
    std::ifstream f(file.c_str());

    if(!f)
    {
	std::cout << "transitivity: could not open file '" 
		  << file << "'" << std::endl;
	//throw "file error";
	exit(1);
    }

    // read words
    std::string	    str;
    int		    tr;
    while(f)
    {
	f >> str;
	f >> tr;
	if(tr == 0)
	{
	    std::cout << "transitivity: read error! (near "
		      << str << ")" << std::endl;
	    exit(1);
	}
	trans.insert(trans_map::value_type(str, trans_t(tr)));
    }
}

Trans::trans_t
Trans::lookup(const char *vb)
{
    trans_map::const_iterator it = trans.find(vb);
    if(it == trans.end())
	return T_NONE;
    else
	return it->second;
}


#endif // ifdef TRANSITIVITY (topmost)
