#pragma GCC optimize("Ofast","unroll-loops", "omit-frame-pointer", "inline")

#include <limits>
#include <iostream>
#include <vector>
#include <assert.h>
#include <fstream>
#include <string>
#include <map>

using namespace std;

#define CMM 0
#define WP 1
#define OPTIONAL 2

typedef vector<double> Ratings;

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
		// generate the system Cr = b
		// where
		//
		//	C_ij = 	-n_ij 		if i != j
		//			2 + n_i		if i = j
		//
		//	b_i = 1 + (w_i + l_i)/2
		//
		//	i,j teams from (1, ..., N) 

		System system;

		ifstream myfile(filename);
		if (!myfile.is_open()){
			std::cout << "Unable to open file: " << filename << std::endl;
		}

		int teams, games;
		myfile >> teams >> games;
		system = System(teams);

		// Ya que los equipos pueden tener numeros de identificacion
		// arbitrarios, es necesario hacer un mapeo entre eso y
		// en indice resultante en la matriz.
		map<int, int> team2idx;

		for(int i = 0;i < teams;i++) {
			system.A.cells[i][i] += 2.0;
		}

		for(int i = 0;i < games;i++){
			int date, teamIName, goalsI, teamJName, goalsJ;
			myfile >> date >> teamIName >> goalsI >> teamJName >> goalsJ;

			// Si no tienen un mapeo, entonces sera por su numero de aparicion
			if(!team2idx.count(teamIName)) {team2idx[teamIName] = team2idx.size(); }
			if(!team2idx.count(teamJName)) {team2idx[teamJName] = team2idx.size(); }

			int teamI = team2idx[teamIName];
			int teamJ = team2idx[teamJName];

			system.A.cells[teamI][teamI] += 1.0;
			system.A.cells[teamJ][teamJ] += 1.0;
			system.A.cells[teamI][teamJ] -= 1.0;
			system.A.cells[teamJ][teamI] -= 1.0;
			system.b[teamI] += (goalsI > goalsJ)*1.0 - (goalsI < goalsJ)*1.0;
			system.b[teamJ] += (goalsJ > goalsI)*1.0 - (goalsJ < goalsI)*1.0;
		}

		for(int i = 0;i < teams;i++) {
			system.b[i] = 1.0 + system.b[i]/2.0;
		}

		myfile.close();
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
