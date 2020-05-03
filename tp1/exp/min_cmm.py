import simulator
import math

import numpy as np
from typing import List
from operator import itemgetter

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

def rank_and_worst(player, games, team_count, worst) -> (int, int):
    ratings = games2cmm(games, team_count)
    # sort ratings into dict
    dratings = [{"idx": i+1, "rating": r} for i, r in enumerate(ratings)]
    dratings = sorted(dratings, key=itemgetter("rating"))

    rank = count_better_than(ratings, player-1)

    return rank, dratings[worst]["idx"]

def lose_against(player1, player2, games):
    aux = copy.deepcopy(games)
    lost = False
    
    for i in range(len(aux)):
        if ((aux[i].team1 == player1 and
            aux[i].team2 == player2 and 
            aux[i].goals1 > aux[i].goals2) or 
            (aux[i].team1 == player2 and
            aux[i].team2 == player1 and
            aux[i].goals2 > aux[i].goals1)):
            
            aux[i].goals1, aux[i].goals2 = aux[i].goals2, aux[i].goals1
            lost = True
            break
            
    return aux, lost

def min_cmm(games, teams, selected):
    return min_cmm_tol(games, teams, selected, 0)

def min_cmm_tol_1(games, teams, selected):
    return min_cmm_tol(games, teams, selected, 1)

def min_cmm_tol_3(games, teams, selected):
    return min_cmm_tol(games, teams, selected, 3)

def min_cmm_tol(
        games: List[simulator.Game],
        teams,
        selected,
        tolerance: int,
    ) -> List[simulator.Game]:
    if(has_draw(selected, games, len(teams))): return games

    old_games = copy.deepcopy(games)
    current_games = copy.deepcopy(games)
    # el iesimo peor
    worst_idx = 0

    initial_rank, _ = rank_and_worst(selected, old_games, len(teams), worst_idx)

    for i in range(len(games)):
        old_games = copy.deepcopy(current_games)
        _, worst_player = rank_and_worst(selected, old_games, len(teams), worst_idx)
        
        current_games, lost = lose_against(selected, worst_player, current_games)
        if not lost:
            # if we didn't lose, then there isn't any more games to lose
            # to the worst player.
            worst_idx += 1
            # No hay mas jugadores contra los cuales perder
            if worst_idx == len(teams): break

        rank, _ = rank_and_worst(selected, current_games, len(teams), worst_idx)
        if (rank - tolerance > initial_rank): break

    return old_games