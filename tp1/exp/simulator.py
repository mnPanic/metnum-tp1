import random
import argparse

from typing import List

TEAMS = 10
MAX_POWER = 100
GAMES_AGAINST_EACH = 3
GOALS_PER_GAME = 3
CLUSTERS = 4

class Team:
    def __init__(self,index,power):
        self.index = index
        self.power = power
    def __repr__(self):  
        return f"{self.index} {self.power}"

    def __str__(self):
        return f"{self.index} {self.power}"

    def __eq__(self, other):
        if type(other) is type(self):
            return self.index == other.index and self.power == other.power
        else:
            return False

class Game:
    def __init__(self,t1,g1,t2,g2):
        self.team1 = t1
        self.team2 = t2
        self.goals1 = g1
        self.goals2 = g2
    def __repr__(self):  
        return f"1 {self.team1} {self.goals1} {self.team2} {self.goals2}"

    def __str__(self):
        return "1 " + str(self.team1) + " " + str(int(self.goals1)) + " " + str(self.team2) + " " + str(int(self.goals2))

def RunGame(team1,team2, goals_per_game: int):
    wins_t1 = 0
    wins_t2 = 0
    for goals in range(0,GOALS_PER_GAME):
        goal_chance = random.randint(1,team1.power+team2.power)

        # If T1 wins...
        if goal_chance <= team1.power:
            wins_t1 += 1
        else:
            wins_t2 += 1

    return Game(team1.index,wins_t1,team2.index,wins_t2)

def RoundRobin(teams: List[Team], games_against_each: int, goals_per_game: int):
    games = []

    for team1 in teams:
        for team2 in teams:
            if team1 != team2:
                for g in range(0,games_against_each):
                   games.append(RunGame(team1,team2, goals_per_game))

    return games

def Clusters(teams: List[Team], clusters: int, goals_per_game: int):
    games = []
    team_cluster = {}

    for team in teams:
        team_cluster[team.index] = random.randint(0,clusters)

    for team1 in teams:
        #cluster1 = int(team1.index*(TEAMS/CLUSTERS))
        for team2 in teams:
            #cluster2 = int(team1.index*(TEAMS/CLUSTERS))
            if team1.index != team2.index and team_cluster[team1.index] == team_cluster[team2.index]:
                for g in range(0,GAMES_AGAINST_EACH):
                    games.append(RunGame(team1,team2, goals_per_game))

    return games

def LinearPowers(team_count: int, max_power: int) -> List[Team]:

    teams_powers = []

    for i in range(1, team_count + 1):
        teams_powers.append(Team(i, i*(max_power/team_count)))

    return teams_powers

def ExponentialPowers(team_count: int, max_power: int) -> List[Team]:

    teams_powers = []

    for i in range(1, team_count + 1):
        teams_powers.append(Team(i,int(pow(max_power,i/team_count))))

    return teams_powers

def SaveMatches(filename, games, team_amount):
    with open(filename, 'w') as f:
        print(f"{team_amount} {len(games)}", file=f)
        for game in games:
            print(game, file=f)

def SavePowers(filename,teams):
    with open(filename, 'w') as f:
        for team in teams:
            print(team, file=f)

def simulate(
        powers: str = "linear",
        team_count: int = TEAMS,
        max_power: int = MAX_POWER,
        matches: str = "roundrobin",
        games_against_each: int = GAMES_AGAINST_EACH,
        goals_per_game: int = GOALS_PER_GAME,
        clusters: int = CLUSTERS,
        matches_output: str = "../data/sim.dat",
        team_powers_output: str = "../data/sim_powers.tsv"
    ):

    teams = []
    if powers == "linear":
        teams = LinearPowers(team_count, max_power)
    elif powers == "exponential": 
        teams = ExponentialPowers(team_count, max_power)

    games = []
    if matches == "roundrobin":
        games = RoundRobin(teams, games_against_each, goals_per_game)
    elif matches == "clusters":
        games = Clusters(teams, clusters, goals_per_game)

    SaveMatches(matches_output, games, len(teams))
    SavePowers(team_powers_output, teams)

def main(args):
    # send args as kwargs
    simulate(**args)

if __name__== "__main__":
    description = 'Simulator'
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('--matches_output',
                        default="../data/sim.dat",
                        type=str,
                        help='Filename where matches output is saved.')
    parser.add_argument('--team_powers_output',
                        type=str,
                        default="../data/sim_powers.tsv",
                        help='Filename where team powers output is saved.')
    parser.add_argument('--powers',
                        type=str,
                        default="linear",
                        choices=["linear","exponential"],
                        help='Method used to define teams power.')
    parser.add_argument('--matches',
                        type=str,
                        default="roundrobin",
                        choices=["roundrobin","clusters"],
                        help='Method used to define matches generation.')
    parser.add_argument('--team_matches',
                        type=int,
                        default=GAMES_AGAINST_EACH,
                        help="Amount of matches played by a team.")
    parser.add_argument('--goals_per_game',
                        type=int,
                        default=GOALS_PER_GAME,
                        help="Amount of goals produced in a match.")
    parser.add_argument('--max_power',
                        type=str,
                        default=MAX_POWER,
                        help='Max power for a team.')
    parser.add_argument('--teams',
                        type=int,
                        default=TEAMS,
                        help="Amount of teams for matches.")
    parser.add_argument('--clusters',
                        type=int,
                        default=CLUSTERS,
                        help="Amount of clusters used for teams matches distribution.")
    args = parser.parse_args()

    main(args)
