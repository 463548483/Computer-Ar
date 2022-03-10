#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <vector>

using namespace std;
const int MAXLINE=128;

int init_client(const char * hostname, const char * port);
int init_server(const char * port);
int server_accept(int listenfd, char * client_hostname);
int get_port(int socket_fd);
int max_fds(vector<int> & fds);
char * receive_message(int connfd);
void send_ip(int index,int connfd, vector<char *> & library);

class Potato {
  public:
    int trace_player[512];
    int round;
    int remain_hop;
  public:

    Potato(){
        memset(trace_player,0,sizeof(trace_player));
        remain_hop=0;
        round=0;
    }
    int trace_potato(int player_id){
      remain_hop-=1;
      round+=1;
      trace_player[round-1]=player_id;
      if (remain_hop==0){
        return 0;
      }
      else{
        return 1;
      }
    }
    ~Potato(){
    }
};
