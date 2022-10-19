#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include "tower.h"

using namespace std;

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

void test_tower(int mem_in_byte, int thread_num)
{
	int w = mem_in_byte / 4 / 5;
	TowerSketch tower(w);

	thread threads[MAX_THREAD_NUM];
	for (int i = 0; i < thread_num; ++i)
		threads[i] = thread(insert_tower, &tower, i);
	for (int i = 0; i < thread_num; ++i)
		threads[i].join();

	double ARE = 0, AAE = 0;
	for (auto pr : freq)
	{
		int est_val = tower.query(pr.first.c_str(), pr.first.length());
		int real_val = pr.second;
		int dist = abs(est_val - real_val);
		ARE += (double)dist / real_val, AAE += dist;
	}
	ARE /= freq.size(), AAE /= freq.size();
	cout << ARE << "\t"/* << AAE << "\t"*/;
	cerr << thread_num << " " << ARE << endl;
    fprintf(file_out, "Tower\n%lf %lf\n", ARE, AAE);
}

void test_towerCU(int mem_in_byte, int thread_num)
{
	int w = mem_in_byte / 4 / 5;
	TowerSketchCU tower_cu(w);

	thread threads[MAX_THREAD_NUM];
	for (int i = 0; i < thread_num; ++i)
		threads[i] = thread(insert_tower_cu, &tower_cu, i);
	for (int i = 0; i < thread_num; ++i)
		threads[i].join();

	double ARE = 0, AAE = 0;
	for (auto pr : freq)
	{
		int est_val = tower_cu.query(pr.first.c_str(), pr.first.length());
		int real_val = pr.second;
		int dist = abs(est_val - real_val);
		ARE += (double)dist / real_val, AAE += dist;
	}
	ARE /= freq.size(), AAE /= freq.size();
	cout << ARE << "\t"/* << AAE << "\t"*/;
    fprintf(file_out, "TowerCU\n%lf %lf\n", ARE, AAE);
}

void test_towerACU(int mem_in_byte, int thread_num)
{
    int w = mem_in_byte / 4 / 5;
    TowerSketchACU tower_acu(w);

    thread threads[MAX_THREAD_NUM];
    for (int i = 0; i < thread_num; ++i)
        threads[i] = thread(insert_tower_acu, &tower_acu, i);
    for (int i = 0; i < thread_num; ++i)
        threads[i].join();

    double ARE = 0, AAE = 0;
    for (auto pr : freq)
    {
        int est_val = tower_acu.query(pr.first.c_str(), pr.first.length());
        int real_val = pr.second;
        int dist = abs(est_val - real_val);
        ARE += (double)dist / real_val, AAE += dist;
    }
    ARE /= freq.size(), AAE /= freq.size();
    cout << ARE << "\t" << AAE << "\t";
    fprintf(file_out, "TowerACU\n%lf %lf\n", ARE, AAE);
}

void test(int mem_in_byte, int thread_num)
{
	cout << thread_num << "\t";
	for (int i = 0; i < thread_num; ++i)
		packets[i].clear();
	for (int i = 0; i < trace.size(); ++i)
		packets[MurmurHash3_x86_32(trace[i].c_str(), trace[i].length(), 1000) % thread_num].push_back(trace[i]);

	test_tower(mem_in_byte, thread_num);
	test_towerCU(mem_in_byte, thread_num);
    test_towerACU(mem_in_byte, thread_num);
	cout << endl;
}

int main()
{
    fileReader("/share/datasets/CAIDA2018/dataset/130000.dat");
    cout << "thread_num\tTower\tTower+CU" << endl;
    int mem_in_byte = 2 << 20;
    file_out = fopen("result_acc.txt", "w");
    test(mem_in_byte, 1);
    for (int thread_num = 10; thread_num <= 40; thread_num += 10)
        test(mem_in_byte, thread_num);
    fclose(file_out);
    return 0;
}