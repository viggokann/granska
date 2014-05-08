/* $Id: socket.h,v 1.2 2002/03/04 15:52:50 jfc Exp $
 * Copyright 2001 Euroling AB
 * Provides an easy way of using sockets. After connection the the operators
 * << and >> can be used to write and read using the socket.
 * Author: Mikael Hallendal
 */

#ifndef _SOCKET_HH
#define _SOCKET_HH

#include <arpa/inet.h>
#include <iostream>
#include <streambuf>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>

enum SOCKET_STATUS {
  SOCKET_INITIATED,
  SOCKET_CONNECTED,
  SOCKET_CLOSED,
  SOCKET_ERROR,
  SOCKET_UNKNOWN_ADDR
};

class Socket : public std::streambuf, public std::iostream {
public:
  Socket(); 
  Socket(const char *addr, unsigned short port);
  Socket(Socket&); // no-no
  ~Socket();
  bool Connect();
  bool Disconnect();
  void Close();
  void SetKeepAlive(bool ka);
  /* -- Set the underlying socket. Used by ServerSocket::Accept()  -- */
  void Set(int socket, struct sockaddr_in *client_info);
  /* -- Retreive information about the socket                      -- */
  const char* GetHostName() const { return _host_name; };
  const char* GetHostAddr() const;
  const ushort GetRemotePort() const { return r_port_; };
  void Print(std::ostream& os) const;
  /* -- Used for the streaming functionality -- */
protected:
  int sync();
  int underflow();
  int overflow(int ch); 
protected:
  bool CreateSocket(const char *addr, const unsigned short port);
  void ResetBuf();
private:
  bool InitBuf(int size);
protected:
  int status_;
  struct sockaddr_in dest_addr_;
private:
  static const int SOCK_BUFSIZE = 8192;
  char _host_name[1024];
  ushort r_port_;
  int socket_;
  char *gbuf, *pbuf;
  int bufsize;
};

inline void Socket::ResetBuf() { clear(); }

inline int Socket::sync() {
  overflow(EOF);
  return 0;
}

inline void Socket::Print(std::ostream& os) const { os << _host_name << ":" << r_port_; }

inline std::ostream& operator<<(std::ostream& os, const Socket *s) {
  if (s) s->Print(os); else os << "Socket is null" << std::endl;
  return os;
}
inline std::ostream& operator<<(std::ostream& os, const Socket &s) {
  s.Print(os);
  return os;
}

#endif
