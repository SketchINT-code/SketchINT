#ifndef _CM_H
#define _CM_H

#include <cstring>
#include "params.h"
#include "murmur3.h"

class CMSketch
{
protected:
	uint32_t w;
	uint32_t* A[d];
	uint32_t hashseed[d];
	int idx[d];

public:
	CMSketch() {}
	CMSketch(uint32_t w_d) { init(w_d); }
	~CMSketch() { clear(); }

	void init(uint32_t w_d)
	{
		w = w_d;
		for (int i = 0; i < d; ++i)
		{
			A[i] = new uint32_t[w_d];
			memset(A[i], 0, w_d * sizeof(uint32_t));
			hashseed[i] = rand() % MAX_PRIME32;
		}
	}

	void clear()
	{
		for (int i = 0; i < d; ++i)
			delete[]A[i];
	}

	void insert(const char* key, uint16_t key_len)
	{
		for (int i = 0; i < d; ++i)
			idx[i] = MurmurHash3_x86_32(key, key_len, hashseed[i]) % w;
		for (int i = 0; i < d; ++i)
			++A[i][idx[i]];
	}

	uint32_t query(const char* key, uint16_t key_len)
	{
		uint32_t ret = UINT32_MAX;
		for (int i = 0; i < d; ++i)
		{
			uint32_t idx = MurmurHash3_x86_32(key, key_len, hashseed[i]) % w;
			ret = (A[i][idx] < ret) ? A[i][idx] : ret;
		}
		return ret;
	}
};

class CUSketch : public CMSketch
{
public:
	CUSketch() {}
	CUSketch(uint32_t w_d) { init(w_d); }
	~CUSketch() {}

	void insert(const char* key, uint16_t key_len)
	{
		uint32_t min_val = UINT32_MAX;
		for (int i = 0; i < d; ++i)
		{
			idx[i] = MurmurHash3_x86_32(key, key_len, hashseed[i]) % w;
			min_val = (A[i][idx[i]] < min_val) ? A[i][idx[i]] : min_val;
		}
		for (int i = 0; i < d; ++i)
			A[i][idx[i]] += (A[i][idx[i]] == min_val) ? 1 : 0;
	}
};

#endif
