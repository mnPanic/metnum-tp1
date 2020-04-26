#ifndef _M_INFO_
#define _M_INFO_
#include "mEntry.h"
#include <vector>

class mInfo {
public:
	mInfo(int,int);
	void addEntry(const mEntry&);
	int size() const;
	int nTeams() const;
	const mEntry& operator[](int) const;
    ~mInfo();

private:
	int _size;
	int _nTeams;
	std::vector<mEntry*> _mEntries;
}
#endif // _M_INFO_
