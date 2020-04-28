#include <vector>
#include <fstream>
#include <string>

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
    info partidos;
    parseData(file, partidos);
    
    for (int i = 0; i < partidos.teams; ++i) {
        int count = 0;
        int wins = 0;
        for (int j = 0 ; j < partidos.entries.size(); ++j) {
            if (partidos.entries[j].player0 == i ||
                partidos.entries[j].player1 == i) {
                    count++;
                    if (partidos.entries[j].win == i) wins++;
                }
        }
        results.push_back((double)wins/(double)count);
    }
}


int main() {
        return 0;
}
