import simulator as sim
from typing import List

def find(teams: List[sim.Team], t: int) -> sim.Team:
    """busca el equipo en la lista"""
    for team in teams:
        if(team.index == t):
            return team
    
    return None

def lose(g: sim.Game, sel: int) -> sim.Game:
    """Hace que el team t pierda"""
    if(g.team1 == sel):
        # make 2 win
        g.goals1 = 0
        g.goals2 = 1
    elif (g.team2 == sel):
        # make 1 win
        g.goals2 = 0
        g.goals1 = 1

    return g

def win_one(games: List[sim.Game], teams: List[sim.Team], selected: int) -> List[sim.Game]:
    """piede todas menos contra el mejor"""
    max_game = -1
    max_skill = -1
    max_idx = -1
    for i in range(len(games)):
        other_idx = -1
        game = games[i]
        if(game.team1 == selected):
            other_idx = game.team2
        elif(game.team2 == selected):
            other_idx = game.team1
        else:
            # no aparece en este juego
            continue

        games[i] = lose(game, selected)
        other_skill = find(teams, other_idx).power
        if(other_skill > max_skill):
            max_game = i
            max_skill = other_skill
            max_idx = other_idx

    # gano 1
    games[max_game] = lose(games[max_game], max_idx)
    return games

def lose_worse(games: List[sim.Game], teams: List[sim.Team], selected: int) -> List[sim.Game]:
    """pierde contra la gente peor que el en skill"""
    skill = find(teams, selected).power
    for i in range(len(games)):
        other_idx = -1
        game = games[i]
        if(game.team1 == selected):
            other_idx = game.team2
        elif(game.team2 == selected):
            other_idx = game.team1
        else:
            # no aparece en este juego
            continue
        
        other_skill = find(teams, other_idx).power
        if (other_skill >= skill):
            # es mejor que nosotros, lo dejamos tal cual
            continue

        # hago que pierda
        games[i] = lose(game, selected)
    
    return games

def lose_better(games: List[sim.Game], teams: List[sim.Team], selected: int) -> List[sim.Game]:
    """pierde contra la persona peor en skill"""
    skill = find(teams, selected).power
    for i in range(len(games)):
        other_idx = -1
        game = games[i]
        if(game.team1 == selected):
            other_idx = game.team2
        elif(game.team2 == selected):
            other_idx = game.team1
        else:
            # no aparece en este juego
            continue
        
        other_skill = find(teams, other_idx).power
        if (other_skill < skill):
            # es peor que nosotros, perdemos
            continue

        # hago que pierda
        games[i] = lose(game, selected)
    
    return games
    