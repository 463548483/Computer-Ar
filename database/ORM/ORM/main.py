import os

# from Table.models import *
os.environ.setdefault("DJANGO_SETTINGS_MODULE", "ORM.settings")

import django
django.setup()

def insertState():
    from Table.models import Color
    file = open('color.txt')
    for line in file:
       color_id, name= line.split(' ')
       Color.objects.get_or_create(name = name[:-1])
    file.close()
    return

def insertColor():
    from Table.models import State
    file = open('state.txt')
    for line in file:
       state_id, name= line.split(' ')
       State.objects.get_or_create(name = name[:-1])
    file.close()
    return


def insertTeam():
    from Table.models import Team,State,Color
    file = open('team.txt')
    for line in file:
       team_id, name, state_id,color_id,wins,losses= line.split(' ')
       Team.objects.get_or_create(name = name,state_id=State.objects.get(state_id=state_id),color_id=Color.objects.get(color_id=color_id),wins=wins,losses=losses)
    file.close()
    return

def insertPlayer():
    from Table.models import Team,State,Color,Player
    file = open('player.txt')
    for line in file:
       player_id, team_id, uniform_id,first_name,last_name,mpg,ppg,rpg,apg,spg,bpg= line.split(' ')
       Player.objects.get_or_create(team_id =Team.objects.get(team_id=team_id),uniform_num=uniform_id,first_name=first_name,last_name=last_name,mpg=mpg,ppg=ppg,rpg=rpg,apg=apg,spg=spg,bpg=bpg)
    file.close()
    return


def query1(use_mpg,  min_mpg,  max_mpg, use_ppg,  min_ppg,  max_ppg, use_rpg,  min_rpg,  max_rpg, use_apg,  min_apg,  max_apg, use_spg, min_spg, max_spg, use_bpg, min_bpg, max_bpg):
    from Table.models import Player
    query_rslt = Player.objects.all()
    if use_mpg:
        query_rslt = query_rslt.filter(mpg__lte = max_mpg, mpg__gte = min_mpg)
    if use_ppg:
       query_rslt = query_rslt.filter(ppg__lte = max_ppg, ppg__gte = min_ppg)
    if use_rpg:
       query_rslt = query_rslt.filter(rpg__lte = max_rpg, rpg__gte = min_rpg)
    if use_apg:
       query_rslt = query_rslt.filter(apg__lte = max_apg, apg__gte = min_apg)
    if use_spg:
       query_rslt = query_rslt.filter(spg__lte = max_spg, spg__gte = min_spg)
    if use_bpg:
       query_rslt = query_rslt.filter(bpg__lte = max_bpg, bpg__gte = min_bpg)
    print("PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG")
    for player in query_rslt:
        print(player.player_id,player.team_id,player.uniform_num,player.first_name,player.last_name,player.mpg,player.ppg,player.rpg,player.apg,player.spg,player.bpg)
    return


def query2(team_color):
    from Table.models import Team
    query_rslt = Team.objects.filter(color_id__name = team_color)
    print("NAME")
    for team in query_rslt:
        print(team.name)
    return


def query3(team_name):
    from Table.models import Player
    query_rslt = Player.objects.filter(team_id__name = team_name).order_by('-ppg')
    print("FIRST_NAME LAST_NAME")
    for player in query_rslt:
        print(player.first_name,player.last_name)
    return


def query4(team_state,team_color):
    from Table.models import Player,Team
    team=Team.objects.filter()
    query_rslt = Player.objects.filter(team_id__color_id__name=team_color,team_id__state_id__name=team_state)
    print("FIRST_NAME LAST_NAME UNIFORM_NUM")
    for player in query_rslt:
        print(player.first_name,player.last_name,player.uniform_num)
    return

def query5(num_wins):
    from Table.models import Player
    query_rslt = Player.objects.filter(team_id__wins__gt=num_wins)
    print("FIRST_NAME LAST_NAME NAME WINS")
    for player in query_rslt:
        print(player.first_name,player.last_name,player.team_id.name,player.team_id.wins)
    return

def main():
    insertState()
    insertColor()
    insertTeam()
    insertPlayer()
    query1(1,35,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
    query2("Green")
    query3("UNC")
    query4("FL","Green")
    query5(12)

if __name__=="__main__":
    main()