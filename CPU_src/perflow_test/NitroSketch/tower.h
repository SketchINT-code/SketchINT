#ifndef _TOWER_H
#define _TOWER_H

#include <iostream>
#include "BOBHash32.h"
#include "utils.h"
#include "../common/EMFSD.h"
#include "murmur3.h"
#include "NitroSketch.h"

using namespace std;

class Matrix
{
public:
    uint8_t counter_len, counter_per_int;
    uint32_t w, h, mask;
    uint32_t **counters;
    BOBHash32 *hash;

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
        // hash = new BOBHash32(random_seed);
        hash = new BOBHash32(rand() % 999 + 1);
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

    inline void insert(const char *key, uint16_t key_len, uint32_t row_id, uint32_t f = 1)
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

    inline void insert_with_hash_value(const char *key, uint16_t key_len, uint32_t row_id, uint32_t hash_value, uint32_t f = 1)
    {
        uint32_t index = hash_value % w;
        uint32_t buf = counters[row_id][index / counter_per_int];
        uint8_t shift = index % counter_per_int * counter_len;
        uint32_t val = (buf >> shift) & mask;
        if (f > counter_max - val)
            val = counter_max;
        else
            val += f;
        counters[row_id][index / counter_per_int] = buf & ~(mask << shift) | (val << shift);
    }

    inline uint32_t query(const char *key, uint16_t key_len, uint32_t row_id)
    {
        uint32_t index = hash->run(key, key_len) % w;
        uint32_t buf = counters[row_id][index / counter_per_int];
        uint8_t shift = index % counter_per_int * counter_len;
        uint32_t val = (buf >> shift) & mask;
        return val;
    }

    inline uint32_t query_with_hash_value(const char *key, uint16_t key_len, uint32_t row_id, uint32_t hash_value)
    {
        uint32_t index = hash_value % w;
        uint32_t buf = counters[row_id][index / counter_per_int];
        uint8_t shift = index % counter_per_int * counter_len;
        uint32_t val = (buf >> shift) & mask;
        return val;
    }

    uint32_t query_pos(int index, int row_id)
    {
        uint32_t buf = counters[row_id][index / counter_per_int];
        int shift = index % counter_per_int * counter_len;
        uint32_t val = (buf >> shift) & mask;
        return val;
    }

    double overflowRate()
    {
        uint32_t overflow_cnt = 0, used_cnt = 0;
        for (uint32_t row_id = 0; row_id < h; ++row_id)
            for (uint32_t index = 0; index < w; ++index)
            {
                uint32_t buf = counters[row_id][index / counter_per_int];
                uint8_t shift = index % counter_per_int * counter_len;
                uint32_t val = (buf >> shift) & mask;
                overflow_cnt += (val == counter_max);
                used_cnt += (val > 0);
            }
        return (double)overflow_cnt / used_cnt;
    }

    void used_count(int &used, int &total)
    {
        total = w * h;
        used = 0;
        for (uint32_t row_id = 0; row_id < h; ++row_id)
            for (uint32_t index = 0; index < w; ++index)
            {
                uint32_t buf = counters[row_id][index / counter_per_int];
                uint8_t shift = index % counter_per_int * counter_len;
                uint32_t val = (buf >> shift) & mask;
                used += (val > 0);
            }
    }
};

class TowerSketch
{
protected:
    uint8_t level;
    Matrix *mat;

    Nitrosketch* Nitro;

    int opt;
    int threshold;

    unordered_map<FIVE_TUPLE, int, Hash_Fun> elephant_items;

    double entropy;

    int max_flow;

public:
    int tot_packets;
    TowerSketch() {}
    TowerSketch(vector<uint32_t> &w, uint32_t h, vector<uint8_t> &counter_len, int _opt, int _threshold = 0, int memory = 0)
    {
        init(w, h, counter_len, _opt, _threshold);
        if (memory!=0){
            Nitro = new Nitrosketch(memory / 4 / 3, 1.0);
        } 
    }
    ~TowerSketch() { clear(); }

    void init(vector<uint32_t> &w, uint32_t h, vector<uint8_t> &counter_len, int _opt, int _threshold)
    {
        level = w.size();
        mat = new Matrix[level];
        for (uint8_t i = 0; i < level; ++i)
            mat[i].init(w[i], h, counter_len[i], 750 + i);

        opt = _opt;
        threshold = _threshold;

        elephant_items.clear();
        entropy = 0;
        tot_packets = 0;
        max_flow = 0;
    }

    void clear()
    {
        if (mat)
            delete[] mat;
    }

    virtual void insert(const char *key, uint16_t key_len, uint32_t row_id, uint32_t f = 1)
    {
        int pre_freq = 0;
        if (opt)
        {
            pre_freq = query(key, key_len, row_id);
        }

        tot_packets += f;

//        for (uint8_t i_level = 0; i_level < level; ++i_level)
//            mat[i_level].insert(key, key_len, row_id, f);

        Nitro->insert(key, key_len);        //************* new **************

        if (opt)
        {
            task_record(key, key_len, row_id, pre_freq);
        }
    }

    /*
     * Record some data for specific tasks, like heavy hitter, heavy change, and entropy estimation.
     * opt = 0: do nothing because we do not need to record anything.
     * opt = 1: recording heavy hitters in a unordered map for heavy hitter / heavy change tasks.
     * opt = 2: updating the flow entropy in a "real time" manner.
     */
    void task_record(const char *key, uint16_t key_len, uint32_t row_id, int pre_freq)
    {
        if (opt == 1)
        { // heavy_hitter or heavy_change
            int res_freq = query(key, key_len, row_id);
            if (res_freq >= threshold)
            {
                elephant_items[FIVE_TUPLE(key)] = res_freq;
            }
        }
        else if (opt == 2)
        {
            int cur_freq = query(key, key_len, row_id);

            // entropy
            if (pre_freq)
            {
                entropy -= pre_freq * log(pre_freq) / log(2);
            }
            entropy += cur_freq * log(cur_freq) / log(2);

            max_flow = max(max_flow, cur_freq);
        }
    }



    int query(const char *key, uint16_t key_len, uint32_t row_id)
    {
        uint32_t ret = UINT32_MAX;

        return Nitro->query(key, key_len);      //***************** new ****************

        for (uint8_t i_level = 0; i_level < level; ++i_level)
        {
            uint32_t tmp = mat[i_level].query(key, key_len, row_id);
            if (tmp < mat[i_level].counter_max)
                ret = min(ret, tmp);
        }
        return ret;
    }

    void query_heavyhitter(unordered_map<FIVE_TUPLE, int, Hash_Fun> *results)
    {
        results->clear();
        unordered_map<FIVE_TUPLE, int, Hash_Fun>::iterator it;

        for (it = elephant_items.begin(); it != elephant_items.end(); it++)
        {
            (*results)[it->first] = it->second;
        }
    }

    void query_heavyhitter_sort(vector<FLOW_ITEM> *results)
    {
        results->clear();

        unordered_map<FIVE_TUPLE, int, Hash_Fun>::iterator it;

        for (it = elephant_items.begin(); it != elephant_items.end(); it++)
        {
            results->push_back(FLOW_ITEM(it->first, it->second));
        }

        sort(results->begin(), results->end(), cmp_item);
    }

    double query_entropy()
    {
        return -entropy / (double)tot_packets + log(tot_packets) / log(2);
    }

    void query_distribution(double *results, int level)         //************** very slow ***************
    {
        EMFSD *em_fsd_algo = new EMFSD();

        int *counters = new int[mat[level].w];
        // printf("%d\n", mat[level].w);
        for (int j = 0; j < mat[level].w; j++)
        {
            counters[j] = mat[level].query_pos(j, 0);
        }

        em_fsd_algo->set_counters(mat[level].w, (uint32_t *)counters, mat[level].mask);
        // em_fsd_algo->set_counters(mat[level].w, (uint32_t *)counters, UINT32_MAX);

        for (int epoch = 0; epoch < 20; epoch++)
        {
            em_fsd_algo->next_epoch();
        }

#ifdef DEBUG_MODE
        printf("%d\n", em_fsd_algo->ns.size());
#endif
        for (int i = 0; i < em_fsd_algo->ns.size(); i++)
        {
            results[i] += em_fsd_algo->ns[i];
        }
        max_flow = max(max_flow, (int)em_fsd_algo->ns.size());

        delete[] counters;
        delete (em_fsd_algo);
    }

    int query_cardinality(int level_id)
    {
        /*
        int used, total;
        mat[level_id].used_count(used, total);
        return int(total * log(total / (double)(max(1, total - used))));*/

        int used=0, total=0;
        total = ns_d * Nitro->w;
        for (int i=0; i<ns_d; i++){
            for (int j=0; j<Nitro->w; j++){
                if (Nitro->counter[i][j]!=0)
                    used++;
                
            }
        }
        printf("%d %d\n", used, total);
        return int(total * log(total / (double)(max(1, total - used))));
    }

    void printOverflowRate()
    {
        for (uint8_t i_level = 0; i_level < level; ++i_level)
            printf("%.6lf\t", mat[i_level].overflowRate());
        printf("\n");
    }
};






#endif
