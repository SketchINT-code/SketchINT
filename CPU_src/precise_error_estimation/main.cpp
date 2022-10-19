#include "tower.h"
#include "cold_filter.h"
#include "cm.h"
#include "cu.h"

#include <thread>
#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <unordered_map>
using namespace std;

string data[2000000];
unordered_map<string, uint32_t> flow_size, real_size;
unordered_map<uint32_t, uint32_t> real_num, cm_num, cu_num, real_cm_num, real_cu_num;
unordered_map<uint32_t, int> cm_score, cu_score, cf_score, hc_score, my_cm_score, my_cu_score;
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

void test_error(pair<pair<uint32_t, uint32_t>, unordered_map<uint32_t, pair<double, uint32_t>> *> args)
{
    uint32_t w = args.first.first, threshold = args.first.second;
    unordered_map<uint32_t, pair<double, uint32_t>> *m = args.second;
    unordered_map<uint32_t, pair<uint32_t, uint32_t>> error_num;
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
        if (diff > threshold)
            error_num[it->second].second++;
        else
            error_num[it->second].first++;
    }
    delete tcm;
    for (unordered_map<uint32_t, pair<uint32_t, uint32_t>>::iterator it = error_num.begin(); it != error_num.end(); it++)
    {
        if (m->find(it->first) == m->end())
            (*m)[it->first] = make_pair(0, 0);
        (*m)[it->first].first += 1.0 * it->second.first / (it->second.first + it->second.second);
        (*m)[it->first].second += 1;
    }
}

void test_error1(pair<pair<uint32_t, uint32_t>, unordered_map<uint32_t, pair<double, uint32_t>> *> args)
{
    uint32_t w = args.first.first, threshold = args.first.second;
    unordered_map<uint32_t, pair<double, uint32_t>> *m = args.second;
    TowerSketch *tcm = new TowerSketch(w);
    for (int i = 0; i < 2000000; ++i)
        tcm->insert(data[i].c_str(), 13);

    for (uint32_t i = 1; i < 1000; ++i)
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
                    if (val > mask[j] - 1 - i)
                        ++cnt;
                }
            }
            p = p * cnt / tcm->w[j];
        }
        // fprintf(file_out, "%u %.5lf\n", i, 1 - p);
        if (m->find(i) == m->end())
            (*m)[i] = make_pair(0, 0);
        (*m)[i].first += 1 - p;
        (*m)[i].second += 1;
    }
    delete tcm;
}

void work1()
{
    unordered_map<uint32_t, pair<double, uint32_t>> m;
    for (int i = 0; i < 100000; ++i)
    {
        fprintf(stderr, "%d %d\n", 1, i);
        test_error(make_pair(make_pair(12800, 50), &m));
    }
    FILE *file_out = fopen("error_CM_zipf10_1", "w");
    for (unordered_map<uint32_t, pair<double, uint32_t>>::iterator it = m.begin(); it != m.end(); it++)
    {
        fprintf(file_out, "%u %lf\n", it->first, it->second.first / it->second.second);
    }
    fclose(file_out);
}

void work2()
{
    unordered_map<uint32_t, pair<double, uint32_t>> m;
    for (int i = 0; i < 100000; ++i)
    {
        fprintf(stderr, "%d %d\n", 2, i);
        test_error1(make_pair(make_pair(12800, 50), &m));
    }
    FILE *file_out = fopen("error_CM_zipf10_2", "w");
    for (unordered_map<uint32_t, pair<double, uint32_t>>::iterator it = m.begin(); it != m.end(); it++)
    {
        fprintf(file_out, "%u %lf\n", it->first, it->second.first / it->second.second);
    }
    fclose(file_out);
}

void work3()
{
    unordered_map<uint32_t, pair<double, uint32_t>> m;
    for (int i = 0; i < 100000; ++i)
    {
        fprintf(stderr, "%d %d\n", 3, i);
        test_error(make_pair(make_pair(12800, 100), &m));
    }
    FILE *file_out = fopen("error_CM_zipf10_3", "w");
    for (unordered_map<uint32_t, pair<double, uint32_t>>::iterator it = m.begin(); it != m.end(); it++)
    {
        fprintf(file_out, "%u %lf\n", it->first, it->second.first / it->second.second);
    }
    fclose(file_out);
}

void work4()
{
    unordered_map<uint32_t, pair<double, uint32_t>> m;
    for (int i = 0; i < 100000; ++i)
    {
        fprintf(stderr, "%d %d\n", 4, i);
        test_error1(make_pair(make_pair(12800, 100), &m));
    }
    FILE *file_out = fopen("error_CM_zipf10_4", "w");
    for (unordered_map<uint32_t, pair<double, uint32_t>>::iterator it = m.begin(); it != m.end(); it++)
    {
        fprintf(file_out, "%u %lf\n", it->first, it->second.first / it->second.second);
    }
    fclose(file_out);
}

int main()
{
    // read_campus();
    // read_caida18();
    // read_webpage();
    // read_zipf05();
     read_zipf10();

    ///*
    thread t1(work1);
    thread t2(work2);
    thread t3(work3);
    thread t4(work4);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    //*/

    return 0;
}