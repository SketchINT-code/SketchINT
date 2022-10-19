#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include <chrono>
#include "tower.h"

using namespace std;
using namespace std::chrono;

const int WARM_UP_TIME = 10;
const int RUN_TIME = 10;
const int MAX_THREAD_NUM = 40;

vector<string> trace;
unordered_map<string, int> freq;
vector<string> packets[MAX_THREAD_NUM];
FILE *file_out;

void fileReader(const char* filename, int MAX_ITEM = INT32_MAX)
{
	ifstream inFile(filename, ios::binary);
	ios::sync_with_stdio(false);

	char key[13], timestamp[8];
	for (int i = 0; i < MAX_ITEM; ++i)
	{
		inFile.read(key, 13);
		if (inFile.gcount() < 13) break;
		inFile.read(timestamp, 8);
		string str = string(key, 4);
		trace.push_back(str);
		freq[str]++;
	}
	inFile.close();

	cout << freq.size() << "flows, " << trace.size() << " packets read" << endl;
}

void insert_tower(TowerSketch* tower, int thread_id)
{
	for (auto key : packets[thread_id])
		tower->insert(key.c_str(), key.length());
}

void insert_tower_cu(TowerSketchCU* tower_cu, int thread_id)
{
	for (auto key : packets[thread_id])
		tower_cu->insert(key.c_str(), key.length());
}

void insert_tower_acu(TowerSketchACU *tower_acu, int thread_id)
{
    for (auto key : packets[thread_id])
        tower_acu->insert(key.c_str(), key.length());
}

double test_tower(int mem_in_byte, int thread_num)
{
	int w = mem_in_byte / 4 / 5;
	TowerSketch tower(w);

	steady_clock::time_point t1 = steady_clock::now();

	thread threads[MAX_THREAD_NUM];
	for (int i = 0; i < thread_num; ++i)
		threads[i] = thread(insert_tower, &tower, i);
	for (int i = 0; i < thread_num; ++i)
		threads[i].join();

	steady_clock::time_point t2 = steady_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	return time_span.count();
}

double test_towerCU(int mem_in_byte, int thread_num)
{
	int w = mem_in_byte / 4 / 5;
	TowerSketchCU tower_cu(w);

	steady_clock::time_point t1 = steady_clock::now();

	thread threads[MAX_THREAD_NUM];
	for (int i = 0; i < thread_num; ++i)
		threads[i] = thread(insert_tower_cu, &tower_cu, i);
	for (int i = 0; i < thread_num; ++i)
		threads[i].join();

	steady_clock::time_point t2 = steady_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	return time_span.count();
}

double test_towerACU(int mem_in_byte, int thread_num)
{
    int w = mem_in_byte / 4 / 5;
    TowerSketchACU tower_acu(w);

    steady_clock::time_point t1 = steady_clock::now();

    thread threads[MAX_THREAD_NUM];
    for (int i = 0; i < thread_num; ++i)
        threads[i] = thread(insert_tower_acu, &tower_acu, i);
    for (int i = 0; i < thread_num; ++i)
        threads[i].join();

    steady_clock::time_point t2 = steady_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    return time_span.count();
}

void test(int mem_in_byte, int thread_num)
{
	cout << thread_num << "\t";
	for (int i = 0; i < thread_num; ++i)
		packets[i].clear();
	for (int i = 0; i < trace.size(); ++i)
		packets[MurmurHash3_x86_32(trace[i].c_str(), trace[i].length(), 1000) % thread_num].push_back(trace[i]);

	double ave_time = 0;
	for (int i = 0; i < WARM_UP_TIME + RUN_TIME; ++i)
	{
		double insert_time = test_tower(mem_in_byte, thread_num);
		if (i >= WARM_UP_TIME) ave_time += insert_time;
	}
	ave_time /= RUN_TIME;
	double throughput = trace.size() / ave_time / 1e6;
	cout << throughput << "Mpps\t";
	cerr << thread_num << " " << throughput << endl;
    fprintf(file_out, "Tower\n%lf\n", throughput);

	ave_time = 0;
	for (int i = 0; i < WARM_UP_TIME + RUN_TIME; ++i)
	{
		double insert_time = test_towerCU(mem_in_byte, thread_num);
		if (i >= WARM_UP_TIME) ave_time += insert_time;
	}
	ave_time /= RUN_TIME;
	throughput = trace.size() / ave_time / 1e6;
	cout << throughput << "Mpps" << endl;
    fprintf(file_out, "TowerCU\n%lf\n", throughput);

    ave_time = 0;
    for (int i = 0; i < WARM_UP_TIME + RUN_TIME; ++i)
    {
        double insert_time = test_towerACU(mem_in_byte, thread_num);
        if (i >= WARM_UP_TIME)
            ave_time += insert_time;
    }
    ave_time /= RUN_TIME;
    throughput = trace.size() / ave_time / 1e6;
    cout << throughput << "Mpps" << endl;
    fprintf(file_out, "TowerACU\n%lf\n", throughput);
}

int main()
{
	fileReader("/share/datasets/CAIDA2018/dataset/130000.dat");
    cout << "thread_num\tTower\tTower+CU" << endl;
    int mem_in_byte = 2 << 20;
    file_out = fopen("result.txt", "w");
    test(mem_in_byte, 1);
    for (int thread_num = 10; thread_num <= 40; thread_num += 10)
        test(mem_in_byte, thread_num);
    fclose(file_out);
    return 0;
}
