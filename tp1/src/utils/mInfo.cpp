#include "mInfo.h"

mInfo::mInfo(int s, int t) {
    this->_size = s;
    this->_nTeams = t;
};

mInfo::addEntry(const mEntry& e) {
	this->_mEntries.push_back(new mEntry(e));

};

int mInfo::size() const {
	return this->_size;
};

int mInfo::nTeams() const {
	return this->_nTeams;
};

// Pre {i < numMatches()}
const mEntry& mInfo::operator[](int i) const {
	return *(this->_mEntries[i]);
};

mInfo::~mInfo() {
    for (int i = 0; i < this->_size, i++) {
        delete this->_mEntries[i];
    }
}
