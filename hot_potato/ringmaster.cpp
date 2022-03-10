#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include "potato.h"
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
  if (num_player<=1) {
    cout << "player must greater than 1" << endl;
    exit(EXIT_FAILURE);
  }
  int num_hops=atoi(argv[3]);
  cout<<"Potato Ringmaster"<<endl;
  cout<<"Players = "<<num_player<<endl;
  cout<<"Hops = "<<num_hops<<endl;

  //intial as server
  int socket_fd=init_server(master_port);
  
  //connect to player process
  vector<int> players_fd;
  vector<char *> players_hostname;
  vector<char *> players_port;
  for (int i=0;i<num_player;i++){
    char * client_hostname=new char[MAXLINE]{0};
    //memset(client_hostname,0,sizeof(char)*MAXLINE);
    char * player_server_port=new char[MAXLINE]{0};
    //memset(player_server_port,0,sizeof(char)*MAXLINE);

    int connfd=server_accept(socket_fd,client_hostname);
    //cout<<"Connected to "<<client_hostname<<" on "<<client_port<<endl;
    send(connfd,&i,sizeof(i),0);
    send(connfd,&num_player,sizeof(num_player),0);
    recv(connfd,player_server_port,sizeof(player_server_port),0);

    players_fd.push_back(connfd);
    players_hostname.push_back(client_hostname);
    players_port.push_back(player_server_port);
  }
  //send neighbor info to player
  for (int i=0;i<num_player;i++){
    //send left neighbor hostname and port
    int left_neigh_id=(i-1+num_player)%num_player;
    send_ip(left_neigh_id,players_fd[i],players_hostname);
    send_ip(left_neigh_id,players_fd[i],players_port);
    // //send player own port
    // send_ip(i,players_fd[i],players_port);
  }
    for (int i=0;i<num_player;i++){
      delete[] players_hostname[i];
      delete[] players_port[i];
  }


  for (int i=0;i<num_player;i++){   
    //receive ready to play
    char message[MAXLINE];
    recv(players_fd[i],message,MAXLINE,0);
    cout<<message<<endl;
  }

  //play potato
  srand((unsigned int)time(NULL)+num_player);
  int random=rand()%num_player;
  cout<<"Ready to start the game, sending potato to player "+to_string(random)<<endl;
  Potato  init_potato;
  init_potato.remain_hop=num_hops;
  send(players_fd[random],&init_potato,sizeof(init_potato),0);
  //cout<<"send fd "<<players_fd[random]<<endl;
  //receive back
  
  int numfds=max_fds(players_fd)+1;
  fd_set readfds;
  FD_ZERO(&readfds);
  for (int i = 0; i < num_player; i++) {
    FD_SET(players_fd[i], &readfds);
  }

  select(numfds + 1, &readfds, NULL, NULL, NULL);

  
  for (int i = 0; i < num_player; i++) {
    if (FD_ISSET(players_fd[i], &readfds)) {
      recv(players_fd[i], &init_potato, sizeof(init_potato), MSG_WAITALL);
      cout<<"Trace of potato:"<<endl;
      for (int i=0;i<init_potato.round;i++){
        cout<<init_potato.trace_player[i];
        if (i!=init_potato.round-1){
          cout<<",";
        }
        
      }
      cout<<endl;
      break;
    }
  }

  //exit
  close(socket_fd);
  return 0;


}
