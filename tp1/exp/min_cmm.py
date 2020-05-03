import simulator
import math

import numpy as np
from typing import List

import copy

def games2cmm(games: List[simulator.Game], teams):
    b = np.zeros((1,teams))[0]
    cmm = np.zeros(shape=(teams, teams))
    
    for game in games:
        cmm[game.team1-1][game.team2-1]-=1
        cmm[game.team2-1][game.team1-1]-=1
        cmm[game.team2-1][game.team2-1]+=1
        cmm[game.team1-1][game.team1-1]+=1
        if game.goals1 > game.goals2:
            b[game.team1-1]+=1
            b[game.team2-1]-=1
        else:
            b[game.team1-1]-=1
            b[game.team2-1]+=1

    for i in range(0,teams):
        cmm[i][i] += 2
        b[i] /= 2
        b[i] += 1
            
    return np.linalg.solve(cmm,b)

def count_better_than(ratings: List[float], player: int) -> int:
    res = 0
    for i in range(len(ratings)):
        if (i == player): continue
        pr = ratings[player]
        r = ratings[i]
        # r >= pr
        if (r > pr or math.isclose(r, pr)): res += 1
    
    return res

def has_draw(player, games, team_count) -> bool:
    ratings = games2cmm(games, team_count)
    for i in range(len(ratings)):
        if (i == player): continue
        pr = ratings[player]
        r = ratings[i]
        if (math.isclose(r, pr)): return True

    return False

def rank_and_worst(player, games, team_count) -> (int, int):
    ratings = games2cmm(games, team_count)
    rank = count_better_than(ratings, player-1)
    worst_rating = ratings[0]
    worst_player_index = 0

    for i in range(len(ratings)):
        if ratings[i] < worst_rating and i != player - 1:
            worst_player_index = i
    
    return rank, worst_player_index + 1

def lose_against(player1, player2, games):
    aux = copy.deepcopy(games)
    
    for i in range(len(aux)):
        if ((aux[i].team1 == player1 and
            aux[i].team2 == player2 and 
            aux[i].goals1 > aux[i].goals2) or 
            (aux[i].team1 == player2 and
            aux[i].team2 == player1 and
            aux[i].goals2 > aux[i].goals1)):
            
            aux[i].goals1, aux[i].goals2 = aux[i].goals2, aux[i].goals1
            break
            
    return aux

def min_cmm(games: List[simulator.Game], teams, selected) -> List[simulator.Game]:
    if(has_draw(selected, games, len(teams))): return games

    old_games = copy.deepcopy(games)
    current_games = copy.deepcopy(games)

    for _ in range(len(games)):
        old_games = copy.deepcopy(current_games)
        rank, worst_player, = rank_and_worst(selected, old_games, len(teams))
        current_games = lose_against(selected, worst_player, current_games)
        new_rank, _, = rank_and_worst(selected, current_games, len(teams))
        if (new_rank > rank): break

    return old_games