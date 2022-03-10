#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "potato.h"

using namespace std;





int main(int argc, char *argv[])
{
  if (argc < 3) {
      cout << "Syntax: player <machine_name><port_num>\n" << endl;
      return EXIT_FAILURE;
  }
  const char *hostname = argv[1];
  const char *port     = argv[2];

  int master_fd=init_client(hostname,port);

  //receive playernum, total_player from server
  int mynum=0;
  recv(master_fd,&mynum,sizeof(mynum),0); 
  int num_player=0;
  recv(master_fd,&num_player,sizeof(num_player),0); 
  cout<<"Connected as player "<<mynum<<" out of "<<num_player<<" total players"<<endl;
  
  //init as server, send port num to master
  int server_socket_fd=init_server("");
  int server_port_num=get_port(server_socket_fd);
  const char * server_port=to_string(server_port_num).c_str();
  send(master_fd,server_port,sizeof(server_port),0);
  //cout<<"as server port "<<server_port<<endl;

  //receive left neighbot descriptor from server
  char * left_neigh_host=receive_message(master_fd);
  char * left_neigh_port=receive_message(master_fd);
  //cout<<"left host "<<left_neigh_host<<"port "<<left_neigh_port<<endl;
  
  //as client connect to left
  int left_socket_fd=init_client(left_neigh_host,left_neigh_port);
  delete left_neigh_host;
  delete left_neigh_port;
  //cout<<"Connected to left "<<endl;

  // as server connect to right
  char right_neigh_host[MAXLINE];
  memset(right_neigh_host,0,sizeof(char)*MAXLINE);
  int right_socket_fd=server_accept(server_socket_fd,right_neigh_host);
  //cout<<"Connected to right"<<endl;

  
  //send ready to play
  string response="Player "+to_string(mynum)+" is ready to play";
  // char message[MAXLINE];
  // memset(message,0,sizeof(char)*MAXLINE);
  // strcpy(message,response.c_str());
  send(master_fd, response.c_str(), MAXLINE, 0);

  //start play potato
  Potato hot_potato;
  vector<int> fds={master_fd,left_socket_fd,right_socket_fd};
  int numfds=max_fds(fds)+1;
  fd_set readfds;

  while (true) {
    FD_ZERO(&readfds);
    for (int i = 0; i < 3; i++) {
      FD_SET(fds[i], &readfds);
    }

    select(numfds + 1, &readfds, NULL, NULL, NULL);

    int rv;
    for (int i = 0; i < 3; i++) {
      if (FD_ISSET(fds[i], &readfds)) {
        rv=recv(fds[i], &hot_potato, sizeof(hot_potato), MSG_WAITALL);
        break;
      }
    }
  
    //end with hops=0 or ringmaster close
    if (hot_potato.remain_hop==0 or rv==0 ){
      break;
    }

    //send potato to neighbor
    int potato_status=hot_potato.trace_potato(mynum);//1 continue,0 end

    if (potato_status==1){//continue send to neighbor
      
      srand((unsigned int)time(NULL)+mynum);
      int random=rand()%2;
      if(random==1){
        send(left_socket_fd,&hot_potato,sizeof(hot_potato),0);
        cout<<"Sending potato to "<<(mynum-1+num_player)%num_player<<endl;
      }
      else{
        send(right_socket_fd,&hot_potato,sizeof(hot_potato),0);
        cout<<"Sending potato to "<<(mynum+1+num_player)%num_player<<endl;
      }
    }
    else{
      send(master_fd,&hot_potato,sizeof(hot_potato),0);
      cout<<"I'm it"<<endl;
      break;
    }
  }
  for (int i=0;i<3;i++){
    close(fds[i]);
  }
  close(server_socket_fd);

  return 0;
}
