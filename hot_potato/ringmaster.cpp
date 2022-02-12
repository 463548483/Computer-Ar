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
  vector<string> players_hostname;
  vector<string> players_port;
  for (int i=0;i<num_player;i++){
    string client_hostname;
    string client_port;

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
    char neigh_host[128]="";
    strcpy(neigh_host,players_hostname[neigh_id].c_str());
    int neigh_port=stoi(players_port[neigh_id]);
    send(players_fd[i],&neigh_host,sizeof(neigh_host),0);
    send(players_fd[i],&neigh_port,sizeof(neigh_port),0);
  }
  
  //play photo
  //exit
  close(socket_fd);
  return 0;
}
