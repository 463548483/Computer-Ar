#!/usr/bin/env python3
from Table.models import Player, Team, State, Color
import django
import os
os.environ.setdefault("DJANGO_SETTINGS_MODULE", "ORM.settings")

django.setup()


def query1(use_mpg,  min_mpg,  max_mpg, use_ppg,  min_ppg,  max_ppg, use_rpg,  min_rpg,  max_rpg, use_apg,  min_apg,  max_apg, use_spg, min_spg, max_spg, use_bpg, min_bpg, max_bpg):
    player_all = Player.obj.all()
    use_flag = [use_mpg, use_ppg, use_rpg, use_apg, use_spg, use_bpg]
    min_range = [min_mpg, min_ppg, min_rpg, min_apg, min_spg, min_bpg]
    max_range = [max_mpg, max_ppg, max_rpg, max_apg, max_spg, max_bpg]
    for i in range(len(use_flag)):
        if use_flag[i]:
            query_rslt = player_all.filter(
                bpg__lte=max_range[i], bpg__gte=min_range[i])
    print("PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG")
    for value in query_rslt:
        print(value)
    return


def query2(team_color):
    query_rslt = Team.objects.filter(color_id__name = team_color)
    print("NAME")
    for team in query_rslt:
        print(team.name)
    return


def query3(team_name):
    query_rslt = Player.objects.filter(team_id__name = team_name).order_by(Player.ppg)
    print("FIRST_NAME LAST_NAME")
    for player in query_rslt:
        print(player.first_name,player.last_name)
    return


def query4(team_state,team_color):
    Team=Team.objects.filter(color_id__name=team_color,state_id__name=team_state)
    query_rslt = Player.objects.filter(team_id = Team.team_id).order_by(Player.ppg)
    print("FIRST_NAME LAST_NAME UNIFORM_NUM")
    for player in query_rslt:
        print(player.first_name,player.last_name,player.uniform_num)
    return

def query5(num_wins):
    Team=Team.objects.filter(wins=num_wins)
    query_rslt = Player.objects.filter(team_id__wins__gte=num_wins)
    print("FIRST_NAME LAST_NAME NAME WINS")
    for player in query_rslt:
        print(player.first_name,player.last_name,player.team_id__name,player.team_id__wins)
    return
