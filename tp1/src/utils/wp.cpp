#include <vector>
#include <fstream>
#include <string>

typedef struct s_entry {
    int player0;
    int player1;
    int win;
    int date;
} entry_t;

typedef struct s_info{
    int teams;
    std::vector<entry_t> entires;
} info_t;

void parseData(std::string file, info_t& info) {
    std::ifstream st (file);
    if (st.is_open()) {
        int t, n;
        info.teams = t;

        st >> t >> n;
        int p0,p1,g0,g1,d,w;
        std::vector<entry_t> vec = std::vector<entry_t>();

        for (int i = 0; i < n; i++) {
            st >> d >> p0 >> g0 >> p1 >> g1;
            if (g0 > g1) w = p0;
            else w = p1;
            entry_t e = entry_t{p0, p1, w, d};
            vec.push_back(e);
        }
        info.entires = vec;
    } else {
        printf("Error opening file\n");
    }
    st.close();
}

int main() {
    info_t partidos;
    parseData("../tests/test1.in", partidos);
    double wp[partidos.teams];
    
    for (int i = 0; i < partidos.teams; ++i) {
        int count = 0;
        int wins = 0;
        for (int j = 0 ; j < partidos.entires.size(); ++j) {
            if (partidos.entires[j].player0 == i ||
                partidos.entires[j].player1 == i) {
                    count++;
                    if (partidos.entires[j].win == i) wins++;
                }
        }
        wp[i] = (double)wins/(double)count;
    }
    return 0;
}
