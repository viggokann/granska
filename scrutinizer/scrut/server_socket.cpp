/* $Id: server_socket.cpp,v 1.4 2005/12/30 18:59:39 me00_tcr Exp $
 * Copyright 2001 Euroling AB
 * Author: Mikael Hallendal
 */

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "server_socket.h"

ServerSocket::ServerSocket(unsigned short port,
                            unsigned short max_queue) :
  socket_(-1),
  ok_(false) {
  Create_socket(htonl(INADDR_ANY), port, max_queue);
}

ServerSocket::~ServerSocket() {}

bool ServerSocket::Accept(Socket &socket) {
  int client_socket;
  struct sockaddr_in client_info;

  socklen_t size = sizeof(client_info);

  client_socket = ::accept(socket_, (struct sockaddr *) &client_info, &size);
  /*  
  struct hostent *c_host = gethostbyaddr((char *)&client_info.sin_addr,
                                          sizeof client_info.sin_addr,
                                          AF_INET);

  if (c_host) {
  cerr << "Serverconnect from: " << c_host->h_name
  << ":" << ntohs(client_info.sin_port) << std::endl;
  }
  */
  socket.Set(client_socket, &client_info);
  return true;
}

void ServerSocket::Close() {
  ::shutdown(socket_, 2);
  socket_ = 0;
}

bool ServerSocket::Create_socket(long addr, unsigned short port, 
				  unsigned short max_queue) {
  struct sockaddr_in name;
  socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_ < 0) {
    std::cerr << "Socket creation failed" << std::endl;
    return (ok_ = false);
  }
  name.sin_family = AF_INET;
  name.sin_port = htons(port);
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  bzero(&(name.sin_zero), 8);
  int opt = 1;
  struct linger linger;
  linger.l_onoff = linger.l_linger = 0;
  setsockopt(socket_, SOL_SOCKET, SO_LINGER, (char *)&linger, (size_t)sizeof (linger));
  setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, (size_t)sizeof(opt));
  if (bind(socket_, (struct sockaddr *) &name, sizeof(name)) < 0) {
    std::cerr << "Binding failed" << std::endl;
    return (ok_ = false);
  }
  // Currently only one connection at a time
  if (listen(socket_, 1) < 0) {
    std::cerr << "Listen failed" << std::endl;
    return (ok_ = false);
  } 
  //  std::cerr << "Server socket connected on port: " << port << std::endl;
  return (ok_ = true);
}
