#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include "function.h"
//#include "potato.h"
#include <cstring>
#include <iostream>
#include  <vector>

using namespace std;

int main(int argc, char * argv[]) {
  if (argc != 4) {
    cout << "ringmaster <port_num> <num_players> <num_hops>" << endl;
    exit(EXIT_FAILURE);
  }

  const char * master_port=argv[1];
  int num_player=atoi(argv[2]);
  int num_hops=atoi(argv[3]);
  cout<<"Potato Ringmaster"<<endl;
  cout<<"Players ="<<num_player<<endl;
  cout<<"Hops ="<<num_hops<<endl;

  //intial as server
  int socket_fd=init_server(master_port);
  
  //connect to player process
  vector<int> players_fd;
  vector<char [MAXLINE]> players_hostname;
  vector<char [MAXLINE]> players_port;
  for (int i=0;i<num_player;i++){
    char client_hostname[MAXLINE];
    char client_port[MAXLINE];

    int connfd=server_send(socket_fd,client_hostname,client_port);
    cout<<"Connected to "<<client_hostname<<" on "<<client_port<<endl;
    send(connfd,&i,sizeof(i),0);
    send(connfd,&num_player,sizeof(num_player),0);
    
    char message[512];
    recv(connfd,&message,sizeof(message),0);
    players_fd.push_back(connfd);
    players_hostname.push_back(client_hostname);
    players_port.push_back(client_port);
    cout<<message<<endl;
  }
  //send neighbor info to player
  for (int i=0;i<num_player;i++){
    int neigh_id=(i+1)%num_player;
    //char * neigh_host=players_hostname[neigh_id];
    //int neigh_port=stoi(players_port[neigh_id]);
    send(players_fd[i],&players_hostname[neigh_id],MAXLINE,0);
    send(players_fd[i],&players_port[neigh_id],MAXLINE,0);
  }

  //play photo
  //exit
  close(socket_fd);
  return 0;
}
