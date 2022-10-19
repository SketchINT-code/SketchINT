#ifndef _HH_TEST_H
#define _HH_TEST_H

#include "../sketch/host_sketches.h"
#include <unordered_map>
#include <unordered_set>

double calcHHAcc(HostSketches *sketch, unordered_map<string, int> *realFreq, int sw_num, int host_num)
{
    uint32_t tp = 0, fn = 0, fp = 0;
    for (int row_id = 0; row_id < sw_num; ++row_id)
    {
        unordered_set<string> real_hh;
        for (auto pr : realFreq[row_id])
            if (pr.second >= hh_thres)
                real_hh.insert(pr.first);

        unordered_set<string> est_hh;
        for (int i = 0; i < host_num; ++i)
        {
            auto ret_hh = sketch[i].getHH(row_id);
            est_hh.insert(ret_hh.begin(), ret_hh.end());
        }

        uint32_t tpi = 0, fni = 0, fpi = 0;
        for (auto key : est_hh)
            if (real_hh.find(key) == real_hh.end())
                ++fpi;
        for (auto key : real_hh)
            if (est_hh.find(key) == est_hh.end())
                ++fni;
        tpi = est_hh.size() - fpi;
        tp += tpi, fn += fni, fp += fpi;
    }

    double precision = (double)tp / (tp + fp);
    double recall = (double)tp / (tp + fn);
    //printf("tp: %u fp: %u fn: %u\n", tp, fp, fn);
    double F1_score = 2 * precision * recall / (precision + recall);

    return F1_score;
}

#endif
