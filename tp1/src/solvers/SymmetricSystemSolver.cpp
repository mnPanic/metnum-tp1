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

static unsigned int g_seed;

inline void fast_srand(int seed) {
  //Seed the generator
  g_seed = seed;
}
inline int fastrand() {
  g_seed = (214013*g_seed+2531011);
  return (g_seed>>16)&0x7FFF;
}
inline int fastRandInt(int maxSize) {
  return fastrand() % maxSize;
}
inline int fastRandInt(int a, int b) {
  return(a + fastRandInt(b - a));
}
inline double fastRandDouble() {
  return static_cast<double>(fastrand()) / 0x7FFF;
}
inline double fastRandDouble(double a, double b) {
  return a + (static_cast<double>(fastrand()) / 0x7FFF)*(b-a);
}

#define Now() chrono::high_resolution_clock::now()

struct Stopwatch {
	chrono::high_resolution_clock::time_point c_time, c_timeout;
	void setTimeout(int us) { c_timeout = c_time + chrono::microseconds(us); }
	void Start(int limit) { c_time = Now(); setTimeout(limit); }
	inline bool Timeout() { return Now() > c_timeout; }
	long long EllapsedMicroseconds() { return chrono::duration_cast<chrono::microseconds>(Now() - c_time).count(); }
	long long EllapsedMilliseconds() { return chrono::duration_cast<chrono::milliseconds>(Now() - c_time).count(); }
};

namespace SymmetricMatrix{
	
	class Matrix{
		public:
			Matrix(){
				rows = 0;
			}
			Matrix(int _rows){
				rows = _rows;
				
				contiguous_cells = vector<float>((rows*(rows+1)/2));
				cells = vector<float*>(rows,NULL);
				for(int i = 0;i < _rows;i++){
					//cells.push_back(vector<float>(i+1));
					cells[i] = &contiguous_cells[(i*(i+1))/2];
				}
			}
		vector<float*>cells;
		int rows;
		friend ostream& operator<<(ostream& os, const Matrix& m);
		private:
		vector<float>contiguous_cells;
	};

	ostream& operator<<(ostream& os, const Matrix& m)
	{
		for(int y = 0;y < m.rows;y++){
			os << "|";
			for(int x = 0;x < m.rows;x++){
				if(y < x)
					os << m.cells[x][y];
				else
					os << m.cells[y][x];
				if(x<m.rows-1)os << " ";
			}
			os << "|" <<endl;
		}
		return os;
	}

	class System{
		public:
		System(){}
		System(int rows) : A(rows),b(rows,0.0f){}
		Matrix A;
		vector<float>b;
	};

		
	void gaussian_elimination(System& s){
		
		auto& m = s.A;
		auto& b = s.b;
		
		for(int i = m.rows-1;i > 0;i--){
			for(int r = i-1;r >= 0;r--){
				//Substract from row r the row i multiplied by correct coeff
				float coeff = m.cells[i][r]/m.cells[i][i];
				for(int c = 0;c <= r;c++){
					m.cells[r][c] -= m.cells[i][c]*coeff;
				}
				b[r] -= b[i]*coeff;
			}
		}
	}


	vector<float> forward_substitution(System& s){
		auto& m = s.A;
		auto& b = s.b;
		vector<float>x(b.size(),0);
		
		for(int r = 0;r < m.rows;r++){
			
			//Sum up all the coefficients multiplied by ratings already obtained
			float sum = 0;
			for(int c = 0;c < r;c++){
				sum += x[c]*m.cells[r][c];
			}
			/*
			float sum = 0.0;               	// Prepare the accumulator.
			float comp = 0.0;                     // A running compensation for lost low-order bits.
			for(int c = 0;c < r;c++){  // The array input has elements indexed input[1] to input[input.length].
				float y = x[c]*m.cells[r][c] - comp;       // c is zero the first time around.
				float t = sum + y;        // Alas, sum is big, y small, so low-order digits of y are lost.
				comp = (t - sum) - y;            // (t - sum) cancels the high-order part of y; subtracting y recovers negative (low part of y)
				sum = t;                    // Algebraically, c should always be zero. Beware overly-aggressive optimizing compilers!
			}                         // Next time around, the lost low part will be added to y in a fresh attempt.
			*/
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

	vector<float> solve(System& s){
		gaussian_elimination(s);
		auto x = forward_substitution(s);
		return x;
	}
}

void save_data(string filename,vector<float> ratings){
	Stopwatch stopwatch;
	cout << "Dumping data to " << filename << "..." << endl;
	stopwatch.Start(0);
	ofstream myfile;
	myfile.open (filename, ios::trunc | ios::out);
	for(int i = 0;i < ratings.size();i++){
		myfile << ratings[i] << endl;
	}
	myfile.close();
	cout << "Done dumping data in " << stopwatch.EllapsedMilliseconds() << "ms" << endl;
}

int main(int argc, char const *argv[]){
	if(argc != 3){
		cout << "Argument count is wrong!" << endl;
	}
	
	string input_file = string(argv[1]);
	string output_file = string(argv[2]);
	
	Stopwatch stopwatch;
   	cout << "Loading data from " << input_file << "..." << endl;
	stopwatch.Start(0);
	SymmetricMatrix::System s3 = SymmetricMatrix::generate_system(input_file);
    cout << "Done loading data in " << stopwatch.EllapsedMilliseconds() << "ms" << endl;
	//Solve Ax = b system
	cout << "Solving system..." << endl;
	stopwatch.Start(0);
	auto x = SymmetricMatrix::solve(s3);
	cout << "Done solving system in " << stopwatch.EllapsedMilliseconds() << "ms" << endl;
	//Save ratings in output file
	save_data(output_file,x);
	return 0;
}
