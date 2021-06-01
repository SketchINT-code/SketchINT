#ifndef _FREQUENCY_TASK_H
#define _FREQUENCY_TASK_H

#include "../sketch/host_sketches.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>

double calcFreqAcc(HostSketches sketch[8], unordered_map<string, int>* realFreq, int sw_num)
{
	double ARE = 0, AAE = 0; int flow_cnt = 0;
	for (int row_id = 0; row_id < sw_num; ++row_id)
	{
		for (auto it = realFreq[row_id].begin(); it != realFreq[row_id].end(); ++it)
		{
			double real_val = it->second;
			int p=it->first[4]-'0';
			string s=it->first.substr(0,4);
			uint32_t est_val=sketch[p].queryFreq(s.c_str(), row_id);
			double dist = abs(real_val - est_val);
			ARE += dist / real_val; AAE += dist;
		}
		flow_cnt += realFreq[row_id].size();
	}
	ARE /= flow_cnt;
	return ARE;
}

#endif
