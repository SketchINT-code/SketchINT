#ifndef HEAVY_HITTER_H
#define HEAVY_HITTER_H

#include <mutex>
#include <thread>

#include "utils.h"
#include "tower.h"
#include "data.h"
#include "task_utils.h"
using namespace std;

void heavy_hitter_run(mutex *mtx, int rep_time, TRACE *traces, FLOW_ITEM *items, int item_cnt, int memory, int sketch_id, int opt, int threshold, int thread_id, double *avg_PR, double *avg_RC, double *avg_ARE, double *avg_F1)
{
    for (int rep = 0; rep < rep_time; rep++)
    {
        printf("%d %d\n", thread_id, rep);
        TowerSketch *sketch;
        sketch = create_sketch(memory, opt, threshold, sketch_id);

        insert_all_packets(sketch, traces);

        unordered_map<FIVE_TUPLE, int, Hash_Fun> *result = new unordered_map<FIVE_TUPLE, int, Hash_Fun>;
        sketch->query_heavyhitter(result);

        int tot_real = 0, hit = 0;
        double ARE = 0;
        for (int i = 0; i < item_cnt; i++)
        {
            int real_freq = items[i].freq;
            if (real_freq < threshold)
            {
                break;
            }

            tot_real++;
            int est_freq = (*result)[items[i].key];
            if (est_freq)
            {
                hit++;
                ARE += fabs(est_freq - real_freq) / (double)real_freq;
            }
        }

        mtx->lock();
        *avg_PR += (double)(hit) / (double)result->size() / (double)REP_TIME;
        *avg_RC += (double)(hit) / (double)tot_real / (double)REP_TIME;
        *avg_ARE += ARE / (double)REP_TIME / (double)hit;
        *avg_F1 += 2 * (double)hit / (double)(result->size() + tot_real) / (double)REP_TIME;
        mtx->unlock();

        delete (sketch);
        delete (result);
    }
}

/*
 * To test the performance (F_1 Score, ARE) of tower+CM/tower+CU/CM/CU over reporting heavy hitters.
 * Here, "heavy hitters" are defined as: {e_i| f_i >= total * threshold}
 */
void heavy_hitter_test(TRACE *traces, FLOW_ITEM *items, int item_cnt, int memory, int sketch_id, int opt, int threshold, FILE *output_file)
{
    double avg_PR = 0;
    double avg_RC = 0;
    double avg_ARE = 0;
    double avg_F1 = 0;

    char sketch_name[100];
    get_sketch_name(sketch_name, sketch_id);
    printf("%s\n", sketch_name);

    mutex mtx;
    thread threads[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; ++i)
    {
        threads[i] = thread(heavy_hitter_run, &mtx, REP_TIME / THREAD_NUM, traces, items, item_cnt, memory, sketch_id, opt, threshold, i, &avg_PR, &avg_RC, &avg_ARE, &avg_F1);
    }
    for (int i = 0; i < THREAD_NUM; ++i)
        threads[i].join();

    printf("\n");
    fprintf(output_file, "%s\n%.9f %.9f %.9f %.9f\n", sketch_name, avg_PR, avg_RC, avg_F1, avg_ARE);
}

#endif