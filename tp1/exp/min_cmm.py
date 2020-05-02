import simulator
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

def rank_and_worst(player, games, teams) -> (int, int):
    ratings = games2cmm(games, teams)
    rank = sum(ratings > ratings[player-1])
    worst_rating = ratings[0]
    worst_player_index = 0
    
    for i in range(len(ratings)):
        if ratings[i] < worst_rating:
            worst_player_index = i
    
    return rank,worst_player_index +1

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
    old_games = games.copy()
    current_games = games.copy()
    for i in range(len(current_games)): #Cambiar
        old_games = current_games.copy()
        rank,worst_player = rank_and_worst(selected, current_games, len(teams))
        
        current_games = lose_against(selected, worst_player, old_games)
        
        new_ratings = games2cmm(current_games, len(teams))
        new_rank = sum(new_ratings > new_ratings[selected-1])
        if (new_rank > rank): break
    return old_games