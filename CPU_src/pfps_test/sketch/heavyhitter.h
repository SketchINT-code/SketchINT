#ifndef _HEAVYHITTER_H
#define _HEAVYHITTER_H

#include <string>
#include <unordered_map>
using namespace std;

#define PH(x) (x - 1)
#define iPH(x) (x + 1)
const int hh_thres = 750;

class HeavyHitter
{
private:
	unordered_map<string, uint16_t> un_map;

public:
	void insert(const char* key, uint16_t key_len, uint32_t f, uint32_t ehn, vector<uint16_t> sid)
	{
		if (f >= hh_thres)
		{
			string s = string(key, key_len);
			char l = '0' + ehn;
			s = s + l;
			uint16_t path = 0;
			for (auto i : sid) path |= 1 << PH(i);
			un_map[s] = path;
		}
	}

	unordered_set<string> getHH(uint16_t row_id)
	{
		unordered_set<string> ret_hh;
		for (auto pr : un_map)
			if(pr.second & 1 << row_id)
				ret_hh.insert(pr.first);
		return ret_hh;
	}
};

#endif
