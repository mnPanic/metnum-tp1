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
TYPE_WP = "1"
TYPE_CMM = "0"

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

    print(f"running {BIN} with input file {path}, method: {method}")

    return subprocess.run([BIN, path, path+typ+OUT, typ])

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
