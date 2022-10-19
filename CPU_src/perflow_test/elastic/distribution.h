#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include "utils.h"
#include "data.h"
#include "task_utils.h"
using namespace std;

/*
 * To test the performance (WMRE) of elastic sketch over measuring the distribution of a stream of flows.
 */
template <int memory>
void distribution_test(TRACE *traces, FLOW_ITEM *items, int item_cnt, int sketch_id, int opt, FILE *output_file)
{
    double avg_WMRE = 0;
    char sketch_name[100];
    get_sketch_name(sketch_name, sketch_id);
    printf("%s\n", sketch_name);

    for (int rep = 0; rep < REP_TIME; rep++)
    {
        printf("%d\n", rep);
        ElasticSketch<memory / 1024, memory> *sketch = new ElasticSketch<memory / 1024, memory>();

        insert_all_packets(sketch, traces);

        vector<double> result_dist;
        result_dist.clear();

        sketch->get_distribution(result_dist);
        for (int i = result_dist.size(); i <= items[0].freq * 2; i++)
        {
            result_dist.push_back(0);
        }
        int *real_distri = new int[MAX_FLOW_SIZE];
        for (int i = 0; i <= 2 * items[0].freq; i++)
        {
            real_distri[i] = 0;
        }
        for (int i = 0; i < item_cnt; i++)
        {
            real_distri[items[i].freq] += 1;
        }

        double WMRE = 0;

        int cur_sum = 0;
        for (int i = 0; i <= 2 * items[0].freq; i++)
        {
            WMRE += fabs(real_distri[i] - result_dist[i]);
            cur_sum += result_dist[i];
        }
        WMRE /= (double)(cur_sum + item_cnt) / 2.0;

        avg_WMRE += WMRE / (double)REP_TIME;

        delete (sketch);
        delete[] real_distri;
    }

    printf("\n");
    fprintf(output_file, "%s\n%.9f\n", sketch_name, avg_WMRE);
}

#endif