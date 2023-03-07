#ifndef _nitrosketch_H
#define _nitrosketch_H

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include <random>
//#include "BOBHash.h"
//#include "params.h"
//#include "ssummary.h"
//#include "BOBHASH64.h"
#include "murmur3.h"
#define ns_d 3
#define rep(i,a,n) for(int i=a;i<=n;i++)
using namespace std;

const uint32_t d = 5;
class Nitrosketch
{
private:
	
	
	//BOBHash * bobhash[d];
	
	int next_packet, next_bucket;
	std::default_random_engine generator;
	double prob;

	uint32_t hashseed[ns_d];
	//int idx[d];

public:
	int *counter[ns_d];
	int w;
	Nitrosketch(int _w, double _prob){ 

		w = _w;  prob=_prob;
		
		next_packet = 1; next_bucket = 0;
		for (int i=0; i<ns_d; i++){
			counter[i] = new int[w];
			memset(counter[i], 0, sizeof(int) * w);	
			//bobhash[i] = new BOBHash(i + 1000);	
			hashseed[i] = rand() % MAX_PRIME32;
		}
	}
	void clear()
	{
		for (int i = 0; i < ns_d; i++)
			memset(counter[i], 0, sizeof(int) * w);	
	}
	void insert(const char * str, uint16_t key_len)
	{
		unsigned int hash[ns_d];
		next_packet--;
		if (next_packet == 0) {
			for (int i = 0; i < ns_d; i++)
				hash[i]=MurmurHash3_x86_32(str, key_len, hashseed[i]) % w;
				//hash[i]=(bobhash[i]->run(str, strlen(str))) % w;
			int i;
			for(;;) {
				i = next_bucket;
				double delta = 1.0/prob*(2*(int)(hash[i]&1)-1);
				counter[i][hash[i]] += (int)delta;

				int sample = 1;
  				if (prob < 1.0) {
 				   std::geometric_distribution<int> dist(prob);
 				   sample = 1 + dist(generator);
 				}

				next_bucket = next_bucket + sample;
				next_packet = next_bucket / ns_d;
				next_bucket %= ns_d;
				if (next_packet > 0)
					break;
			}
		}
	}
	double query(const char * str, uint16_t key_len)
	{
		int maxv = 0;
		unsigned int hash[ns_d];
		int values[ns_d];
		for (int i = 0; i < ns_d; i++)
			hash[i]=MurmurHash3_x86_32(str, key_len, hashseed[i]) % w;
			//hash[i]=(bobhash[i]->run(str, strlen(str))) % w;

		for(int i = 0; i < ns_d; i++)
		{
			values[i] = counter[i][hash[i]]*(2*(hash[i]&1)-1);
		}
		sort(values, values + ns_d);
		if (ns_d & 1)
			maxv = std::abs(values[ns_d/2]);
		else
			maxv = std::abs((values[ns_d/2-1] + values[ns_d/2]) / 2);
		return maxv;
	}
	~Nitrosketch()
	{ 
		for(int i = 0; i < ns_d; i++)	
		{
			delete []counter[i];
		}


		for(int i = 0; i < ns_d; i++)
		{
			//delete bobhash[i];
		}
	}
};
#endif
