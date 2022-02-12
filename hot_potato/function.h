#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

using namespace std;
const int MAXLINE=128;

int init_client(const char * hostname, const char * port);
int init_server(const char * port);
int server_send(int listenfd, char (& client_hostname)[MAXLINE] ,char (& client_port)[MAXLINE] );