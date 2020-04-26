#include "mEntry.h"

mEntry::mEntry(int p0, int p1, bool w, int d) {
	this->_player0 = p0;
	this->_player1 = p1;
	this->_winner = w;
	this->_date = d;
};

mEntry::mEntry(const mEntry& c) {
    this->_player0  = c._player0;
    this->_player1 = c.player1;
    this->_winner = c._winner;
    this->_date = c._date;
}

int mEntry::getPlayer(int p) const {
	if (!p) return this->_player0;
	else return this->_player1;
};

bool mEntry::getWinner() const {
	return this->_winner;
};

int mEntry::getDate() const {
	return this->_date;
};
