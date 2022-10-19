#ifndef _TOWER_H
#define _TOWER_H

#include <cstring>
#include "BOBHash32.h"
#include <iostream>

int output_rid = -1;

class Matrix
{
private:
    uint8_t counter_len, counter_per_int;
    uint32_t w, h, mask;
    uint32_t **counters;
    BOBHash32 *hash;

public:
    uint32_t counter_max;

    Matrix() {}
    Matrix(uint32_t in_w, uint32_t in_h, uint8_t in_counter_len, uint32_t random_seed)
    {
        init(in_w, in_h, in_counter_len, random_seed);
    }
    ~Matrix() { clear(); }

    void init(uint32_t in_w, uint32_t in_h, uint8_t in_counter_len, uint32_t random_seed)
    {
        w = in_w, h = in_h, counter_len = in_counter_len;
        counter_per_int = 32 / counter_len;
        if (counter_len == 32)
            mask = counter_max = 0xffffffff;
        else
            mask = counter_max = (1u << counter_len) - 1;
        uint32_t actual_w = (w + counter_per_int - 1) / counter_per_int;
        counters = new uint32_t *[h];
        *counters = new uint32_t[h * actual_w];
        for (uint32_t i = 1; i < h; ++i)
            counters[i] = *counters + i * actual_w;
        memset(*counters, 0, sizeof(uint32_t) * h * actual_w);
        hash = new BOBHash32(random_seed);
    }

    void clear()
    {
        if (*counters)
            delete[] * counters;
        if (counters)
            delete[] counters;
        if (hash)
            delete hash;
    }

    void insert(const char *key, uint16_t key_len, uint32_t row_id, uint32_t f = 1)
    {
        uint32_t index = hash->run(key, key_len) % w;
        uint32_t buf = counters[row_id][index / counter_per_int];
        uint8_t shift = index % counter_per_int * counter_len;
        uint32_t val = (buf >> shift) & mask;
        if (f > counter_max - val)
            val = counter_max;
        else
            val += f;
        counters[row_id][index / counter_per_int] = buf & ~(mask << shift) | (val << shift);
    }

    void modify(const char *key, uint16_t key_len, uint32_t row_id, uint32_t f = 0)
    {
        uint32_t index = hash->run(key, key_len) % w;
        uint32_t buf = counters[row_id][index / counter_per_int];
        uint8_t shift = index % counter_per_int * counter_len;
        uint32_t val = f;
        counters[row_id][index / counter_per_int] = buf & ~(mask << shift) | (val << shift);
    }

    uint32_t query(const char *key, uint16_t key_len, uint32_t row_id)
    {
        uint32_t index = hash->run(key, key_len) % w;
        uint32_t buf = counters[row_id][index / counter_per_int];
        uint8_t shift = index % counter_per_int * counter_len;
        uint32_t val = (buf >> shift) & mask;
        return val;
    }

    void mergeAdd(const Matrix &B)
    {
        for (uint32_t row_id = 0; row_id < B.h; ++row_id)
            for (uint32_t index = 0; index < B.w; ++index)
            {
                uint32_t shift = index % counter_per_int * counter_len;
                uint32_t buf1 = counters[row_id][index / counter_per_int];
                uint32_t val1 = (buf1 >> shift) & counter_max;
                uint32_t buf2 = B.counters[row_id][index / counter_per_int];
                uint32_t val2 = (buf2 >> shift) & counter_max;
                if (val2 > counter_max - val1)
                    val1 = counter_max;
                else
                    val1 += val2;
                counters[row_id][index / counter_per_int] = buf1 & ~(counter_max << shift) | (val1 << shift);
            }
    }

    void mergeMax(const Matrix &B)
    {
        for (uint32_t row_id = 0; row_id < B.h; ++row_id)
            for (uint32_t index = 0; index < B.w; ++index)
            {
                uint32_t shift = index % counter_per_int * counter_len;
                uint32_t buf1 = counters[row_id][index / counter_per_int];
                uint32_t val1 = (buf1 >> shift) & counter_max;
                uint32_t buf2 = B.counters[row_id][index / counter_per_int];
                uint32_t val2 = (buf2 >> shift) & counter_max;
                val1 = max(val1, val2);
                counters[row_id][index / counter_per_int] = buf1 & ~(counter_max << shift) | (val1 << shift);
            }
    }
};

class TowerSketch
{
protected:
    uint8_t level;
    Matrix *mat;

public:
    TowerSketch() : mat(NULL) {}
    TowerSketch(vector<uint32_t> &w, uint32_t h, vector<uint8_t> &counter_len, uint32_t seed_num)
    {
        init(w, h, counter_len, seed_num);
    }
    ~TowerSketch() { clear(); }

    void init(vector<uint32_t> &w, uint32_t h, vector<uint8_t> &counter_len, uint32_t seed_num)
    {
        level = w.size();
        mat = new Matrix[level];
        for (uint8_t i = 0; i < level; ++i)
            mat[i].init(w[i], h, counter_len[i], seed_num + i);
    }

    void clear()
    {
        if (mat)
            delete[] mat;
    }

    virtual void insert(const char *key, uint16_t key_len, uint32_t row_id, uint32_t f = 1)
    {
        for (uint8_t i_level = 0; i_level < level; ++i_level)
            mat[i_level].insert(key, key_len, row_id, f);
    }

    void modify(const char *key, uint16_t key_len, uint32_t row_id, uint32_t f = 0)
    {
        for (uint8_t i_level = 0; i_level < level; ++i_level)
            mat[i_level].modify(key, key_len, row_id, f);
    }

    uint32_t query(const char *key, uint16_t key_len, uint32_t row_id)
    {
        uint32_t ret = UINT32_MAX;
        for (uint8_t i_level = 0; i_level < level; ++i_level)
        {
            uint32_t tmp = mat[i_level].query(key, key_len, row_id);
            if (tmp < mat[i_level].counter_max)
                ret = min(ret, tmp);
        }
        return ret;
    }

    vector<uint32_t> queryAll(const char *key, uint16_t key_len, uint32_t row_id)
    {
        vector<uint32_t> ret;
        for (uint8_t i_level = 0; i_level < level; ++i_level)
        {
            uint32_t tmp = mat[i_level].query(key, key_len, row_id);
            if (tmp < mat[i_level].counter_max)
                ret.push_back(tmp);
            else
                ret.push_back(UINT32_MAX);
        }
        return ret;
    }

    void mergeAdd(const TowerSketch &B)
    {
        for (uint32_t i = 0; i < level; ++i)
            mat[i].mergeAdd(B.mat[i]);
    }

    void mergeMax(const TowerSketch &B)
    {
        for (uint32_t i = 0; i < level; ++i)
            mat[i].mergeMax(B.mat[i]);
    }
};

class TowerSketchCU : public TowerSketch
{
public:
    TowerSketchCU() {}
    TowerSketchCU(vector<uint32_t> &w, uint32_t h, vector<uint8_t> &cs, uint32_t seed_num) : TowerSketch(w, h, cs, seed_num) {}

    void insert(const char *key, uint16_t key_len, uint32_t row_id, uint32_t f = 1)
    {
        uint32_t min_val = UINT32_MAX;
        vector<uint32_t> ret_val(level);
        for (uint8_t i_level = 0; i_level < level; ++i_level)
        {
            ret_val[i_level] = mat[i_level].query(key, key_len, row_id);
            if (ret_val[i_level] < mat[i_level].counter_max)
                min_val = min(min_val, ret_val[i_level]);
        }
        min_val += f;
        for (uint8_t i_level = 0; i_level < level; ++i_level)
            if (ret_val[i_level] < min_val)
                mat[i_level].insert(key, key_len, row_id, min_val - ret_val[i_level]);
    }
};

class TowerSketchACU : public TowerSketch
{
public:
    TowerSketchACU() {}
    TowerSketchACU(vector<uint32_t> &w, uint32_t h, vector<uint8_t> &cs, uint32_t seed_num) : TowerSketch(w, h, cs, seed_num) {}

    virtual void insert(const char *key, uint16_t key_len, uint32_t row_id, uint32_t f = 1)
    {
        uint32_t min_val = UINT32_MAX;
        vector<uint32_t> ret_val(level);
        for (uint8_t i_level = 0; i_level < level; ++i_level)
        {
            ret_val[i_level] = mat[i_level].query(key, key_len, row_id);
            if (ret_val[i_level] < mat[i_level].counter_max)
                min_val = min(min_val, ret_val[i_level]);
            if (min_val != UINT32_MAX && ret_val[i_level] < min_val + f)
                mat[i_level].insert(key, key_len, row_id, min_val + f - ret_val[i_level]);
        }
    }
};

#endif
