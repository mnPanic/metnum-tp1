#pragma GCC optimize "Ofast,omit-frame-pointer,inline"
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")

#include <iostream>
#include <vector>
#include <assert.h>
#include <fstream>
#include <string>
#include <chrono>

#include <emmintrin.h>

using namespace std;

#define Now() chrono::high_resolution_clock::now()

struct Stopwatch {
	chrono::high_resolution_clock::time_point c_time, c_timeout;
	void setTimeout(int us) { c_timeout = c_time + chrono::microseconds(us); }
	void Start(int limit) { c_time = Now(); setTimeout(limit); }
	inline bool Timeout() { return Now() > c_timeout; }
	long long EllapsedMicroseconds() { return chrono::duration_cast<chrono::microseconds>(Now() - c_time).count(); }
	long long EllapsedMilliseconds() { return chrono::duration_cast<chrono::milliseconds>(Now() - c_time).count(); }
};

class Matrix{
	public:
		Matrix(){
			rows = 0;
			cols = 0;
			cells = NULL;
		}
		Matrix(int _rows,int _cols){
			rows = _rows;
			cols = _cols;
			
			float* ptr = new float[rows*cols];
			cells = new float*[rows];
			
			for(int i = 0;i < rows;i++)
				cells[i] = &ptr[i*cols];
				
			for(int y = 0;y < rows;y++){
				for(int x = 0;x < cols;x++){
					cells[y][x] = 0;
				}
			}
		}
		
		Matrix(const Matrix& m){
			if(m.cells!=NULL){
				rows = m.rows;
				cols = m.cols;
				
				float* ptr = new float[rows*cols];
				cells = new float*[rows];
				
				for(int i = 0;i < rows;i++)
					cells[i] = &ptr[i*cols];
					
				for(int y = 0;y < rows;y++){
					for(int x = 0;x < cols;x++){
						cells[y][x] = m.cells[y][x];
					}
				}
			}
		}
		
		~Matrix(){
			if(cells!=NULL){
				delete &cells[0][0];
				delete cells;
			}
		}
		
		Matrix& operator=(const Matrix& o){
			if(cells != NULL){
				delete &cells[0][0];
				delete cells;
			}
			if(o.cells!=NULL){
				rows = o.rows;
				cols = o.cols;
				
				float* ptr = new float[rows*cols];
				cells = new float*[rows];
				
				for(int i = 0;i < rows;i++)
					cells[i] = &ptr[i*cols];
					
				for(int y = 0;y < rows;y++){
					for(int x = 0;x < cols;x++){
						cells[y][x] = o.cells[y][x];
					}
				}
			}
		}
	float **cells;
	int rows;
	int cols;
	friend ostream& operator<<(ostream& os, const Matrix& m);
};

ostream& operator<<(ostream& os, const Matrix& m)
{
    for(int y = 0;y < m.rows;y++){
    	os << "|";
		for(int x = 0;x < m.cols;x++){
			os << m.cells[y][x];
			if(x<m.cols-1)os << " ";
		}
		os << "|" <<endl;
	}
    return os;
}

class System{
	public:
	System(){}
	System(int rows) : A(rows,rows),b(rows,0.0f){}
	Matrix A;
	vector<float>b;
	friend ostream& operator<<(ostream& os, const System& s);
};

ostream& operator<<(ostream& os, const System& s)
{
    for(int y = 0;y < s.A.rows;y++){
		for(int x = 0;x < s.A.cols;x++){
			os << s.A.cells[y][x];
			if(x<s.A.cols-1)os << " ";
		}
		os << " | " << s.b[y] << endl;
	}
    return os;
}

void gaussian_elimination(System& s){
	
	auto& m = s.A;
	auto& b = s.b;
	
	for(int i = 1;i < m.rows;i++){
		for(int r = i;r < m.rows;r++){
			//Substract from row r the row i multiplied by correct coeff
			float coeff = m.cells[r][i-1]/m.cells[i-1][i-1];
			for(int c = i-1;c < m.cols;c++){
				m.cells[r][c] -= m.cells[i-1][c]*coeff;
			}
			b[r] -= b[i-1]*coeff;
		}
	}
}

vector<float> back_substitution(System& s){
	auto& m = s.A;
	auto& b = s.b;
	vector<float>x(b.size(),0);
	
	for(int r = m.rows-1;r >= 0;r--){
		
		//Sum up all the coefficients multiplied by ratings already obtained
		float sum = 0;
		for(int c = r+1;c < m.cols;c++){
			sum += x[c]*m.cells[r][c];
		}
		
		//Find next rating value
		x[r] = (b[r]-sum)/m.cells[r][r];
	}
	return x;
}

vector<float> solve(System& s){
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

void save_data(string filename,vector<float> ratings){
	ofstream myfile;
	myfile.open (filename, ios::trunc | ios::out);
	for(int i = 0;i < ratings.size();i++){
		myfile << ratings[i] << endl;
	}
	myfile.close();
}

int main(int argc, char const *argv[]){
	if(argc != 3){
		cout << "Argument count is wrong!" << endl;
	}
	
	string input_file = string(argv[1]);
	string output_file = string(argv[2]);
	
	Stopwatch stopwatch;
	//Load data from input file
	cout << "Loading data from " << input_file << "..." << endl;
	stopwatch.Start(0);
	System s = generate_system(input_file);
	cout << "Done loading data in " << stopwatch.EllapsedMilliseconds() << "ms" << endl;
	//Solve Ax = b system
	cout << "Solving system..." << endl;
	stopwatch.Start(0);
	auto x = solve(s);
	cout << "Done solving system in " << stopwatch.EllapsedMilliseconds() << "ms" << endl;
	//Save ratings in output file
	cout << "Dumping data to " << output_file << "..." << endl;
	stopwatch.Start(0);
	save_data(output_file,x);
	cout << "Done dumping data in " << stopwatch.EllapsedMilliseconds() << "ms" << endl;
	return 0;
}
