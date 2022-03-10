#include <iostream>
#include <pqxx/pqxx>

#include "exerciser.h"
#include "query_funcs.h"

using namespace std;
using namespace pqxx;
#include <fstream>

connection * C;

void commit(string sql){
    /* Create a transactional object. */
  work W(*C);
  
  /* Execute SQL query */
  W.exec( sql );
  W.commit();
  //cout << "submit successfully" << endl;
}


void drop(string tbname){
  string sql;
  sql="DROP TABLE IF EXISTS "+tbname+" CASCADE;";
  commit(sql);
}

void create(string tbfile){
  string line;
  string sql;
  ifstream infile(tbfile.c_str());
  while(getline(infile,line)){
    sql.append(line);
  }
  commit(sql);
}

void insertColor(string tbfile){
  string sql,line,name;
  ifstream infile(tbfile.c_str());
  while(getline(infile,line)){
    istringstream iss(line);
    int color_id;
    if (!(iss>>color_id>>name)){
      break;
    }
    add_color(C,name);
    //cout<<"Insert color success"<<endl;
  }
}

void insertState(string tbfile){
  string sql,line,name;
  ifstream infile(tbfile.c_str());
  while(getline(infile,line)){
    istringstream iss(line);
    int state_id;
    if (!(iss>>state_id>>name)){
      break;
    }
    add_state(C,name);
    //cout<<"Insert state success"<<endl;
  }
}

void insertTeam(string tbfile){
  string sql,line;
  string name;
  int team_id,state_id, color_id, wins, losses;
  ifstream infile(tbfile.c_str());
  while(getline(infile,line)){
    istringstream iss(line);
    int color_id;
    if (!(iss>>team_id>>name>>state_id>>color_id>>wins>>losses)){
      break;
    }
    add_team(C,name,state_id, color_id, wins, losses);
    //cout<<"Insert team success"<<endl;
  }
}

void insertPlayer(string tbfile){
  string sql,line,first_name,last_name;
  int player_id,team_id, jersey_num, mpg, ppg, rpg, apg;
  double spg,  bpg;
  ifstream infile(tbfile.c_str());
  while(getline(infile,line)){
    istringstream iss(line);
    int color_id;
    if (!(iss>>player_id>>team_id>>jersey_num>>first_name>>last_name>>mpg>>ppg>>rpg>>apg>>spg>>bpg)){
      break;
    }
    add_player(C,team_id, jersey_num,first_name,last_name, mpg, ppg, rpg, apg,spg,bpg);
    //cout<<"Insert player success"<<endl;
  }
}

int main (int argc, char *argv[]) 
{

  //Allocate & initialize a Postgres connection object
  //connection *C;
  string sql;
  try{
    //Establish a connection to the database
    //Parameters: database name, user name, user password
    C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
    if (C->is_open()) {
      cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return 1;
    }
    
    drop("STATE");
    drop("COLOR");
    drop("TEAM");
    drop("PLAYER");
    create("table.sql");
    insertColor("color.txt");
    insertState("state.txt");
    insertTeam("team.txt");
    insertPlayer("player.txt");
    C->disconnect ();
  } catch (const std::exception &e){
    cerr << e.what() << std::endl;
    return 1;
  }


  exercise(C);


  //Close database connection
  C->disconnect();

  return 0;
}


