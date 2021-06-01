#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "tower.h"
#include "cm.h"

using namespace std;

vector<string> packets;
unordered_map<string, int> freq;

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
		packets.push_back(str);
		freq[str]++;
	}
	inFile.close();

	cout << freq.size() << "flows, " << packets.size() << " packets read" << endl;
}

void test_CM(int mem_in_byte)
{
	int w = mem_in_byte / 4 / 5;
	CMSketch cm(w);

	for (auto key : packets)
		cm.insert(key.c_str(), key.length());

	double ARE = 0, AAE = 0;
	for (auto pr : freq)
	{
		int est_val = cm.query(pr.first.c_str(), pr.first.length());
		int real_val = pr.second;
		int dist = abs(est_val - real_val);
		ARE += (double)dist / real_val, AAE += dist;
	}
	ARE /= freq.size(), AAE /= freq.size();
	cout << "CM" << " " << ARE << "\t" << AAE << endl;
}

void test_CU(int mem_in_byte)
{
	int w = mem_in_byte / 4 / 5;
	CUSketch cu(w);

	for (auto key : packets)
		cu.insert(key.c_str(), key.length());

	double ARE = 0, AAE = 0;
	for (auto pr : freq)
	{
		int est_val = cu.query(pr.first.c_str(), pr.first.length());
		int real_val = pr.second;
		int dist = abs(est_val - real_val);
		ARE += (double)dist / real_val, AAE += dist;
	}
	ARE /= freq.size(), AAE /= freq.size();
	cout << "CU" << " " << ARE << "\t" << AAE << endl;
}

void test_tower(int mem_in_byte)
{
	int w = mem_in_byte / 4 / 5;
	TowerSketch tower(w);

	for (auto key : packets)
		tower.insert(key.c_str(), key.length());

	double ARE = 0, AAE = 0;
	for (auto pr : freq)
	{
		int est_val = tower.query(pr.first.c_str(), pr.first.length());
		int real_val = pr.second;
		int dist = abs(est_val - real_val);
		ARE += (double)dist / real_val, AAE += dist;
	}
	ARE /= freq.size(), AAE /= freq.size();
	cout << "tower" << " " << ARE << "\t" << AAE << endl;
}

void test_towerCU(int mem_in_byte)
{
	int w = mem_in_byte / 4 / 5;
	TowerSketchCU tower_cu(w);

	for (auto key : packets)
		tower_cu.insert(key.c_str(), key.length());

	double ARE = 0, AAE = 0;
	for (auto pr : freq)
	{
		int est_val = tower_cu.query(pr.first.c_str(), pr.first.length());
		int real_val = pr.second;
		int dist = abs(est_val - real_val);
		ARE += (double)dist / real_val, AAE += dist;
	}
	ARE /= freq.size(), AAE /= freq.size();
	cout << "tower+CU" << " " << ARE << "\t" << AAE << endl;
}

void test(int mem_in_byte)
{
	test_CM(mem_in_byte);
	test_CU(mem_in_byte);
	test_tower(mem_in_byte);
	test_towerCU(mem_in_byte);
}

int main()
{
	fileReader("path to dataset");
	int mem_in_byte = 2 << 20;
	test(mem_in_byte);
	return 0;
}
