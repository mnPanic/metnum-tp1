#pragma GCC optimize("Ofast","unroll-loops", "omit-frame-pointer", "inline")
//#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
//#pragma GCC target("rdrnd", "popcnt", "avx", "bmi2")

#include <limits>
#include <iostream>
#include <vector>
#include <assert.h>
#include <fstream>
#include <string>
#include <chrono>

#include <emmintrin.h>
#include <immintrin.h>

using namespace std;

#define CMM 0
#define WP 1
#define OPTIONAL 2

typedef vector<double> Ratings;

#define Now() chrono::high_resolution_clock::now()

struct Stopwatch {
	chrono::high_resolution_clock::time_point c_time, c_timeout;
	void setTimeout(int us) { c_timeout = c_time + chrono::microseconds(us); }
	void Start(int limit) { c_time = Now(); setTimeout(limit); }
	inline bool Timeout() { return Now() > c_timeout; }
	long long EllapsedMicroseconds() { return chrono::duration_cast<chrono::microseconds>(Now() - c_time).count(); }
	long long EllapsedMilliseconds() { return chrono::duration_cast<chrono::milliseconds>(Now() - c_time).count(); }
};

namespace CMM_method{
	
	class Matrix{
		public:
			Matrix(){
				rows = 0;
			}
			Matrix(int _rows){
				rows = _rows;
				
				contiguous_cells = vector<double>((rows*(rows+1)/2));
				cells = vector<double*>(rows,NULL);
				for(int i = 0;i < _rows;i++){
					//cells.push_back(vector<double>(i+1));
					cells[i] = &contiguous_cells[(i*(i+1))/2];
				}
			}
		vector<double*>cells;
		int rows;
		private:
		vector<double>contiguous_cells;
	};

	class System{
		public:
		System(){}
		System(int rows) : A(rows),b(rows,0.0f){}
		Matrix A;
		vector<double>b;
	};

		
	void gaussian_elimination(System& s){
		
		auto& m = s.A;
		auto& b = s.b;
		
		for(int i = m.rows-1;i > 0;i--){
			for(int r = i-1;r >= 0;r--){
				//Substract from row r the row i multiplied by correct coeff
				double coeff = m.cells[i][r]/m.cells[i][i];
				for(int c = 0;c <= r;c++){
					m.cells[r][c] -= m.cells[i][c]*coeff;
				}
				b[r] -= b[i]*coeff;
			}
		}
	}


	vector<double> forward_substitution(System& s){
		auto& m = s.A;
		auto& b = s.b;
		vector<double>x(b.size(),0);
		
		for(int r = 0;r < m.rows;r++){
			
			//Sum up all the coefficients multiplied by ratings already obtained
			double sum = 0;
			for(int c = 0;c < r;c++){
				sum += x[c]*m.cells[r][c];
			}
			
			//Find next rating value
			x[r] = (b[r]-sum)/m.cells[r][r];
		}
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

				if(teamI < teamJ){
					swap(teamI,teamJ);
					swap(goalsI,goalsJ);
				}

				system.A.cells[teamI][teamJ] -= 1.0f;
				system.A.cells[teamI][teamI] += 1.0f;
				system.A.cells[teamJ][teamJ] += 1.0f;

				system.b[teamI] += (goalsI > goalsJ)*1.0f - (goalsI < goalsJ)*1.0f;
				system.b[teamJ] += (goalsJ > goalsI)*1.0f - (goalsJ < goalsI)*1.0f;
			}
			
			for(int i = 0;i < teams;i++)
				system.b[i] = 1.0f + system.b[i]/2.0f;
				
			myfile.close();
		}
		return system;
	}

	Ratings solve(System& s){
		gaussian_elimination(s);
		Ratings r = forward_substitution(s);
		return r;
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

	Stopwatch stopwatch;

	switch(ranking_algorithm){
		case CMM:
		{
     		cout << "Running CMM Ranking method" << endl;
			cout << "Loading data from " << input_file << "..." << endl;
			stopwatch.Start(0);
			CMM_method::System system = CMM_method::generate_system(input_file);
    		cout << "Done loading data in " << stopwatch.EllapsedMilliseconds() << "ms" << endl;
					
			cout << "Solving system..." << endl;
			stopwatch.Start(0);
			ratings = CMM_method::solve(system);
			cout << "Done solving system in " << stopwatch.EllapsedMilliseconds() << "ms" << endl;
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

	cout << "Dumping data to " << output_file << "..." << endl;
	stopwatch.Start(0);
	save_data(output_file,ratings);
	cout << "Done dumping data in " << stopwatch.EllapsedMilliseconds() << "ms" << endl;

	return 0;
}
