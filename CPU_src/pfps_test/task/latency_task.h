#ifndef _LATENCY_TASK_H
#define _LATENCY_TASK_H

#include "../sketch/host_sketches.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>

double calcLatencyAcc(HostSketches sketch[8], unordered_map<string, int>* realFreq, unordered_map<string, int>* realTotL, int sw_num)
{
	double ARE = 0, AAE = 0; int flow_cnt = 0;
	for (int row_id = 0; row_id < sw_num; ++row_id)
	{
		for (auto it = realFreq[row_id].begin(); it != realFreq[row_id].end(); ++it)
		{
			double real_val = (double)realTotL[row_id][it->first] / it->second;
			int p=it->first[16]-'0';
			string s=it->first.substr(0,16);
			double est_val = sketch[p].queryAveDelay(s.c_str(), row_id);
			double dist = abs(real_val - est_val);
			ARE += dist / real_val; AAE += dist;
		}
		flow_cnt += realFreq[row_id].size();
	}
	ARE /= flow_cnt;
	return ARE;
}

double calcBurstAcc(unordered_set<string>* est_burst, unordered_set<string>* real_burst, int sw_num)
{
	uint32_t tp = 0, fn = 0, fp = 0;
	for (int row_id = 0; row_id < sw_num; ++row_id)
	{
		uint32_t tpi = 0, fni = 0, fpi = 0;
		for (auto key : est_burst[row_id])
			if (real_burst[row_id].find(key) == real_burst[row_id].end())
				++fpi;
		for (auto key : real_burst[row_id])
			if (est_burst[row_id].find(key) == est_burst[row_id].end())
				++fni;
		tpi = est_burst[row_id].size() - fpi;
		tp += tpi, fn += fni, fp += fpi;
	}
	double precision = (double)tp / (tp + fp);
	double recall = (double)tp / (tp + fn);
	double F1_score = 2 * precision * recall / (precision + recall);

	return F1_score;
}

#endif
