#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "function.h"

using namespace std;

int main(int argc, char *argv[])
{
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = argv[1];
  const char *port     = argv[2];
  
  if (argc < 2) {
      cout << "Syntax: client <hostname>\n" << endl;
      return 1;
  }

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if
  
  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if
  int mynum=0;
  recv(socket_fd,&mynum,sizeof(mynum),0); 
  string response="Player "+to_string(mynum)+" is ready to play";
  //char message[128];
  //strcpy(message,response.c_str());
  send(socket_fd, &response, MAXLINE, 0);
  //char neigh_host[128];
  char * neigh_host;
  recv(socket_fd,&neigh_host,sizeof(neigh_host),0);
  char * neigh_port;
  recv(socket_fd,&neigh_port,sizeof(neigh_port),0);
  cout<<"host:"<<neigh_host<<" port:"<<neigh_port<<endl;

  freeaddrinfo(host_info_list);
  close(socket_fd);

  return 0;
}
