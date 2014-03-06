/*
 *  named pipe client  --  Johnny Bigert 2002-01-15
 *
 *  The client always performs a receive and a send pair,
 *  where the parameters are provided by receive() and
 *  send() respectively.
 *  If received returns false, the process will terminate.
 *
 */
#ifndef MISC_NAMED_PIPE_CLIENT_H
#define MISC_NAMED_PIPE_CLIENT_H

#include <string>


namespace Misc
{
    class Pipe_client;
    class Pipe_client_impl;
}

class Misc::Pipe_client
{
public:
    Pipe_client(std::string pipe_name);
    ~Pipe_client();

    void		    run();
    virtual std::string	    send() = 0;
    virtual bool	    received(std::string s) = 0;

protected:
    Pipe_client_impl *impl;
};



#endif /* MISC_NAMED_PIPE_CLIENT_H */
