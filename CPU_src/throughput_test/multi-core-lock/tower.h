#ifndef _TOWER_H
#define _TOWER_H

#include <cstring>
#include <mutex>
#include "params.h"
#include "murmur3.h"

using namespace std;

const int mtx_num = 1 << 10;
const int mtx_mask = 0x3;
const int mtx_shift = 2;

class TowerSketch
{
protected:
	uint32_t w[d];
	uint32_t* A[d];
	uint32_t hashseed[d];
	mutex mtx[mtx_num];

public:
	TowerSketch() {}
	TowerSketch(uint32_t w_d) { init(w_d); }
	~TowerSketch() { clear(); }

	void init(uint32_t w_d)
	{
		for (int i = 0; i < d; ++i)
		{
			w[i] = w_d << d - i - 1;
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
		int idx[d], mtx_id = 0;
		for (int i = 0; i < d; ++i)
		{
			idx[i] = MurmurHash3_x86_32(key, key_len, hashseed[i]) % w[i];
			mtx_id = (mtx_id << mtx_shift) | (idx[i] >> cpw[i] & mtx_mask);
		}
		mtx[mtx_id].lock();
		for (int i = 0; i < d; ++i)
		{
			uint32_t a = A[i][idx[i] >> cpw[i]];
			uint32_t shift = (idx[i] & lo[i]) << cs[i];
			uint32_t val = (a >> shift) & mask[i];
			A[i][idx[i] >> cpw[i]] += (val < mask[i]) ? (1 << shift) : 0;
		}
		mtx[mtx_id].unlock();
	}

	uint32_t query(const char* key, uint16_t key_len)
	{
		uint32_t ret = UINT32_MAX;
		for (int i = 0; i < d; ++i)
		{
			uint32_t idx = MurmurHash3_x86_32(key, key_len, hashseed[i]) % w[i];
			uint32_t a = A[i][idx >> cpw[i]];
			uint32_t shift = (idx & lo[i]) << cs[i];
			uint32_t val = (a >> shift) & mask[i];
			ret = (val < mask[i] && val < ret) ? val : ret;
		}
		return ret;
	}
};

class TowerSketchCU : public TowerSketch
{
public:
	TowerSketchCU() {}
	TowerSketchCU(uint32_t w_d) { init(w_d); }
	~TowerSketchCU() {}

	void insert(const char* key, uint16_t key_len)
	{
		int idx[d], mtx_id = 0;
		for (int i = 0; i < d; ++i)
		{
			idx[i] = MurmurHash3_x86_32(key, key_len, hashseed[i]) % w[i];
			mtx_id = (mtx_id << mtx_shift) | (idx[i] >> cpw[i] & mtx_mask);
		}
		uint32_t min_val = UINT32_MAX;
		mtx[mtx_id].lock();
		for (int i = 0; i < d; ++i)
		{
			uint32_t a = A[i][idx[i] >> cpw[i]];
			uint32_t shift = (idx[i] & lo[i]) << cs[i];
			uint32_t val = (a >> shift) & mask[i];
			min_val = (val < mask[i] && val < min_val) ? val : min_val;
		}
		for (int i = 0; i < d; ++i)
		{
			uint32_t& a = A[i][idx[i] >> cpw[i]];
			uint32_t shift = (idx[i] & lo[i]) << cs[i];
			uint32_t val = (a >> shift) & mask[i];
			a += (val < mask[i] && val == min_val) ? (1 << shift) : 0;
		}
		mtx[mtx_id].unlock();
	}
};

class TowerSketchACU : public TowerSketch
{
public:
	TowerSketchACU() {}
	TowerSketchACU(uint32_t w_d) { init(w_d); }
	~TowerSketchACU() {}

	void insert(const char* key, uint16_t key_len)
	{
		int idx[d], mtx_id = 0;
		for (int i = 0; i < d; ++i)
		{
			idx[i] = MurmurHash3_x86_32(key, key_len, hashseed[i]) % w[i];
			mtx_id = (mtx_id << mtx_shift) | (idx[i] >> cpw[i] & mtx_mask);
		}
		uint32_t min_val = UINT32_MAX;
		mtx[mtx_id].lock();
		for (int i = 0; i < d; ++i)
		{
			uint32_t &a = A[i][idx[i] >> cpw[i]];
			uint32_t shift = (idx[i] & lo[i]) << cs[i];
			uint32_t val = (a >> shift) & mask[i];
			min_val = (val < mask[i] && val < min_val) ? val : min_val;
            a += (val < mask[i] && val == min_val) ? (1 << shift) : 0;
		}
		mtx[mtx_id].unlock();
	}
};

#endif
