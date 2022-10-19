#ifndef _CM_H
#define _CM_H

#include <random>
#include <cstring>
#include <algorithm>

#include "params.h"
#include "murmur3.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))

class CM
{
public:
    uint32_t w;
    uint32_t *A[d];
    uint32_t hashseed[d];

    CM() {}
    CM(uint32_t _w) { init(_w); }
    virtual ~CM() { clear(); }

    void init(uint32_t _w)
    {
        std::random_device rd;
        w = _w;
        for (int i = 0; i < d; ++i)
        {
            A[i] = new uint32_t[w];
            memset(A[i], 0, w * sizeof(uint32_t));
            hashseed[i] = rd() % MAX_PRIME32;
        }
    }

    void clear()
    {
        for (int i = 0; i < d; ++i)
            delete[] A[i];
    }

    uint32_t insert(const char *key, uint16_t key_len)
    {
        uint32_t ret = UINT32_MAX;
        for (int i = 0; i < d; ++i)
        {
            uint32_t idx = MurmurHash3_x86_32(key, key_len, hashseed[i]) % w;
            A[i][idx] += 1;
            ret = MIN(ret, A[i][idx]);
        }
        return ret;
    }

    uint32_t query(const char *key, uint16_t key_len)
    {
        uint32_t ret = UINT32_MAX;
        for (int i = 0; i < d; ++i)
        {
            uint32_t idx = MurmurHash3_x86_32(key, key_len, hashseed[i]) % w;
            ret = MIN(ret, A[i][idx]);
        }
        return ret;
    }
};

#endif