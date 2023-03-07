#ifndef _CSKETCH_H
#define _CSKETCH_H

#include <algorithm>
#include <cstring>
#include <string.h>
//#include "params.h"
//#include "BOBHash.h"
#include <iostream>
#include <algorithm>
#include "murmur3.h"

using namespace std;

const uint32_t d=5;
class CSketch
{	
private:
	//BOBHash * bobhash[MAX_HASH_NUM * 2];
	uint32_t hashseed[d*2];

	int index[d];
	int *counter[d];
	int w;
	//int MAX_CNT, MIN_CNT;
	//int counter_index_size;
	uint64_t hash_value;


public:
	CSketch(int _w)
	{
		//counter_index_size = 20;
		w = _w;
		
		for(int i = 0; i < d; i++)	
		{
			counter[i] = new int[w];
			memset(counter[i], 0, sizeof(int) * w);
		}

		for(int i = 0; i < d * 2; i++)
		{
			hashseed[i] = rand() % MAX_PRIME32;
		}
	}
	void insert(const char * str, uint16_t key_len)
	{
		int g = 0;
		for(int i = 0; i < d; i++)
		{
			index[i] = MurmurHash3_x86_32(str, key_len, hashseed[i]) % w;
			g = MurmurHash3_x86_32(str, key_len, hashseed[i+d]) % 2;

			if(g == 0)
			{
				counter[i][index[i]]++;
			}
			else
			{
				counter[i][index[i]]--;
			}
		}
	}

	double query(const char *str, uint16_t key_len)
	{
		int temp;
		int res[d];
		int g;
		for(int i = 0; i < d; i++)
		{
			index[i] = MurmurHash3_x86_32(str, key_len, hashseed[i]) % w;
			temp = counter[i][index[i]];
			g = MurmurHash3_x86_32(str, key_len, hashseed[i+d]) % 2;

			res[i] = (g == 0 ? temp : -temp);
		}

		sort(res, res + d);
		int r;
		if(d % 2 == 0)
		{
			r = (res[d / 2] + res[d / 2 - 1]) / 2;
		}
		else
		{
			r = res[d / 2];
		}
		return r;
	}
	~CSketch()
	{
		for(int i = 0; i < d; i++)	
		{
			delete []counter[i];
		}


		/*for(int i = 0; i < d * 2; i++)
		{
			delete hashseed[i];
		}*/
	}
};
#endif//_CSKETCH_H