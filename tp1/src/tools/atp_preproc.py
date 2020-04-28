import csv

input_path = '../../data/atp_matches_2015.dat'
output_path = '../../data/atp_matches_2015.dat.norm'

name2idx = {}

with open(input_path,'r') as csvfile, open(output_path,'w') as outfile:
    score_reader = csv.reader(csvfile, delimiter=' ', skipinitialspace=True)
    teams, n_games = next(score_reader)
    out_str = ""
    for date, team_1, score_1, team_2, score_2 in score_reader:
        if team_1 not in name2idx: name2idx[team_1] = len(name2idx) + 1
        if team_2 not in name2idx: name2idx[team_2] = len(name2idx) + 1

        out_str += f"{date} {name2idx[team_1]} {score_1} {name2idx[team_2]} {score_2}\n"

    # Write number of teams and games
    outfile.write(teams + ' ' + n_games + '\n')
    # Write games details
    outfile.write(out_str)

    print("Done")
