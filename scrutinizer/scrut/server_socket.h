/* $Id: server_socket.h,v 1.1 2002/02/26 16:09:53 jfc Exp $
 * Copyright 2001 Euroling AB
 * A socket for listening for incoming connections.
 * Call accept with an empty Socket which returns when a client connects.
 * The socket reference is then used to communicate with the client.
 * Author: Mikael Hallendal
 */

#ifndef SERVER_SOCKET_HH
#define SERVER_SOCKET_HH

#include "socket.h"

class ServerSocket {
public:
  ServerSocket(unsigned short port, unsigned short max_queue = DEFAULT_QUEUE); 
  ServerSocket(const ServerSocket &old_ServerSocket); // not allowed
  ~ServerSocket();
  bool IsOK() const { return socket_ >= 0 && ok_; }
  bool Accept(Socket &socket);
  void Close();
private:
  static const int MAXMSG = 512;
  static const int DEFAULT_QUEUE = 20;
  bool Create_socket(long addr, unsigned short port, unsigned short max_queue);
  int socket_;
  bool ok_;
};

#endif
