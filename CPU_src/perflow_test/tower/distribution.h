#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include <mutex>
#include <thread>

#include "utils.h"
#include "tower.h"
#include "data.h"
#include "task_utils.h"
using namespace std;

void distribution_run(mutex *mtx, int rep_time, TRACE *traces, FLOW_ITEM *items, int item_cnt, int memory, int sketch_id, int opt, int thread_id, double *avg_WMRE) {
    for (int rep = 0; rep < rep_time; rep++)
    {
        printf("%d %d\n", thread_id, rep);
        TowerSketch *sketch;
        sketch = create_sketch(memory, opt, 0, sketch_id);

        insert_all_packets(sketch, traces);

        double *result_distri = new double[MAX_FLOW_SIZE];
        for (int i = 0; i <= 2 * items[0].freq; i++)
        {
            result_distri[i] = 0;
        }

        if (sketch_id >= 2 && sketch_id <= 3)
        {
            for (int i = 0; i < cs_cm.size(); i++)
            {
                sketch->query_distribution(result_distri, i);
            }
            for (int i = 0; i <= 2 * items[0].freq; i++)
            {
                result_distri[i] /= cs_cm.size();
            }
        }
        else
        {
            int thresholds[10];
            thresholds[0] = 1;
            for (int i = 1; i < cs_tower.size(); i++)
            {
                thresholds[i] = (1 << cs_tower[i - 1]) - 2;
            }
            thresholds[cs_tower.size()] = 2 * items[0].freq + 1;
            for (int i = 0; i < cs_tower.size(); i++)
            {
                double *tmp_distri = new double[MAX_FLOW_SIZE];
                for (int j = 0; j <= 2 * items[0].freq; j++)
                {
                    tmp_distri[j] = 0;
                }
                sketch->query_distribution(tmp_distri, i);
                for (int j = thresholds[i]; j < thresholds[i + 1]; j++)
                {
                    result_distri[j] = tmp_distri[j];
                }
            }
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
            WMRE += fabs((double)real_distri[i] - (double)result_distri[i]);
            cur_sum += result_distri[i];
        }

        // printf("%f\n", WMRE);
        WMRE /= (double)(cur_sum + item_cnt) / 2.0;

        mtx->lock();
        *avg_WMRE += WMRE / (double)REP_TIME;
        mtx->unlock();

        delete (sketch);
        delete[] result_distri;
        delete[] real_distri;
    }
}

/*
 * To test the performance (WMRE) of tower+CM/tower+CU/CM/CU over measuring the distribution of a stream of flows.
 */
void distribution_test(TRACE *traces, FLOW_ITEM *items, int item_cnt, int memory, int sketch_id, int opt, FILE *output_file)
{
    double avg_WMRE = 0;
    char sketch_name[100];
    get_sketch_name(sketch_name, sketch_id);
    printf("%s\n", sketch_name);

    mutex mtx;
    thread threads[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; ++i)
    {
        threads[i] = thread(distribution_run, &mtx, REP_TIME / THREAD_NUM, traces, items, item_cnt, memory, sketch_id, opt, i, &avg_WMRE);
    }
    for (int i = 0; i < THREAD_NUM; ++i)
        threads[i].join();

    printf("\n");
    fprintf(output_file, "%s\n%.9f\n", sketch_name, avg_WMRE);
}

#endif