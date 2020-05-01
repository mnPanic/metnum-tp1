import subprocess
import pandas as pd

# Files
DATA_DIR = "../data/"
NBA_SCORES = DATA_DIR + "nba_2016_scores.dat"
ATP_SCORES = DATA_DIR + "atp_matches_2015.dat.norm"

NBA_NAMES = DATA_DIR + "nba_2016_teams.csv"
ATP_NAMES = DATA_DIR + "atp_players.csv"

# Extension de resultados
OUT = ".out"

# Binario del TP
BIN = "../src/tp"

# Metodos
METHOD_WP = "1"
METHOD_CMM = "0"

def run(path: str, method: str, out: str=None):
    """
    Corre un programa y retorna el output.

    - path: El path al archivo que contiene los datos a partir de los cuales
      calcular en ranking, en el formato provisto por la catedra.
    - method: El metodo a utilizar (0: CMM, 1: WP)
    - out: El archivo en el cual almacenar el ranking resultante. Por default
      se utiliza `path`+method+".out"
    """

    if out is None: out = path + method + OUT

    #print(f"running {BIN} with input file {path}, method: {method}")

    return subprocess.run([BIN, path, out, method])

def read_mn(path: str) -> pd.DataFrame:
    """
    Lee un archivo de datos en el formato provisto por la catedra, y devuelve
    un DataFrame de pandas conteniendolo. Muy util para hacer graficos.
    """

    return pd.read_csv(
        path,
        sep=' ',
        skiprows=2, # la primera row contiene cant teams y cant partidos
        # Headers custom porque el formato de la catedra no lo tiene
        header=None,
        names=["date", "team1", "score1", "team2", "score2"],
    )

def read_ranking(path: str) -> pd.DataFrame:
    """
    Lee los rankings resultantes de correr el binario del tp, suponiendo que
    esta en el formato definido por la catedra. Devuelve un pd.DataFrame.
    """

    return pd.read_csv(
        path,
        sep=' ',
        header=None,
        names=["rating"],
    )

def analyze_data(path: str) -> pd.DataFrame:
    df = read_mn(path)
    
    teams = {} # {team : { "percent": percent played, "total": total played, "wins": games won, "played": set of teams}}

    for i, row in df.iterrows():
        date, team1, score1, team2, score2 = row
        if team1 not in teams: teams[team1] = {"total": 0, "wins": 0, "opponents": set()}
        if team2 not in teams: teams[team2] = {"total": 0, "wins": 0, "opponents": set()}


        teams[team1]["total"] += 1
        teams[team1]["wins"] += 1 if score1 > score2 else 0
        teams[team1]["opponents"].add(team2) # wont add if present
        
        teams[team2]["total"] += 1
        teams[team2]["wins"] += 1 if score2 > score1 else 0
        teams[team2]["opponents"].add(team1) # wont add if present
    
    total_teams = len(teams)
    
    max_games_played = -1
    for vals in teams.values():
        if vals["total"] > max_games_played:
            max_games_played = vals["total"]

    data = pd.DataFrame(columns=["team", "share", "total", "wins", "participation"])
    for i, team in enumerate(teams.keys()):
        # calculate %
        teams[team]["percent"] = len(teams[team]["opponents"]) / (total_teams - 1)
        teams[team]["participation"] = teams[team]["total"] / max_games_played

        # add to pandas dataframe
        data.loc[i] = [
            int(team), 
            teams[team]["percent"], 
            teams[team]["total"],
            teams[team]["wins"],
            teams[team]["participation"]
        ]
    
    return data