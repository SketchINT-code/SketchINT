#include "tower.h"
#include "cold_filter.h"
#include "cm.h"
#include "cu.h"

#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <unordered_map>
using namespace std;

string data[2300000];
FILE *file_out;
unordered_map<string, uint32_t> flow_size, real_size;
unordered_map<uint32_t, uint32_t> real_num, cm_num, cu_num, real_cm_num, real_cu_num;
unordered_map<uint32_t, int> cm_score, cu_score, cf_score, hc_score, my_cm_score, my_cu_score;
unordered_map<uint32_t, pair<uint32_t, uint32_t>> error_num;
vector<uint32_t> thresholds;

string char_array_to_string(char *s, int len)
{
    string result;
    for (int i = 0; i < len; ++i)
        result.push_back(s[i]);
    return result;
}

void read_campus()
{
    flow_size.clear();
    FILE *file_in = fopen("/share/datasets/campus/campus.dat", "rb");
    char s[20];
    for (uint32_t i = 0; i < 1000000; ++i)
    {
        int len1 = 13, len2 = 0;
        while (len1 != 0)
        {
            int tmp = fread(s + len2, 1, len1, file_in);
            len2 += tmp;
            len1 -= tmp;
        }
        data[i] = char_array_to_string(s, 13);
        if (flow_size.find(data[i]) == flow_size.end())
            flow_size[data[i]] = 1;
        else
            flow_size[data[i]] += 1;
    }
    fclose(file_in);
}

void read_caida18()
{
    flow_size.clear();
    FILE *file_in = fopen("/share/datasets/CAIDA2018/dataset/130000.dat", "rb");
    char s[20];
    char t[20];
    for (uint32_t i = 0; i < 2300000; ++i)
    {
        int len1 = 13, len2 = 0;
        while (len1 != 0)
        {
            int tmp = fread(s + len2, 1, len1, file_in);
            len2 += tmp;
            len1 -= tmp;
        }
        len1 = 8;
        len2 = 0;
        while (len1 != 0)
        {
            int tmp = fread(t + len2, 1, len1, file_in);
            len2 += tmp;
            len1 -= tmp;
        }
        data[i] = char_array_to_string(s, 13);
        if (flow_size.find(data[i]) == flow_size.end())
            flow_size[data[i]] = 1;
        else
            flow_size[data[i]] += 1;
    }
    fclose(file_in);
}

void read_zipf05()
{
    flow_size.clear();
    FILE *file_in = fopen("/root/SketchINT/data/zipf/trace0.5.dat", "rb");
    char s[20];
    char t[20];
    for (uint32_t i = 0; i < 2000000; ++i)
    {
        int len1 = 13, len2 = 0;
        while (len1 != 0)
        {
            int tmp = fread(s + len2, 1, len1, file_in);
            len2 += tmp;
            len1 -= tmp;
        }
        len1 = 8;
        len2 = 0;
        while (len1 != 0)
        {
            int tmp = fread(t + len2, 1, len1, file_in);
            len2 += tmp;
            len1 -= tmp;
        }
        data[i] = char_array_to_string(s, 13);
        if (flow_size.find(data[i]) == flow_size.end())
            flow_size[data[i]] = 1;
        else
            flow_size[data[i]] += 1;
    }
    fclose(file_in);
}

void read_zipf10()
{
    flow_size.clear();
    FILE *file_in = fopen("/root/SketchINT/data/zipf/trace1.0.dat", "rb");
    char s[20];
    char t[20];
    for (uint32_t i = 0; i < 2000000; ++i)
    {
        int len1 = 13, len2 = 0;
        while (len1 != 0)
        {
            int tmp = fread(s + len2, 1, len1, file_in);
            len2 += tmp;
            len1 -= tmp;
        }
        len1 = 8;
        len2 = 0;
        while (len1 != 0)
        {
            int tmp = fread(t + len2, 1, len1, file_in);
            len2 += tmp;
            len1 -= tmp;
        }
        data[i] = char_array_to_string(s, 13);
        if (flow_size.find(data[i]) == flow_size.end())
            flow_size[data[i]] = 1;
        else
            flow_size[data[i]] += 1;
    }
    fclose(file_in);
}

void read_webpage()
{
    flow_size.clear();
    FILE *file_in = fopen("/share/datasets/webpage/webdocs_form00.dat", "rb");
    char s[20];
    for (uint32_t i = 0; i < 2000000; ++i)
    {
        memset(s, 0, sizeof(s));
        int len1 = 8, len2 = 0;
        while (len1 != 0)
        {
            int tmp = fread(s + len2, 1, len1, file_in);
            len2 += tmp;
            len1 -= tmp;
        }
        data[i] = char_array_to_string(s, 13);
        if (flow_size.find(data[i]) == flow_size.end())
            flow_size[data[i]] = 1;
        else
            flow_size[data[i]] += 1;
    }
    fclose(file_in);
}

void test_init()
{
    real_num.clear();
    cm_num.clear();
    cu_num.clear();
    real_cm_num.clear();
    real_cu_num.clear();
    thresholds.clear();
    cm_score.clear();
    cu_score.clear();
    cf_score.clear();
    hc_score.clear();
    my_cm_score.clear();
    my_cu_score.clear();
    for (int threshold = 500; threshold <= 500; threshold += 50)
    {
        thresholds.push_back(threshold);
        real_num[threshold] = 0;
        cm_num[threshold] = 0;
        cu_num[threshold] = 0;
        real_cm_num[threshold] = 0;
        real_cu_num[threshold] = 0;
        cm_score[threshold] = 100*REP_TIME;
        cu_score[threshold] = 100*REP_TIME;
        cf_score[threshold] = 100*REP_TIME;
        hc_score[threshold] = 100*REP_TIME;
        my_cm_score[threshold] = 100*REP_TIME;
        my_cu_score[threshold] = 100*REP_TIME;
    }
}

void test_score(uint32_t w)
{
    printf("%u: \n", w);
    int mem = w * 4 * 4;
    test_init();

    printf("tower cm begins\n");
    for (int R = 0; R < REP_TIME; ++R)
    {
        real_size.clear();
        TowerSketch *tcm = new TowerSketch(w);
        for (int i = 0; i < 2300000; ++i)
        {
            uint32_t size_sketch, size_real;
            size_sketch = tcm->insert(data[i].c_str(), 13);
            if (real_size.find(data[i]) == real_size.end())
                real_size[data[i]] = 0;
            real_size[data[i]] += 1;
            size_real = flow_size[data[i]];
            for (int j = 0; j < thresholds.size(); ++j)
            {
                /*
                if (size_real >= thresholds[j])
                    real_num[thresholds[j]] += 1;
                if (size_sketch >= thresholds[j])
                    cm_num[thresholds[j]] += 1;
                */
                if (size_real >= thresholds[j] && size_sketch >= thresholds[j])
                    real_cm_num[thresholds[j]] += 1;
                if (size_sketch >= thresholds[j] && real_size[data[i]] < thresholds[j] && size_real >= thresholds[j])
                    cm_score[thresholds[j]] += 1;
                if (size_sketch >= thresholds[j] && size_real < thresholds[j])
                    cm_score[thresholds[j]] -= 1;
            }
        }
        delete tcm;
    }

    printf("tower cu begins\n");
    for (int R = 0; R < REP_TIME; ++R)
    {
        real_size.clear();
        TowerSketchCU *tcu = new TowerSketchCU(w);
        for (int i = 0; i < 2300000; ++i)
        {
            uint32_t size_sketch, size_real;
            size_sketch = tcu->insert(data[i].c_str(), 13);
            if (real_size.find(data[i]) == real_size.end())
                real_size[data[i]] = 0;
            real_size[data[i]] += 1;
            size_real = flow_size[data[i]];
            for (int j = 0; j < thresholds.size(); ++j)
            {
                /*
                if (size_sketch >= thresholds[j])
                    cu_num[thresholds[j]] += 1;
                if (size_real >= thresholds[j] && size_sketch >= thresholds[j])
                    real_cu_num[thresholds[j]] += 1;
                */
                if (size_sketch >= thresholds[j] && real_size[data[i]] < thresholds[j] && size_real >= thresholds[j])
                    cu_score[thresholds[j]] += 1;
                if (size_sketch >= thresholds[j] && size_real < thresholds[j])
                    cu_score[thresholds[j]] -= 1;
            }
        }
        delete tcu;
    }

    printf("cf begin\n");
    for (int R = 0; R < REP_TIME; ++R)
    {
        real_size.clear();
        StreamClassifier<1, 1, 485, 65, 13> *cf = new StreamClassifier<1, 1, 485, 65, 13>(mem);
        for (int i = 0; i < 2300000; ++i)
        {
            uint32_t size_sketch, size_real;
            cf->insert(data[i]);
            cf->refresh();
            size_sketch = cf->query(data[i]);
            if (real_size.find(data[i]) == real_size.end())
                real_size[data[i]] = 0;
            real_size[data[i]] += 1;
            // printf("%u,%u\n", size_sketch, real_size[data[i]]);
            size_real = flow_size[data[i]];
            for (int j = 0; j < thresholds.size(); ++j)
            {
                if (size_sketch >= thresholds[j] && real_size[data[i]] < thresholds[j] && size_real >= thresholds[j])
                    cf_score[thresholds[j]] += 1;
                if (size_sketch >= thresholds[j] && size_real < thresholds[j])
                    cf_score[thresholds[j]] -= 1;
            }
        }
        delete cf;
    }

    printf("hc begin\n");
    for (int R = 0; R < REP_TIME; ++R)
    {
        real_size.clear();
        TowerSketchHalfCU *hc = new TowerSketchHalfCU(w);
        for (int i = 0; i < 2300000; ++i)
        {
            uint32_t size_sketch, size_real;
            size_sketch = hc->insert(data[i].c_str(), 13);
            if (real_size.find(data[i]) == real_size.end())
                real_size[data[i]] = 0;
            real_size[data[i]] += 1;
            // printf("%u,%u\n", size_sketch, real_size[data[i]]);
            size_real = flow_size[data[i]];
            for (int j = 0; j < thresholds.size(); ++j)
            {
                if (size_sketch >= thresholds[j] && real_size[data[i]] < thresholds[j] && size_real >= thresholds[j])
                    hc_score[thresholds[j]] += 1;
                if (size_sketch >= thresholds[j] && size_real < thresholds[j])
                    hc_score[thresholds[j]] -= 1;
            }
        }
        delete hc;
    }

    printf("cm begin\n");
    for (int R = 0; R < REP_TIME; ++R)
    {
        real_size.clear();
        CM *cm = new CM(w);
        for (int i = 0; i < 2300000; ++i)
        {
            uint32_t size_sketch, size_real;
            size_sketch = cm->insert(data[i].c_str(), 13);
            if (real_size.find(data[i]) == real_size.end())
                real_size[data[i]] = 0;
            real_size[data[i]] += 1;
            // printf("%u,%u\n", size_sketch, real_size[data[i]]);
            size_real = flow_size[data[i]];
            for (int j = 0; j < thresholds.size(); ++j)
            {
                if (size_sketch >= thresholds[j] && real_size[data[i]] < thresholds[j] && size_real >= thresholds[j])
                    my_cm_score[thresholds[j]] += 1;
                if (size_sketch >= thresholds[j] && size_real < thresholds[j])
                    my_cm_score[thresholds[j]] -= 1;
            }
        }
        delete cm;
    }

    printf("cu begin\n");
    for (int R = 0; R < REP_TIME; ++R)
    {
        real_size.clear();
        CU *cu = new CU(w);
        for (int i = 0; i < 2300000; ++i)
        {
            uint32_t size_sketch, size_real;
            size_sketch = cu->insert(data[i].c_str(), 13);
            if (real_size.find(data[i]) == real_size.end())
                real_size[data[i]] = 0;
            real_size[data[i]] += 1;
            // printf("%u,%u\n", size_sketch, real_size[data[i]]);
            size_real = flow_size[data[i]];
            for (int j = 0; j < thresholds.size(); ++j)
            {
                if (size_sketch >= thresholds[j] && real_size[data[i]] < thresholds[j] && size_real >= thresholds[j])
                    my_cu_score[thresholds[j]] += 1;
                if (size_sketch >= thresholds[j] && size_real < thresholds[j])
                    my_cu_score[thresholds[j]] -= 1;
            }
        }
        delete cu;
    }

    printf("caculate begins\n");
    for (int i = 0; i < thresholds.size(); ++i)
    {
        printf("%d,%u,%u,%d,%d,%d,%d,%d,%d\n", mem, w, thresholds[i], cm_score[thresholds[i]], cu_score[thresholds[i]], cf_score[thresholds[i]], hc_score[thresholds[i]], my_cm_score[thresholds[i]], my_cu_score[thresholds[i]]);
        fprintf(file_out, "%d,%u,%u,%lf,%lf,%lf,%lf,%lf,%lf\n", mem, w, thresholds[i], 1.0 * cm_score[thresholds[i]] / REP_TIME, 1.0 * cu_score[thresholds[i]] / REP_TIME, 1.0 * cf_score[thresholds[i]] / REP_TIME, 1.0 * hc_score[thresholds[i]] / REP_TIME, 1.0 * my_cm_score[thresholds[i]] / REP_TIME, 1.0 * my_cu_score[thresholds[i]] / REP_TIME);

        /*
        printf("cu,%u,%u,%u,%u,%u,%.5lf,%.5lf,%d\n", w, thresholds[i], real_num[thresholds[i]], cu_num[thresholds[i]], real_cu_num[thresholds[i]], 1.0 * real_cu_num[thresholds[i]] / real_num[thresholds[i]], 1.0 * real_cu_num[thresholds[i]] / cu_num[thresholds[i]], cu_score[thresholds[i]]);
        fprintf(file_out, "cu,%u,%u,%u,%u,%u,%.5lf,%.5lf,%d\n", w, thresholds[i], real_num[thresholds[i]], cu_num[thresholds[i]], real_cu_num[thresholds[i]], 1.0 * real_cu_num[thresholds[i]] / real_num[thresholds[i]], 1.0 * real_cu_num[thresholds[i]] / cu_num[thresholds[i]], cu_score[thresholds[i]]);
        */
    }
}

void test_error(uint32_t w, uint32_t threshold)
{

    TowerSketch *tcm = new TowerSketch(w);
    for (int i = 0; i < 2000000; ++i)
        tcm->insert(data[i].c_str(), 13);
    error_num.clear();
    for (unordered_map<string, uint32_t>::iterator it = flow_size.begin(); it != flow_size.end(); it++)
    {
        int size_sketch = tcm->query(it->first.c_str(), 13);
        int diff = abs(size_sketch - int(it->second));
        if (error_num.find(it->second) == error_num.end())
            error_num[it->second] = make_pair(0, 0);
        if (diff >= threshold)
            error_num[it->second].second++;
        else
            error_num[it->second].first++;
    }
    delete tcm;
    for (unordered_map<uint32_t, pair<uint32_t, uint32_t>>::iterator it = error_num.begin(); it != error_num.end(); it++)
    {
        fprintf(file_out, "%u %u %u\n", it->first, it->second.first, it->second.second);
    }
}

void test_error1(uint32_t w, uint32_t threshold)
{
    TowerSketch *tcm = new TowerSketch(w);
    for (int i = 0; i < 2000000; ++i)
        tcm->insert(data[i].c_str(), 13);

    for (uint32_t i = 0; i < 4000; ++i)
    {
        double p = 1;
        for (int j = 0; j < 5; ++j)
        {
            if (mask[j] <= i)
                continue;
            uint32_t cnt = 0;
            for (int k = 0; k < tcm->w[j]; ++k)
            {
                uint32_t &a = tcm->A[j][k >> cpw[j]];
                uint32_t shift = (k & lo[j]) << cs[j];
                uint32_t val = (a >> shift) & mask[j];
                if (mask[j] >= i + threshold)
                {
                    if (val > threshold)
                        ++cnt;
                }
                else
                {
                    if (val > mask[j] - i)
                        ++cnt;
                }
            }
            p = p * cnt / tcm->w[j];
        }
        fprintf(file_out, "%u %.5lf\n", i, p);
    }
}

int main()
{
    // read_campus();
    read_caida18();
    // read_webpage();
    //read_zipf05();
    //read_zipf10();

    ///*
    file_out = fopen("result.csv", "w");
    fprintf(file_out, "memory,w,threshold,tower_cm,tower_cu,cold filter,tower_halfcu,cm,cu\n");
    for (uint32_t w = 625; w <= 625*6; w += 625)
    {
        test_score(w);
    }
    fclose(file_out);

    printf(">=0 : %lu\n", flow_size.size());
    for (int i = 0; i < thresholds.size(); ++i)
    {
        int cnt = 0;
        for (unordered_map<string, uint32_t>::iterator it = flow_size.begin(); it != flow_size.end(); ++it)
            if (it->second >= thresholds[i])
                ++cnt;
        printf(">=%u : %d\n", thresholds[i], cnt);
    }
    //*/

    /*
    file_out = fopen("tmp1", "w");
    for (int i = 0; i < 10000; ++i)
        test_error(4000, 50);
    fclose(file_out);
    file_out = fopen("tmp2", "w");
    for (int i = 0; i < 10000; ++i)
        test_error1(4000, 50);
    fclose(file_out);
    file_out = fopen("tmp3", "w");
    for (int i = 0; i < 10000; ++i)
        test_error(4000, 100);
    fclose(file_out);
    file_out = fopen("tmp4", "w");
    for (int i = 0; i < 10000; ++i)
        test_error1(4000, 100);
    fclose(file_out);
    */

    return 0;
}