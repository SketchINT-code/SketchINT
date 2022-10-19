#ifndef _CU_H
#define _CU_H

#include "cm.h"

class CU : public CM
{
public:
    CU() {}
    CU(uint32_t _w) { init(_w); }
    ~CU() {}

    uint32_t insert(const char *key, uint16_t key_len)
    {
        uint32_t idx[d];
        uint32_t ret = UINT32_MAX;
        for (int i = 0; i < d; ++i)
        {
            idx[i] = MurmurHash3_x86_32(key, key_len, hashseed[i]) % w;
            ret = MIN(ret, A[i][idx[i]]);
        }

        for (int i = 0; i < d; ++i)
        {
            if (ret == A[i][idx[i]])
                A[i][idx[i]] += 1;
        }
        return ret + 1;
    }
};

#endif