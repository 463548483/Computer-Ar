#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include "function.h"

using namespace std;


int init_client(const char * hostname, const char * port) {
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo * host_info_list;

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  }  //if

  socket_fd = socket(host_info_list->ai_family,
                     host_info_list->ai_socktype,
                     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);;
  }  //if

  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;

  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);;
  }  //if

  // const char * message = "hi there!";
  // send(socket_fd, message, strlen(message), 0);

  freeaddrinfo(host_info_list);
  // close(socket_fd);

  return socket_fd;
}

int init_server(const char * port){
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = NULL;

  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags    = AI_PASSIVE;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);;
  } 

  if (port==NULL){
    struct sockaddr_in * addrinfo=(struct sockaddr_in *)(host_info_list->ai_addr);
    addrinfo->sin_port=0;
  }

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);;
  } //if

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);;
  } //if

  status = listen(socket_fd, 100);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl; 
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);;
  } 

  cout << "Waiting for connection on port " << port << endl;
  // struct sockaddr_storage socket_addr;
  // socklen_t socket_addr_len = sizeof(socket_addr);
  // int client_connection_fd;
  // client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  // if (client_connection_fd == -1) {
  //   cerr << "Error: cannot accept connection on socket" << endl;
  //   exit(EXIT_FAILURE);;
  // } //if

  // char buffer[512];
  // recv(client_connection_fd, buffer, 9, 0);
  // buffer[9] = 0;

  // cout << "Server received: " << buffer << endl;

  freeaddrinfo(host_info_list);
  // close(socket_fd);

  return socket_fd;
}



// int clientecho(char * host, char * port){
//   int clientfd;
//   char buf[MAXLINE];
//   rio_t rio;

//   clientfd = Open_clientfd(host, port);
//   Rio_readinitb(&rio, clientfd);

//   while (Fgets(buf, MAXLINE, stdin) != NULL) {
//   Rio_writen(clientfd, buf, strlen(buf));
//   Rio_readlineb(&rio, buf, MAXLINE);
//   Fputs(buf, stdout);
//   }
//   //close(clientfd);
//   exit(0);
// }

int server_send(int listenfd, char (& client_hostname)[MAXLINE], char (& client_port)[MAXLINE]){
  socklen_t clientlen;
  struct sockaddr_storage clientaddr; /* Enough space for any address */
  //char client_hostname[MAXLINE], client_port[MAXLINE];

  clientlen = sizeof(struct sockaddr_storage);
  int connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
  if (connfd == -1) {
    cerr << "Error: cannot accept connection on socket" << endl;
    exit(EXIT_FAILURE);
  }

  char host[MAXLINE];
  char service[MAXLINE];

  getnameinfo((struct sockaddr *) &clientaddr, clientlen, host, MAXLINE,
  service, MAXLINE, 0);

  memcpy(client_hostname,host,MAXLINE);
  memcpy(client_port,service,MAXLINE);
  
  return connfd;

  
}