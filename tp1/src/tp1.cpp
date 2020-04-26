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
		System system;

		string line;
		ifstream myfile(filename);
		if (myfile.is_open()){
			int teams,games;
			myfile >> teams >> games;
			system = System(teams);

			for(int i = 0;i < teams;i++)
				system.A.cells[i][i] += 2.0f;

			for(int i = 0;i < games;i++){
				int date,teamI,goalsI,teamJ,goalsJ;
				myfile >> date >> teamI >> goalsI >> teamJ >> goalsJ;
				teamI-=1;
				teamJ-=1;
				system.A.cells[teamI][teamI] += 1.0f;
				system.A.cells[teamJ][teamJ] += 1.0f;
				system.A.cells[teamI][teamJ] -= 1.0f;
				system.A.cells[teamJ][teamI] -= 1.0f;
				system.b[teamI] += (goalsI > goalsJ)*1.0f - (goalsI < goalsJ)*1.0f;
				system.b[teamJ] += (goalsJ > goalsI)*1.0f - (goalsJ < goalsI)*1.0f;
			}

			for(int i = 0;i < teams;i++)
				system.b[i] = 1.0f + system.b[i]/2.0f;

			myfile.close();
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
