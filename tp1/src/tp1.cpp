#pragma GCC optimize("Ofast","unroll-loops", "omit-frame-pointer", "inline")

#include <limits>
#include <iostream>
#include <vector>
#include <assert.h>
#include <fstream>
#include <string>

using namespace std;

#define CMM 0
#define WP 1
#define OPTIONAL 2

typedef vector<double> Ratings;

struct entry {
    int player0;
    int player1;
    int win;
    int date;
};

struct info{
    int teams;
    std::vector<entry> entries;
};

void parseData(std::string file, info& info) {
    std::ifstream st (file);
    if (!st.is_open()) {
        printf("Error opening file\n");
        exit(EXIT_FAILURE);
    }
    int t, n;
    st >> t >> n;

    info.teams = t;
    int p0,p1,g0,g1,d,w;

    for (int i = 0; i < n; i++) {
        st >> d >> p0 >> g0 >> p1 >> g1;
        if (g0 > g1) w = p0-1;
        else w = p1-1;
        entry e = entry{p0-1, p1-1, w, d};
        info.entries.push_back(e);
    }
    st.close();
}

void wp (std::string file, std::vector<double>& results) {
    info partidos{};
    parseData(file, partidos);
    
    for (int i = 0; i < partidos.teams; ++i) {
        int count = 0;
        int wins = 0;

        for (int j = 0 ; j < partidos.entries.size(); ++j) {
            if (partidos.entries[j].player0 == i ||
                partidos.entries[j].player1 == i) {
                    count++;
                    if (partidos.entries[j].win == i) {
                        wins++;
                    }
                }
        }
        results.push_back((double)wins/(double)count);
    }
}

namespace CMM_method{
	
	class Matrix{
		public:
			Matrix(){
				rows = 0;
				cols = 0;
			}
			Matrix(int _rows,int _cols){
				rows = _rows;
				cols = _cols;

				cells = vector<vector<double>>(rows,vector<double>(cols,0));
			}
		vector<vector<double>>cells;
		int rows;
		int cols;
	};

	class System{
		public:
		System(){}
		System(int rows) : A(rows,rows),b(rows,0.0f){}
		Matrix A;
		vector<double>b;
	};

	void gaussian_elimination(System& s){

		auto& m = s.A;
		auto& b = s.b;

		for(int i = 0;i < m.rows;i++){
			for(int r = i+1;r < m.rows;r++){
				//Substract from row r the row i multiplied by correct coeff
				double coeff = m.cells[r][i]/m.cells[i][i];
				for(int c = i;c < m.cols;c++){
					m.cells[r][c] -= m.cells[i][c]*coeff;
				}
				b[r] -= b[i]*coeff;
			}
		}
	}

	Ratings back_substitution(System& s){
		auto& m = s.A;
		auto& b = s.b;
		Ratings x(b.size(),0);

		for(int r = m.rows-1;r >= 0;r--){

			//Sum up all the coefficients multiplied by ratings already obtained
			double sum = 0;
			for(int c = r+1;c < m.cols;c++){
				sum += x[c]*m.cells[r][c];
			}

			//Find next rating value
			x[r] = (b[r]-sum)/m.cells[r][r];
		}
		return x;
	}

	Ratings solve(System& s){
		gaussian_elimination(s);
		auto x = back_substitution(s);
		return x;
	}

	System generate_system(string filename){
		//System system;
        info partidos {};

        parseData(filename, partidos);
        System system(partidos.teams);

        for (int i = 0; i < partidos.teams; ++i) {
            system.A.cells[i][i] += 2.0;
        }

        for (int i = 0; i < partidos.entries.size(); ++i) {
            int p0 = partidos.entries[i].player0;
            int p1 = partidos.entries[i].player1;

            system.A.cells[p0][p0] += 1.0;
            system.A.cells[p1][p1] += 1.0;
            system.A.cells[p0][p1] -= 1.0;
            system.A.cells[p1][p0] -= 1.0;

            if (partidos.entries[i].win == p0) {
                system.b[p0] += 1.0;
                system.b[p1] -= 1.0;
            } else {
                system.b[p1] += 1.0;
                system.b[p0] -= 1.0;
            }
        }

        for (int i = 0; i < partidos.teams; i++) {
            system.b[i] = 1.0 + system.b[i] / 2.0;
        }

        return system;
    }
}

void save_data(string filename,Ratings ratings){
	ofstream myfile;
	myfile.open (filename, ios::trunc | ios::out);
	for(int i = 0;i < ratings.size();i++){
		myfile << ratings[i] << endl;
	}
	myfile.close();
}


int main(int argc, char const *argv[]){

	if(argc != 4){
		cout << "Argument count is wrong!" << endl;
        exit(EXIT_FAILURE);
	}
	
	string input_file = string(argv[1]);
	string output_file = string(argv[2]);
	int ranking_algorithm = stoi(argv[3]);

	Ratings ratings;

	switch(ranking_algorithm){
		case CMM:
		{
			CMM_method::System system = CMM_method::generate_system(input_file);
			ratings = CMM_method::solve(system);
		}
		break;
		case WP:
            wp(input_file, ratings);
		break;
		case OPTIONAL:
		break;
		default:
			cout << "Invalid ranking algorithm" << endl;
			return 1;
		break;
	}

	save_data(output_file,ratings);

	return 0;
}
