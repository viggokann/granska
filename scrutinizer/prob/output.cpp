#include "misc/xmloutput.h"
#include "output.h"
#include <sstream>
#include "defines.h"


#pragma warning(disable: 4786)
#include <fstream>
#include <iostream>



#ifndef NO_XML_SUPPORT
#include <xercesc/util/PlatformUtils.hpp>

using namespace xercesc;

class XML_handler
{
public:
    XML_handler()
    {
	init();
    }
    ~XML_handler() 
    {
	exit();
    }
    static void init()
    {
	if(inited)
	    return;

	try
	{
	    XMLPlatformUtils::Initialize();
	}
	catch(...)
	{
	    throw "error in XML4C initialization";
	}
	inited = true;
    }
    static void exit()
    {
	if(exited)
	    return; 

	try
	{
	    XMLPlatformUtils::Terminate();
	}
	catch(...)
	{
	    throw "error in XML4C termination";
	}
	exited = true;
    }
    static bool inited;
    static bool exited;
};

bool XML_handler::inited = false;
bool XML_handler::exited = false;
static XML_handler the_xml_handler;

#endif // !NO_XML_SUPPORT



static Prob::Output out;
static Prob::Output no_out(true);


Prob::Output &Prob::output(const char *name)
{
    if(name)
    	out.file(name);

    return out;
}

Prob::Output &Prob::no_output(const char *name)
{
    return no_out;
}

Prob::Output &Prob::output(std::ostream &o)
{
    out.stream(o);

    return out;
}

Prob::Output &Prob::no_output(std::ostream &)
{
    return no_out;
}
