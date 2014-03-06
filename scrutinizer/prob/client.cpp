#include "probadmin.h"

#ifdef USE_AS_CLIENT
#include <sstream>
#include "scrutinizer.h"
#include "config.h"

Prob::Client::Client(std::string name, Scrutinizer &s, Prob::Config &c)
    : Pipe_client(name), scrut(s), config(c), calc(false)
{}

Prob::Client::~Client()
{}


bool Prob::Client::received(std::string s)
{
    if(s == "done")
    {
	std::cerr << "done" << std::endl;
	calc = false;
	return true;
    }
    else
    {
	std::cerr << "got '" << s << "'" << std::endl;
	std::istringstream is(s);

	is >> config.thresh[0];
	for(int i = 0; i < BEST_COUNT; i++)
	    is >> config.weights[i];

	int n = 0;
	scrut.Scrutinize(&n);
	scrut.PrintResult();

	calc = true;
	return true;
    }

}

std::string Prob::Client::send()
{
    if(!calc)
    {
	std::cerr << "sending 'start'" << std::endl;
	return "start";
    }
    else
    {
	std::ostringstream o;
	
	for(int i = 0; i < 6; i++)
	    o << config.output[i] << " ";

	std::string s = o.str();
	std::cerr << "sending '" << s << "'" << std::endl;
	return s;
    }
}

#endif // USE_AS_SERVER
