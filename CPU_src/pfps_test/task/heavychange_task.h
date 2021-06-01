#ifndef _HC_TEST_H
#define _HC_TEST_H

#include "../sketch/host_sketches.h"
#include <unordered_map>
#include <unordered_set>

double calcHCAcc(HostSketches sketch[2][8], unordered_map<string, int> realFreq[2], int sw_num, int host_num)
{
	uint32_t tp = 0, fn = 0, fp = 0;
	for (int row_id = 0; row_id < sw_num; ++row_id)
	{
		unordered_set<string> real_hc;
		for (auto pr : realFreq[row_id])
			if (abs(pr.second) >= hh_thres)
				real_hc.insert(pr.first);

		unordered_set<string> est_hc;
		for (int i = 0; i < host_num; ++i)
		{
			auto ret_hc = sketch[0][i].getHC(sketch[1][i], row_id);
			est_hc.insert(ret_hc.begin(), ret_hc.end());
		}

		uint32_t tpi = 0, fni = 0, fpi = 0;
		for (auto key : est_hc)
			if (real_hc.find(key) == real_hc.end())
				++fpi;
		for (auto key : real_hc)
			if (est_hc.find(key) == est_hc.end())
				++fni;
		tpi = est_hc.size() - fpi;
		tp += tpi, fn += fni, fp += fpi;
	}

	double precision = (double)tp / (tp + fp);
	double recall = (double)tp / (tp + fn);
	double F1_score = 2 * precision * recall / (precision + recall);

	return F1_score;
}

#endif
