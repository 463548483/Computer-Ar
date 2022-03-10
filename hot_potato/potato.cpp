#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include "potato.h"

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

  //cout << "Connecting to " << hostname << " on port " << port << "..." << endl;

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

  if (strcmp(port,"")==0){
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

  // cout << "Server received: " << buffer << endl;

  freeaddrinfo(host_info_list);
  // close(socket_fd);

  return socket_fd;
}


int server_accept(int listenfd, char * client_hostname){
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
  
  return connfd;
  
}



int get_port(int socket_fd) {
  struct sockaddr_in addrinfo;
  socklen_t len = sizeof(addrinfo);
  if (getsockname(socket_fd, (struct sockaddr *)&addrinfo, &len) == -1) {
    cerr << "Error: cannot getsockname" << endl;
    exit(EXIT_FAILURE);
  }
  return ntohs(addrinfo.sin_port);
}

int max_fds(vector<int> & fds){
  int max_fd=0;
  for (size_t i=0;i<fds.size();i++){
    max_fd=max(max_fd,fds[i]);
  }
  return max_fd;
}

char * receive_message(int connfd){
  char * message=new char[MAXLINE]{0};
  //memset(message,0,sizeof(char)*MAXLINE);
  recv(connfd,message,MAXLINE,0);
  return message;
}

void send_ip(int index,int connfd, vector<char *> & library){
    char * neigh_ip=library[index];
    send(connfd,neigh_ip,MAXLINE,0);
}