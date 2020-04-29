import random

TEAMS = 5
MAX_POWER = 1000
GAMES_AGAINST_EACH = 5
GOALS_PER_GAME = 3
CLUSTERS = 5

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

def RunGame(team1,team2):
    wins_t1 = 0
    wins_t2 = 0
    for goals in range(0,GOALS_PER_GAME):
        goal_chance = random.randint(1,team1.power+team2.power)
        win_t1 = (goal_chance <= team1.power)
        wins_t1 = wins_t1 + win_t1
        wins_t2 = wins_t2 + 1-win_t1

    return Game(team1.index,wins_t1,team2.index,wins_t2)

def RoundRobin(teams):
    games = []

    for team1 in teams:
        for team2 in teams:
            if team1 != team2:
                for g in range(0,GAMES_AGAINST_EACH):
                   games.append(RunGame(team1,team2))

    return games

def Clusters(teams):
    games = []
    team_cluster = {}

    for team in teams:
        team_cluster[team.index] = random.randint(0,CLUSTERS)

    for team1 in teams:
        #cluster1 = int(team1.index*(TEAMS/CLUSTERS))
        for team2 in teams:
            #cluster2 = int(team1.index*(TEAMS/CLUSTERS))
            if team1.index != team2.index and team_cluster[team1.index] == team_cluster[team2.index]:
                for g in range(0,GAMES_AGAINST_EACH):
                    games.append(RunGame(team1,team2))

    return games

def LinearPowers():

    teams_powers = []

    for i in range(1,TEAMS+1):
        teams_powers.append(Team(i,i*(MAX_POWER/TEAMS)))

    return teams_powers

def ExponentialPowers():

    teams_powers = []

    for i in range(1,TEAMS+1):
        teams_powers.append(Team(i,int(pow(MAX_POWER,i/TEAMS))))

    return teams_powers

def SaveMatches(filename,games):
    with open(filename, 'w') as f:
        for game in games:
            print(game, file=f)

def SavePowers(filename,teams):
    with open(filename, 'w') as f:
        for team in teams:
            print(team, file=f)

def main():
    teams = LinearPowers()
    print(teams)
    games = RoundRobin(teams)
    SaveMatches("matches_roundrobin.tsv",games)
    SavePowers("powers_linear.out",teams)
    teams2 = ExponentialPowers()
    print(teams2)
    games2 = Clusters(teams2)
    SaveMatches("matches_clusters.tsv",games2)
    SavePowers("powers_exp.out",teams2)
 
main()
