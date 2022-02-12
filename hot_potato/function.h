#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

using namespace std;

int init_client(const char * hostname, const char * port);
int init_server(const char * port);
int server_send(int listenfd, char* & client_hostname,char* & client_port);