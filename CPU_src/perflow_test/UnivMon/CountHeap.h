#ifndef COUNTHEAP_H
#define COUNTHEAP_H

#include "CSketch.h"
#include <map>

using namespace std;

class CountHeap
{
private:
	map<string, int> mp;

	CSketch* sketch;
	int heap_size;

	double heavy_ratio = 0.25;
    double light_ratio = 0.75;

public:
	CountHeap(int memory_in_bytes){
		int w = memory_in_bytes * light_ratio / 4 / d;
		//int w = memory_in_bytes / 4 / d;
		int heap_size = memory_in_bytes * heavy_ratio /
							 ((sizeof(uint32_t) + 4) * 2 + sizeof(uint32_t) + sizeof(uint32_t));

		sketch = new CSketch(w);
		//heap_size = size;
	}
	~CountHeap(){
		delete sketch;
	}
	void insert(const char * str, uint16_t key_len){
		sketch->insert(str, key_len);
		int estimate = sketch->query(str, key_len);

		string s = string(str, 4);

		mp[s]=estimate;

		if (mp.size()>heap_size){
			mp.erase(mp.begin());
		}
	}

	int query(const char * str, uint16_t key_len){
		string s = string(str, 4);
		int ret = mp[s];
		if (ret!=0){
			return ret;
		}
		return sketch->query(str, key_len);
	}



};
#endif
