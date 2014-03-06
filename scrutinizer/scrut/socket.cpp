/* $Id: socket.cpp,v 1.2 2002/03/04 15:52:50 jfc Exp $
 * Copyright 2001 Euroling AB
 * Author: Mikael Hallendal
 */

#include <netdb.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#include "socket.h"

Socket::Socket() :
  std::streambuf (),
  std::iostream ((std::streambuf *) this),
  socket_(-1),
  gbuf(NULL),
  pbuf(NULL) {
  gbuf = pbuf = NULL;
  InitBuf (SOCK_BUFSIZE);
} 

Socket::Socket (const char *addr, ushort port) :
  std::streambuf (),
  std::iostream ((std::streambuf *) this) {
  gbuf = pbuf = NULL;
  strncpy (_host_name, addr, 1023);
  r_port_ = port;
  CreateSocket (addr, port);
  InitBuf (SOCK_BUFSIZE);
}

bool Socket::Connect () {
  /* -- Check to see if the socket has been initiated                       -- */
  if (status_ == SOCKET_INITIATED) {
    if (connect (socket_, (struct sockaddr *)&dest_addr_, 
                 sizeof (struct sockaddr)) == -1) {
      
      /* -- Couldn't connect. Set status to SOCKET_ERROR                    -- */
      status_ = SOCKET_ERROR;
      return false;
    }
  }
  else {
    if (status_ == SOCKET_CONNECTED)
      return true;
    if (status_ == SOCKET_ERROR || socket_ == SOCKET_UNKNOWN_ADDR)
      return false;
  }
  status_ = SOCKET_CONNECTED;
  return true;
}

bool Socket::Disconnect () {
  /* -- FIX: Disconnect and reset the socket...                             -- */
  ::shutdown (socket_, 2);

  status_ = SOCKET_INITIATED;
  return true;
}

void Socket::Close () {
  if (socket_ >= 0) ::close (socket_);
  socket_ = -1;
  status_ = SOCKET_CLOSED;
}

void Socket::Set (int socket, struct sockaddr_in *client_info) {
  r_port_ = ntohs (client_info->sin_port);
  //   m_verbose << "Setting incoming..." << std::endl;
  struct hostent *c_host = gethostbyaddr ((char *)&(client_info->sin_addr),
                                          sizeof client_info->sin_addr,
                                          AF_INET);
  if (c_host) {
    strcpy (_host_name, c_host->h_name);
  }
  socket_ = socket;
}

const char* Socket::GetHostAddr () const {
  struct hostent *host = gethostbyname (_host_name);
  if (host)
    return inet_ntoa (*(struct in_addr *)host->h_addr);
  return NULL;
}

void Socket::SetKeepAlive (bool ka) {
  int opt = 1;
  if (!ka)
    opt = 0;
  /* -- Set the keepalive option                                            -- */
  setsockopt (socket_, SOL_SOCKET, SO_KEEPALIVE, 
              (char *)&opt, (size_t)sizeof (opt));
}

bool Socket::CreateSocket (const char *addr, const ushort port) {
  struct hostent *l_addr = gethostbyname (addr);
  if (l_addr == NULL) {
    status_ = SOCKET_UNKNOWN_ADDR;
    return false;
  }

  socket_ = socket (AF_INET, SOCK_STREAM, 0);
  if (socket_ == -1) {
    std::cerr << "Couldn't create socket..." << std::endl;
    status_ = SOCKET_ERROR;
    return false;
  }
  /* -- Used when Connection is called. Holds information about which host  -- */
  /* -- and port to connect to.                                             -- */
  dest_addr_.sin_family = AF_INET;
  dest_addr_.sin_port = htons (port);
  dest_addr_.sin_addr = *((struct in_addr *)l_addr->h_addr);
  bzero (&(dest_addr_.sin_zero), 8);
  status_ = SOCKET_INITIATED;
  return true;
}

bool Socket::InitBuf (int size) {
  if (gbuf != NULL) 
    delete gbuf;
  if (pbuf != NULL) 
    delete pbuf;
  gbuf = new char[size]; // new OK
  pbuf = new char[size]; // new OK
  if (!gbuf || !pbuf) {
    std::cerr << "Couldn't allocate memory for get- and putbuffers" << std::endl;
    return false;
  }
  bufsize = size;
  /* -- Tells the streambuf to use the newly created buffers                -- */
  /* -- Set the getarea                                                     -- */
  setg (gbuf, gbuf + bufsize, gbuf + bufsize);
  /* -- Set the putarea                                                     -- */
  setp (pbuf, pbuf + bufsize);
  return true;
}

int Socket::underflow () {
  /* -- FIX: Learn what this is doing...                                    -- */
  //  m_verbose << "Socket underflow ()" << std::endl;
  int r_len;
  if (bufsize == 1) {
    unsigned char ch;
    /* -- Receive one byte from the stream                                  -- */
    r_len = ::recv (socket_, (char *)&ch, 1, 0);
    if (r_len < 0) {
      std::cerr  << "Error while reading a char from socket" << std::endl;
      return EOF;
    }
    return ch;
  }
  if (!gptr ()) return EOF;
  if (gptr () < egptr ())
    return (unsigned char) *gptr ();
  r_len = (gbuf + bufsize) - eback ();
  r_len = ::recv (socket_, (char *)eback (), r_len, 0);
  if (r_len < 1) {
    if (r_len < 0) 
      std::cerr << "Error while reading from socket" << std::endl;
    return EOF;
  }
  setg (eback (), eback (), eback () + r_len);
  return (unsigned char) *gptr ();
}

int Socket::overflow (int ch) {
  int buf_len;
  int send_len = 0;
  int next_send = 0;
  if (bufsize == 1) {
    if (ch != EOF) {
      ch = (unsigned char)ch;
      /* -- Send one char                                                   -- */
      ::send (socket_, (char *) ch, 1, 0);
    }
  }
  buf_len = pptr () - pbase ();
  /* -- Loop until the entire buffer is sent                                -- */
  while (buf_len) {
    send_len = ::send (socket_, (char *)(pbase () + next_send), buf_len, 0);
    if (send_len < -1) {
      std::cerr << "Error while writing to socket" << std::endl;
      return EOF;
    }
    next_send += send_len;
    buf_len -= send_len;
  }
  /* -- Move the pointer to the first character not sent.                   -- */
  setp (pbuf, pbuf + bufsize);
  if (ch != EOF) {
    *pptr () = (unsigned char) ch;
    pbump (1);
  }
  return ch;
}

Socket::~Socket() {
  this->sync ();
  if (gbuf) delete gbuf;
  if (pbuf) delete pbuf;
  if (socket_ >= 0) {
    shutdown(socket_, 2);
    close(socket_);
  }
}

