#ifndef _HOST_SKETCHES_H
#define _HOST_SKETCHES_H

#include <string>
#include <algorithm>
#include "tower.h"
#include "heavyhitter.h"

#include<iostream>

using namespace std;

const uint32_t KEY_LEN = 16;
struct Packet
{
	char key[KEY_LEN];
	vector<uint16_t> sid;
	vector<uint64_t> tstamp;
	vector<uint32_t> deltat;
};

enum Task { FREQUENCY, HEAVYHITTER, HEAVYCHANGE, LATENCY };

class HostSketches
{
private:
	TowerSketch freq, delay;
	HeavyHitter hh;

public:
	Task task;
	uint32_t ehn;
	HostSketches() {}
	HostSketches(uint32_t mem_in_byte, uint32_t h, Task in_task, uint32_t seed_num)
	{
		init(mem_in_byte, h, in_task, seed_num);
	}

	void init(uint32_t mem_in_byte, uint32_t h, Task in_task, uint32_t seed_num)
	{
		task = in_task;
		if (task == FREQUENCY)
		{
			uint32_t min_w = mem_in_byte / (4 * 3);
			vector<uint32_t> w = { min_w * 4, min_w * 2, min_w };
			vector<uint8_t> counter_len_freq = { 8, 16, 32 };
			freq.init(w, 1, counter_len_freq, seed_num);
		}
		if (task == HEAVYHITTER || task == HEAVYCHANGE)
		{
			uint32_t min_w = mem_in_byte / (4 * 3);
			vector<uint32_t> w = { min_w, min_w, min_w };
			vector<uint8_t> counter_len_freq = { 32, 32, 32 };
			freq.init(w, 1, counter_len_freq, seed_num);
		}
		if (task == LATENCY)
		{
			uint32_t min_w = mem_in_byte / (4 * 3 + 16 + 8 + 4);
			vector<uint32_t> w = { min_w * 4, min_w * 2, min_w };
			vector<uint8_t> counter_len_freq = { 8, 16, 32 };
			vector<uint8_t> counter_len_delay = { 32, 32, 32 };
			freq.init(w, 1, counter_len_freq, seed_num);
			delay.init(w, 1, counter_len_delay, seed_num);
		}
	}

	void insert(const Packet& pkt)
	{
		if (task == FREQUENCY)
		{
			for (uint32_t i = 0; i < pkt.sid.size(); ++i)
				freq.insert(pkt.key, KEY_LEN, PH(pkt.sid[i]), 1);
		}
		if (task == HEAVYHITTER || task == HEAVYCHANGE)
		{
			freq.insert(pkt.key, KEY_LEN, 0, 1);
			uint32_t ret_freq = queryFreq(pkt.key, 0);
			hh.insert(pkt.key, KEY_LEN, ret_freq, ehn, pkt.sid);
		}
		if (task == LATENCY)
		{
			char key[KEY_LEN + 1];
			memcpy(key, pkt.key, KEY_LEN);
			for (uint32_t i = 0; i < pkt.sid.size(); ++i)
			{
				key[KEY_LEN] = PH(pkt.sid[i]);
				freq.insert(key, KEY_LEN + 1, 0, 1);
				delay.insert(key, KEY_LEN + 1, 0, pkt.deltat[i]);
			}
		}
	}

	uint32_t queryFreq(const char* flow_id, uint32_t row_id)
	{
		char key[KEY_LEN + 1];
		memcpy(key, flow_id, KEY_LEN);
		key[KEY_LEN] = row_id;
		return freq.query(key, KEY_LEN + 1, 0);
	}

	double queryAveDelay(const char* flow_id, uint32_t row_id)
	{
		char key[KEY_LEN + 1];
		memcpy(key, flow_id, KEY_LEN);
		key[KEY_LEN] = row_id;
		vector<uint32_t> ret_freq = freq.queryAll(key, KEY_LEN + 1, 0);
		vector<uint32_t> ret_delay = delay.queryAll(key, KEY_LEN + 1, 0);
		vector<pair<uint32_t, double>> fd_pair;
		for (uint8_t i_d = 0; i_d < ret_freq.size(); ++i_d)
			if (ret_freq[i_d] != UINT32_MAX && ret_delay[i_d] != UINT32_MAX)
			{
				double ave_delay = (double)ret_delay[i_d] / ret_freq[i_d];
				fd_pair.push_back(make_pair(ret_freq[i_d], ave_delay));
			}
		sort(fd_pair.begin(), fd_pair.end());
		return fd_pair[0].second;
	}

	unordered_set<string> getHH(uint32_t row_id)
	{
		return hh.getHH(row_id);
	}

	unordered_set<string> getHC(HostSketches& B, uint32_t row_id)
	{
		auto ret_hh1 = getHH(row_id);
		auto ret_hh2 = B.getHH(row_id);
		ret_hh1.insert(ret_hh2.begin(), ret_hh2.end());
		unordered_set<string> ret_hc;
		for (auto key : ret_hh1)
		{
			int ret_freq1 = queryFreq(key.c_str(), 0);
			int ret_freq2 = B.queryFreq(key.c_str(), 0);
			int delta = abs(ret_freq1 - ret_freq2);
			if (delta >= hh_thres)
				ret_hc.insert(key);
		}
		return ret_hc;
	}
};

#endif
