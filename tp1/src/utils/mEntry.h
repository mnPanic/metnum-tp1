#ifndef _M_ENTRY_
#define _M_ENTRY_

class mEntry {
public:
	mEntry(int,int,bool,int);
    mEntry(const mEntry&)
	int getPlayer(int) const;
	int getDate() const;
	bool getWinner() const;
private:
	int _player0;
	int _player1;
	bool _winner; // 0 : Player0 , 1 -> Player1
	int _date;
};

#endif // _M_ENTRY_
