#ifndef CARDINALITY_H
#define CARDINALITY_H

#include <mutex>
#include <thread>

#include "utils.h"
#include "tower.h"
#include "data.h"
#include "task_utils.h"
using namespace std;

void cardinality_run(mutex *mtx, int rep_time, TRACE *traces, FLOW_ITEM *items, int item_cnt, int memory, int sketch_id, int opt, int thread_id, double *avg_RE)
{
    for (int rep = 0; rep < rep_time; rep++)
    {
        printf("%d %d\n", thread_id, rep);
        TowerSketch *sketch;
        sketch = create_sketch(memory, opt, 0, sketch_id);

        insert_all_packets(sketch, traces);

        int cardinality = 0;
        if (sketch_id <= 1 || sketch_id >= 4)
        { // Tower Sketch
            cardinality = sketch->query_cardinality(0);
        }
        else
        {
            for (int i = 0; i < cs_cm.size(); i++)
            {
                cardinality += sketch->query_cardinality(i);
            }
            cardinality /= cs_cm.size();
        }

        printf("%d %d\n", item_cnt, cardinality);
        mtx->lock();
        *avg_RE += fabs(item_cnt - cardinality) / (double)item_cnt / (double)REP_TIME;
        mtx->unlock();

        delete (sketch);
    }
}

/*
 * To test the performance (Relative Error) of tower+CM/tower+CU/CM/CU over measuring the cardinality of a stream of flows.
 */
void cardinality_test(TRACE *traces, FLOW_ITEM *items, int item_cnt, int memory, int sketch_id, int opt, FILE *output_file)
{
    double avg_RE = 0;
    char sketch_name[100];
    get_sketch_name(sketch_name, sketch_id);
    printf("%s\n", sketch_name);

    mutex mtx;
    thread threads[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; ++i)
    {
        threads[i] = thread(cardinality_run, &mtx, REP_TIME / THREAD_NUM, traces, items, item_cnt, memory, sketch_id, opt, i, &avg_RE);
    }
    for (int i = 0; i < THREAD_NUM; ++i)
        threads[i].join();

    fprintf(output_file, "%s\n%.9f\n", sketch_name, avg_RE);
}

#endif