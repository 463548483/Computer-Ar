#include "query_funcs.h"


void add_player(connection *C, int team_id, int jersey_num, string first_name, string last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg)
{
    work W(*C);
    string sql="INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG,RPG, APG, SPG, BPG)";
    sql.append("VALUES ("+to_string(team_id));
    sql.append(","+to_string(jersey_num));
    sql.append(","+W.quote(first_name));
    sql.append(","+W.quote(last_name));
    sql.append(","+to_string(mpg));
    sql.append(","+to_string(ppg));
    sql.append(","+to_string(rpg));
    sql.append(","+to_string(apg));
    sql.append(","+to_string(spg));
    sql.append(","+to_string(bpg)+");");
    W.exec( sql );
    W.commit();
}


void add_team(connection *C, string name, int state_id, int color_id, int wins, int losses)
{
    work W(*C);
    string sql="INSERT INTO TEAM (NAME, STATE_ID, COLOR_ID, WINS, LOSSES)";
    sql.append("VALUES ("+W.quote(name));
    sql.append(","+to_string(state_id));
    sql.append(","+to_string(color_id));
    sql.append(","+to_string(wins));
    sql.append(","+to_string(losses)+");");
    W.exec( sql );
    W.commit();
}


void add_state(connection *C, string name)
{
    work W(*C);
    string sql="INSERT INTO STATE (NAME)";
    sql.append("VALUES ("+W.quote(name)+");");
    W.exec( sql );
    W.commit();
}


void add_color(connection *C, string name)
{
    work W(*C);
    string sql="INSERT INTO COLOR (NAME)";
    sql.append("VALUES ("+W.quote(name)+");");
    W.exec( sql );
    W.commit();
}


void query1(connection *C,
	    int use_mpg, int min_mpg, int max_mpg,
            int use_ppg, int min_ppg, int max_ppg,
            int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg,
            int use_spg, double min_spg, double max_spg,
            int use_bpg, double min_bpg, double max_bpg
            )
{
    string sql="select * from player where ";
    int use_flag[6]={use_mpg,use_ppg,use_rpg,use_apg,use_spg,use_bpg};
    double min_range[6]={min_mpg,min_ppg,min_rpg,min_apg,min_spg,min_bpg};
    double max_range[6]={max_mpg,max_ppg,max_rpg,max_apg,max_spg,max_bpg};
    string variable[6]={ "mpg","ppg","rpg","apg","spg","bpg"};
    for (int i=0;i<6;i++){
        if (use_flag[i]>0){
            sql.append(variable[i]+"<="+to_string(max_range[i])+" and "+variable[i]+">="+to_string(min_range[i]));
        }
    }
    sql.append(";");

    /* Create a non-transactional object. */
    nontransaction N(*C);
      
    /* Execute SQL query */
    result R( N.exec( sql ));
    cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG"<< endl;
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        for(int i=0;i<11;i++){
            cout<<c[i]<<" ";
        }
        cout<<endl;
    }
}


void query2(connection *C, string team_color)
{
    string sql="select team.name from team,color where color.name='"+team_color+"' and team.color_id=color.color_id;";
    /* Create a non-transactional object. */
    nontransaction N(*C);
      
    /* Execute SQL query */
    result R( N.exec( sql ));
    cout<<"NAME"<<endl;
    /* List down all the records */
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
      cout<< c[0] << endl;
    }
}


void query3(connection *C, string team_name)
{
    string sql="select first_name,last_name from player,team where team.name='"+team_name+"' and team.team_id=player.team_id order by ppg desc;";
    /* Create a non-transactional object. */
    nontransaction N(*C);
      
    /* Execute SQL query */
    result R( N.exec( sql ));
    cout<<"FIRST_NAME LAST_NAME"<<endl;
    /* List down all the records */
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
      cout <<c[0]<<" " << c[1] << endl;
    }
}


void query4(connection *C, string team_state, string team_color)
{
    string sql="select first_name,last_name,uniform_num from player,team,state,color "\
    " where color.name='"+team_color+"' and state.name='"+team_state+"' and team.color_id=color.color_id and team.state_id=state.state_id and team.team_id=player.team_id;";
    /* Create a non-transactional object. */
    nontransaction N(*C);
      
    /* Execute SQL query */
    result R( N.exec( sql ));
    cout<<"FIRST_NAME LAST_NAME UNIFORM_NUM"<<endl;
    /* List down all the records */
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
      cout <<c[0]<<" " << c[1] <<" "<<c[2].as<int>()<< endl;
    }
}


void query5(connection *C, int num_wins)
{
    string sql="select first_name,last_name,team.name, wins from player,team where team.team_id=player.team_id and wins>"+to_string(num_wins)+";";
    /* Create a non-transactional object. */
    nontransaction N(*C);
      
    /* Execute SQL query */
    result R( N.exec( sql ));
    cout<<"FIRST_NAME LAST_NAME NAME WINS"<<endl;
    /* List down all the records */
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
      cout <<c[0]<<" " << c[1] <<" "<<c[2]<<" "<<c[3].as<int>()<< endl;
    }
}
