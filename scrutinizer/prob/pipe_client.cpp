#include <windows.h> 
#include <stdio.h>
#include "pipe_client.h"


static void send_receive(const char                 *str,
                         char                       *buf,
                         int                         len,
                         Misc::Pipe_client_impl     &impl);
static DWORD run(const char                 *name,
                 Misc::Pipe_client_impl     &impl);


class Misc::Pipe_client_impl
{
public:
    Pipe_client_impl(std::string n, Misc::Pipe_client &s)
	: client(s)
    {
	hPipe = 0;
	name = "\\\\.\\pipe\\" + n;
    }
    void run()
    {
	::run(name.c_str(), *this);
    }
    
public:
    HANDLE		hPipe;
    
    Misc::Pipe_client  &client;
    std::string         name;
};

Misc::Pipe_client::Pipe_client(std::string name)
{
    impl = new Pipe_client_impl(name, *this);
}

Misc::Pipe_client::~Pipe_client()
{
    delete impl;
}

void Misc::Pipe_client::run()
{
    impl->run();
}




static DWORD run(const char *name, Misc::Pipe_client_impl &impl) 
{ 
    LPTSTR lpszPipename = (char *)name;

    // Try to open a named pipe; wait for it, if necessary. 
    while (1) 
    { 
	impl.hPipe = CreateFile( 
	    lpszPipename,   // pipe name 
	    GENERIC_READ |  // read and write access 
	    GENERIC_WRITE, 
	    0,              // no sharing 
	    NULL,           // no security attributes
	    OPEN_EXISTING,  // opens existing pipe 
	    0,              // default attributes 
	    NULL);          // no template file 
	
	// Break if the pipe handle is valid. 
	
	if (impl.hPipe != INVALID_HANDLE_VALUE) 
	    break;
	
	// Exit if an error other than ERROR_PIPE_BUSY occurs. 
	
	if (GetLastError() != ERROR_PIPE_BUSY) 
	    throw "Could not open pipe, pipe busy";
	
	// All pipe instances are busy, so wait for 20 seconds. 
	
	if (! WaitNamedPipe(lpszPipename, 20000) ) 
	    throw "Could not open pipe, timed out";
    } 
    

    while(1)
    {
	std::string s = impl.client.send();

	char param[1024];
	send_receive(s.c_str(), param, sizeof param, impl);
    
	if(!impl.client.received(param))
	    break;
    }
    
    CloseHandle(impl.hPipe);
    
    return 0;
} 


static void send_receive(const char                 *str,
                         char                       *buf,
                         int                         len,
                         Misc::Pipe_client_impl     &impl)
{
    LPVOID lpvMessage;
    CHAR chBuf[512];
    BOOL fSuccess;
    DWORD cbRead, cbWritten, dwMode;
    
    // The pipe connected; change to message-read mode. 
    
    dwMode = PIPE_READMODE_MESSAGE;
    fSuccess = SetNamedPipeHandleState( 
	impl.hPipe,    // pipe handle 
	&dwMode,  // new pipe mode 
	NULL,     // don't set maximum bytes 
	NULL);    // don't set maximum time 
    if (!fSuccess) 
	throw ("SetNamedPipeHandleState");
    
    // Send a message to the pipe server. 
    
    lpvMessage = (char *)str;
    
    fSuccess = WriteFile( 
	impl.hPipe,                  // pipe handle 
	lpvMessage,             // message 
	strlen((const char *)lpvMessage) + 1, // message length 
	&cbWritten,             // bytes written 
	NULL);                  // not overlapped 
    if (! fSuccess) 
	throw ("WriteFile");
    
    do 
    { 
	// Read from the pipe. 
	
	fSuccess = ReadFile( 
	    impl.hPipe,    // pipe handle 
	    chBuf,    // buffer to receive reply 
	    512,      // size of buffer 
	    &cbRead,  // number of bytes read 
	    NULL);    // not overlapped 
	
	if (! fSuccess && GetLastError() != ERROR_MORE_DATA) 
	    break;
	
	strncpy(buf, chBuf, len);
	break;
	
    } while (! fSuccess);  // repeat loop if ERROR_MORE_DATA 
} 
