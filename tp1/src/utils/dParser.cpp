const mInfo& dParser::getMatches(std::string file, char d) {
    std::ifstream st (file);
    if (st.is_open()) {
        std::string line;
        std::vector<std::string> r;

        getline(st, line); // first line
        boost::split(r, line, [](char c){return c == d});

        mInfo info = mInfo(std::stoi(r[0]), std::stoi(r[1]));

        while(getline(st, line)) {
            boost::split(r, line, [](char c){return c == d});
            mEntry e = mEntry(std::stoi(r[1]), std::stoi(r[3]),
                (std::stoi(r[2]) < std::stoi(r[4])), std::stoi(r[0]));
            info.addEntry(e);
        }
        return info;
    }
    printf("Error opening file\n");
    return NULL
}