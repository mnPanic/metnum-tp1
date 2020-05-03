import random
import argparse
import numpy as np
from simulator import *
import math

def count_better_than(ratings: List[float], player: int) -> int:
    res = 0
    for i in range(len(ratings)):
        if i == player: continue
        pr = ratings[player]
        r = ratings[i]
        # r >= pr
        if (r > pr or math.isclose(r, pr)): res += 1
    
    return res

def GenerateCMM(games,teams_count):
    cmm = np.zeros(shape=(teams_count,teams_count))
    b = np.zeros((1,teams_count))[0]

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

    for i in range(0,teams_count):
        cmm[i][i] += 2
        b[i] /= 2
        b[i] += 1
    
    return [cmm,b]

def ScoreSolution(ranking,losses,team_count):
    score = 0.0
    score += 5.0 * (team_count-ranking)/team_count
    score += losses/((team_count-1)*1000.0)
    return score
       
def HillClimber(games,teams,selected):

    matches_against_each = {}
    wins_against_each = {}
    best_team = Team(selected, 0)

    for index in range(1, len(teams) + 1):
        if index != best_team.index:
            wins_against_each[index] = 0
    
    for game in games:
        if game.team1 == best_team.index:
            if game.team1 in matches_against_each:
                matches_against_each[game.team2]+=1
            else:
                matches_against_each[game.team2]=1

            if game.goals1 > game.goals1:
                wins_against_each[game.team2]+=1
        elif game.team2 == best_team.index:
            if game.team1 in matches_against_each:
                matches_against_each[game.team1]+=1
            else:
                matches_against_each[game.team1]=1

            if game.goals2 > game.goals1:
                wins_against_each[game.team1]+=1
    
    
    #print(wins_against_each)
    #print(matches_against_each)

    cmm, b = GenerateCMM(games,len(teams))

    #print("b")
    #print(b)
    #calculate actual ranking
    temp_x = np.linalg.solve(cmm,b)
    best_ranking = count_better_than(temp_x, best_team.index-1)
    best_losses = sum(list(matches_against_each.values())) - sum(list(wins_against_each.values()))
    best_score = ScoreSolution(best_ranking,best_losses,len(teams))
    best_decisions = wins_against_each
    
    losses = best_losses
    #print(losses)

    for i in range(0,1000):
        #select an opp
        random_team = random.choices(list(matches_against_each.keys()))[0]
        #select outcome
        options = []
        #if matches_against_each[random_team] > wins_against_each[random_team]:
        #    options.append(0)
        if 0 < wins_against_each[random_team]:
           options.append(-1)
        else:
            options.append(0)

        rnd_game_result = random.choices(options)[0]

        #apply outcome to variables
        wins_against_each[random_team] += rnd_game_result
        b[best_team.index-1] += rnd_game_result
        b[random_team-1] -= rnd_game_result
        losses -= rnd_game_result

        #solve new system
        x = np.linalg.solve(cmm,b)
        #calculate new ranking
        ranking = count_better_than(x, best_team.index-1)

        score = ScoreSolution(ranking,losses,len(teams))

        #if solution is better save and keep going from here
        #revert otherwise
        if best_score < score:
            best_decisions = wins_against_each
            best_ranking = ranking
            best_losses = losses
            best_score = score
            #print("New best:")
            #print("ranking",best_ranking)
            #print("best_losses",best_losses)
            #print("best_score",best_score)
            #print(wins_against_each)
            #print(x)
            #print(b)
        else:
            wins_against_each[random_team] -= rnd_game_result
            b[best_team.index-1] -= rnd_game_result
            b[random_team-1] += rnd_game_result
            losses += rnd_game_result

    #print("Final result:")
    #print("ranking",best_ranking)
    #print("best_losses",best_losses)
    #print("best_score",best_score)
    #print("best_decisions",best_decisions)
    #print(b)

    #print(games)
    for game in games:
        if game.team1 == best_team.index:
            game.goals1 = 0
            game.goals2 = 1
        if game.team2 == best_team.index:
            game.goals2 = 0
            game.goals1 = 1

    #print(games)
    
    for game in games:
        if game.team1 == best_team.index and best_decisions[game.team2] > 0:
            game.goals1 = 1
            game.goals2 = 0
            best_decisions[game.team2]-=1
        if game.team2 == best_team.index and best_decisions[game.team1] > 0:
            game.goals2 = 1
            game.goals1 = 0
            best_decisions[game.team1]-=1

    return games
