#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include "tower.h"
#include "cm.h"
#include "../../perflow_test/elastic/elastic_sketch/ElasticSketch.h"

using namespace std;
using namespace std::chrono;

const int WARM_UP_TIME = 10;
const int RUN_TIME = 10;

vector<string> packets;
unordered_map<string, int> freq;

void fileReader(const char *filename, int MAX_ITEM = INT32_MAX)
{
    ifstream inFile(filename, ios::binary);
    ios::sync_with_stdio(false);

    char key[13], timestamp[8];
    for (int i = 0; i < MAX_ITEM; ++i)
    {
        inFile.read(key, 13);
        if (inFile.gcount() < 13)
            break;
        inFile.read(timestamp, 8);
        string str = string(key, 4);
        packets.push_back(str);
        freq[str]++;
    }
    inFile.close();

    cout << freq.size() << "flows, " << packets.size() << " packets read" << endl;
}

void test_CM(int mem_in_byte, double &insert_time, double &query_time)
{
    int w = mem_in_byte / 4 / 5;
    CMSketch cm(w);

    steady_clock::time_point t1 = steady_clock::now();
    for (auto key : packets)
        cm.insert(key.c_str(), key.length());
    steady_clock::time_point t2 = steady_clock::now();
    duration<double> time_span1 = duration_cast<duration<double>>(t2 - t1);
    insert_time = time_span1.count();

    steady_clock::time_point t3 = steady_clock::now();
    for (auto pr : freq)
        int est_val = cm.query(pr.first.c_str(), pr.first.length());
    steady_clock::time_point t4 = steady_clock::now();
    duration<double> time_span2 = duration_cast<duration<double>>(t4 - t3);
    query_time = time_span2.count();
}

void test_CU(int mem_in_byte, double &insert_time, double &query_time)
{
    int w = mem_in_byte / 4 / 5;
    CUSketch cu(w);

    steady_clock::time_point t1 = steady_clock::now();
    for (auto key : packets)
        cu.insert(key.c_str(), key.length());
    steady_clock::time_point t2 = steady_clock::now();
    duration<double> time_span1 = duration_cast<duration<double>>(t2 - t1);
    insert_time = time_span1.count();

    steady_clock::time_point t3 = steady_clock::now();
    for (auto pr : freq)
        int est_val = cu.query(pr.first.c_str(), pr.first.length());
    steady_clock::time_point t4 = steady_clock::now();
    duration<double> time_span2 = duration_cast<duration<double>>(t4 - t3);
    query_time = time_span2.count();
}

void test_tower(int mem_in_byte, double &insert_time, double &query_time)
{
    int w = mem_in_byte / 4 / 5;
    TowerSketch tower(w);

    steady_clock::time_point t1 = steady_clock::now();
    for (auto key : packets)
        tower.insert(key.c_str(), key.length());
    steady_clock::time_point t2 = steady_clock::now();
    duration<double> time_span1 = duration_cast<duration<double>>(t2 - t1);
    insert_time = time_span1.count();

    steady_clock::time_point t3 = steady_clock::now();
    for (auto pr : freq)
        int est_val = tower.query(pr.first.c_str(), pr.first.length());
    steady_clock::time_point t4 = steady_clock::now();
    duration<double> time_span2 = duration_cast<duration<double>>(t4 - t3);
    query_time = time_span2.count();
}

void test_towerCU(int mem_in_byte, double &insert_time, double &query_time)
{
    int w = mem_in_byte / 4 / 5;
    TowerSketchCU tower_cu(w);

    steady_clock::time_point t1 = steady_clock::now();
    for (auto key : packets)
        tower_cu.insert(key.c_str(), key.length());
    steady_clock::time_point t2 = steady_clock::now();
    duration<double> time_span1 = duration_cast<duration<double>>(t2 - t1);
    insert_time = time_span1.count();

    steady_clock::time_point t3 = steady_clock::now();
    for (auto pr : freq)
        int est_val = tower_cu.query(pr.first.c_str(), pr.first.length());
    steady_clock::time_point t4 = steady_clock::now();
    duration<double> time_span2 = duration_cast<duration<double>>(t4 - t3);
    query_time = time_span2.count();
}

void test_towerACU(int mem_in_byte, double &insert_time, double &query_time)
{
    int w = mem_in_byte / 4 / 5;
    TowerSketchACU tower_acu(w);

    steady_clock::time_point t1 = steady_clock::now();
    for (auto key : packets)
        tower_acu.insert(key.c_str(), key.length());
    steady_clock::time_point t2 = steady_clock::now();
    duration<double> time_span1 = duration_cast<duration<double>>(t2 - t1);
    insert_time = time_span1.count();

    steady_clock::time_point t3 = steady_clock::now();
    for (auto pr : freq)
        int est_val = tower_acu.query(pr.first.c_str(), pr.first.length());
    steady_clock::time_point t4 = steady_clock::now();
    duration<double> time_span2 = duration_cast<duration<double>>(t4 - t3);
    query_time = time_span2.count();
}

void test_Elastic(int mem_in_byte, double &insert_time, double &query_time)
{
    ElasticSketch<(2 << 20) / 128, (2 << 20)> ela;

    steady_clock::time_point t1 = steady_clock::now();
    for (auto key : packets)
        ela.insert((uint8_t *)key.c_str());
    steady_clock::time_point t2 = steady_clock::now();
    duration<double> time_span1 = duration_cast<duration<double>>(t2 - t1);
    insert_time = time_span1.count();

    steady_clock::time_point t3 = steady_clock::now();
    for (auto pr : freq)
        int est_val = ela.query((uint8_t *)pr.first.c_str());
    steady_clock::time_point t4 = steady_clock::now();
    duration<double> time_span2 = duration_cast<duration<double>>(t4 - t3);
    query_time = time_span2.count();
}

void test(int mem_in_byte)
{
    FILE *file_out = fopen("result.txt", "w");

    double ave_insert_time = 0, ave_query_time = 0;
    for (int i = 0; i < WARM_UP_TIME + RUN_TIME; ++i)
    {
        double insert_time, query_time;
        test_CM(mem_in_byte, insert_time, query_time);
        if (i >= WARM_UP_TIME)
            ave_insert_time += insert_time, ave_query_time += query_time;
    }
    ave_insert_time /= RUN_TIME, ave_query_time /= RUN_TIME;
    double insert_throughput = packets.size() / ave_insert_time / 1e6;
    double query_throughput = freq.size() / ave_query_time / 1e6;
    cout << "CM\t" << insert_throughput << "Mpps\t" << query_throughput << "Mpps" << endl;
    fprintf(file_out, "CM\n%lf %lf\n", insert_throughput, query_throughput);

    ave_insert_time = 0, ave_query_time = 0;
    for (int i = 0; i < WARM_UP_TIME + RUN_TIME; ++i)
    {
        double insert_time, query_time;
        test_CU(mem_in_byte, insert_time, query_time);
        if (i >= WARM_UP_TIME)
            ave_insert_time += insert_time, ave_query_time += query_time;
    }
    ave_insert_time /= RUN_TIME, ave_query_time /= RUN_TIME;
    insert_throughput = packets.size() / ave_insert_time / 1e6;
    query_throughput = freq.size() / ave_query_time / 1e6;
    cout << "CU\t" << insert_throughput << "Mpps\t" << query_throughput << "Mpps" << endl;
    fprintf(file_out, "CU\n%lf %lf\n", insert_throughput, query_throughput);

    ave_insert_time = 0, ave_query_time = 0;
    for (int i = 0; i < WARM_UP_TIME + RUN_TIME; ++i)
    {
        double insert_time, query_time;
        test_tower(mem_in_byte, insert_time, query_time);
        if (i >= WARM_UP_TIME)
            ave_insert_time += insert_time, ave_query_time += query_time;
    }
    ave_insert_time /= RUN_TIME, ave_query_time /= RUN_TIME;
    insert_throughput = packets.size() / ave_insert_time / 1e6;
    query_throughput = freq.size() / ave_query_time / 1e6;
    cout << "Tower\t" << insert_throughput << "Mpps\t" << query_throughput << "Mpps" << endl;
    fprintf(file_out, "Tower\n%lf %lf\n", insert_throughput, query_throughput);

    ave_insert_time = 0, ave_query_time = 0;
    for (int i = 0; i < WARM_UP_TIME + RUN_TIME; ++i)
    {
        double insert_time, query_time;
        test_towerCU(mem_in_byte, insert_time, query_time);
        if (i >= WARM_UP_TIME)
            ave_insert_time += insert_time, ave_query_time += query_time;
    }
    ave_insert_time /= RUN_TIME, ave_query_time /= RUN_TIME;
    insert_throughput = packets.size() / ave_insert_time / 1e6;
    query_throughput = freq.size() / ave_query_time / 1e6;
    cout << "Tower+CU\t" << insert_throughput << "Mpps\t" << query_throughput << "Mpps" << endl;
    fprintf(file_out, "TowerCU\n%lf %lf\n", insert_throughput, query_throughput);

    ave_insert_time = 0, ave_query_time = 0;
    for (int i = 0; i < WARM_UP_TIME + RUN_TIME; ++i)
    {
        double insert_time, query_time;
        test_towerACU(mem_in_byte, insert_time, query_time);
        if (i >= WARM_UP_TIME)
            ave_insert_time += insert_time, ave_query_time += query_time;
    }
    ave_insert_time /= RUN_TIME, ave_query_time /= RUN_TIME;
    insert_throughput = packets.size() / ave_insert_time / 1e6;
    query_throughput = freq.size() / ave_query_time / 1e6;
    cout << "Tower+ACU\t" << insert_throughput << "Mpps\t" << query_throughput << "Mpps" << endl;
    fprintf(file_out, "TowerACU\n%lf %lf\n", insert_throughput, query_throughput);

    ave_insert_time = 0, ave_query_time = 0;
    for (int i = 0; i < WARM_UP_TIME + RUN_TIME; ++i)
    {
        double insert_time, query_time;
        test_Elastic(mem_in_byte, insert_time, query_time);
        if (i >= WARM_UP_TIME)
            ave_insert_time += insert_time, ave_query_time += query_time;
    }
    ave_insert_time /= RUN_TIME, ave_query_time /= RUN_TIME;
    insert_throughput = packets.size() / ave_insert_time / 1e6;
    query_throughput = freq.size() / ave_query_time / 1e6;
    cout << "Elastic\t" << insert_throughput << "Mpps\t" << query_throughput << "Mpps" << endl;
    fprintf(file_out, "Elastic\n%lf %lf\n", insert_throughput, query_throughput);

    fclose(file_out);
}

int main()
{
    fileReader("/share/datasets/CAIDA2018/dataset/130000.dat");
    int mem_in_byte = 2 << 20;
    cout << "sketch\tinsert\tquery" << endl;
    test(mem_in_byte);
    return 0;
}
