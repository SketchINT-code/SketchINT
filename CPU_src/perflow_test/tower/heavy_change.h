#ifndef HEAVY_CHANGE_H
#define HEAVY_CHANEG_H

#include <mutex>
#include <thread>

#include "utils.h"
#include "tower.h"
#include "data.h"
#include "task_utils.h"
using namespace std;

void heavy_change_run(mutex *mtx, int rep_time, TRACE *traces, FLOW_ITEM *items, int item_cnt, TRACE *traces2, FLOW_ITEM *items2, int item_cnt2, int memory, int sketch_id, int opt, int threshold, int thread_id, double *avg_PR, double *avg_RC, double *avg_ARE, double *avg_F1) {
    for (int rep = 0; rep < rep_time; rep++){
        printf("%d %d\n", thread_id, rep);
        TowerSketch* sketch;
        sketch = create_sketch(memory, opt, threshold, sketch_id);

        insert_all_packets(sketch, traces);

        TowerSketch* sketch2;
        sketch2 = create_sketch(memory, opt, threshold, sketch_id);
        insert_all_packets(sketch2, traces2);

        unordered_map<FIVE_TUPLE, int, Hash_Fun>* result1 = new unordered_map<FIVE_TUPLE, int, Hash_Fun>;
        unordered_map<FIVE_TUPLE, int, Hash_Fun>* result2 = new unordered_map<FIVE_TUPLE, int, Hash_Fun>;
        unordered_map<FIVE_TUPLE, int, Hash_Fun>* result = new unordered_map<FIVE_TUPLE, int, Hash_Fun>;
        result->clear();
        sketch->query_heavyhitter(result1);
        sketch2->query_heavyhitter(result2);

        unordered_map<FIVE_TUPLE, int, Hash_Fun>::iterator it;
        for (it = result1->begin(); it != result1->end(); it++){
            (*result)[it->first] = sketch->query(it->first.key, TUPLE_LEN, 0) - sketch2->query(it->first.key, TUPLE_LEN, 0);
        }
        for (it = result2->begin(); it != result2->end(); it++){
            (*result)[it->first] = sketch->query(it->first.key, TUPLE_LEN, 0) - sketch2->query(it->first.key, TUPLE_LEN, 0);
        }

        unordered_map<FIVE_TUPLE, int, Hash_Fun>* answer = new unordered_map<FIVE_TUPLE, int, Hash_Fun>;
        answer->clear();

        for (int i = 0; i < item_cnt; i++){
            (*answer)[items[i].key] += items[i].freq;
        }
        for (int i = 0; i < item_cnt2; i++){
            (*answer)[items2[i].key] -= items2[i].freq;
        }

        int tot_real = 0, tot_report = 0, hit = 0;
        double ARE = 0;
        for (it = answer->begin(); it != answer->end(); it++){
            int real_freq = abs(it->second);
            if (real_freq >= threshold){
                tot_real++;
                int est_freq = abs((*result)[it->first]);
                if (est_freq >= threshold){
                    hit++;
                    ARE += fabs(est_freq - real_freq) / (double)real_freq;
                }
            }
        }

        for (it = result->begin(); it != result->end(); it++){
            if (abs(it->second) >= threshold){
                tot_report++;
            }
        }

        mtx->lock();
        *avg_PR += (double)(hit) / (double)tot_report / (double)REP_TIME;
        *avg_RC += (double)(hit) / (double)tot_real / (double)REP_TIME;
        *avg_ARE += ARE / (double)REP_TIME / (double)hit;
        *avg_F1 += 2 * (double)hit / (double)(tot_report + tot_real) / (double)REP_TIME;
        mtx->unlock();

        delete(sketch);
        delete(result);
        delete(result2);
        delete(sketch2);
        delete(answer);
        delete(result1);
    }
}

/*
 * To test the performance (F_1 Score) of tower+CM/tower+CU/CM/CU over reporting heavy change flows.
 * Here, "heavy change flows" are defined as: {e_i| |f_i - f'_i| >= total * threshold}
 */
void heavy_change_test(TRACE* traces, FLOW_ITEM* items, int item_cnt, int memory, int sketch_id, int opt, int threshold, FILE* output_file, char* extra_file_name){
    double avg_PR = 0;
    double avg_RC = 0;
    double avg_ARE = 0;
    double avg_F1 = 0;

    char sketch_name[100];
    get_sketch_name(sketch_name, sketch_id);
    printf("%s\n", sketch_name);

    TRACE* traces2;
    FLOW_ITEM* items2;
    int item_cnt2;

    ReadInTraces(extra_file_name, traces2, items2, item_cnt2);

    mutex mtx;
    thread threads[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; ++i)
    {
        threads[i] = thread(heavy_change_run, &mtx, REP_TIME / THREAD_NUM, traces, items, item_cnt, traces2, items2, item_cnt2, memory, sketch_id, opt, threshold, i, &avg_PR, &avg_RC, &avg_ARE, &avg_F1);
    }
    for (int i = 0; i < THREAD_NUM; ++i)
        threads[i].join();

    printf("\n");
    fprintf(output_file, "%s\n%.9f %.9f %.9f %.9f\n", sketch_name, avg_PR, avg_RC, avg_F1, avg_ARE);

    delete(traces2);
    delete[] items2;
}

#endif
