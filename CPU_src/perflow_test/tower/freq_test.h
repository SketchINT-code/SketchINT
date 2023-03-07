#ifndef FREQ_TEST_H
#define FREQ_TEST_H

#include <mutex>
#include <thread>

#include "utils.h"
#include "tower.h"
#include "data.h"
#include "task_utils.h"
using namespace std;

void frequency_run(mutex *mtx, int rep_time, TRACE *traces, FLOW_ITEM *items, int item_cnt, int memory, int sketch_id, int opt, int thread_id, double *avg_ARE, double *avg_AAE)
{
    for (int rep = 0; rep < rep_time; rep++)
    {
        printf("%d %d\n", thread_id, rep);
        TowerSketch *sketch;
        sketch = create_sketch(memory, opt, 0, sketch_id);

        insert_all_packets(sketch, traces);

        double ARE = 0;
        double AAE = 0;

        for (int i = 0; i < item_cnt; i++)
        {
            double freq = sketch->query(items[i].key.key, TUPLE_LEN, 0);
            ARE += fabs(freq - items[i].freq) / (double)items[i].freq;
            AAE += fabs(freq - items[i].freq);

#ifdef DEBUG_MODE
            if (items[i].freq > freq)
                printf("est_val < real_val !!!\n");
#endif
        }

        ARE /= (double)item_cnt;
        AAE /= (double)item_cnt;

        mtx->lock();
        *avg_ARE += ARE / (double)REP_TIME;
        *avg_AAE += AAE / (double)REP_TIME;
        mtx->unlock();

        delete (sketch);
    }
}

/*
 * To test the performance (AAE, ARE) of tower+CM/tower+CU/CM/CU over reporting heavy change flows.
 */
void frequency_test(TRACE *traces, FLOW_ITEM *items, int item_cnt, int memory, int sketch_id, int opt, FILE *output_file)
{
    double avg_ARE = 0;
    double avg_AAE = 0;
    char sketch_name[100];
    get_sketch_name(sketch_name, sketch_id);
    printf("%s\n", sketch_name);
    mutex mtx;
    thread threads[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; ++i)
    {
        threads[i] = thread(frequency_run, &mtx, REP_TIME / THREAD_NUM, traces, items, item_cnt, memory, sketch_id, opt, i, &avg_ARE, &avg_AAE);
    }
    for (int i = 0; i < THREAD_NUM; ++i)
        threads[i].join();

    printf("\n");
    fprintf(output_file, "%s\n%.9f %.9f\n", sketch_name, avg_ARE, avg_AAE);
}

#endif