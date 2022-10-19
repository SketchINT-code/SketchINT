#ifndef HEAVY_HITTER_H
#define HEAVY_HITTER_H

#include "utils.h"
#include "data.h"
#include "task_utils.h"
using namespace std;

/*
 * To test the performance (F_1 Score, ARE) of elastic sketch over reporting heavy hitters.
 * Here, "heavy hitters" are defined as: {e_i| f_i >= total * threshold}
 */
template <int memory>
void heavy_hitter_test(TRACE* traces, FLOW_ITEM* items, int item_cnt, int sketch_id, int opt, int threshold, FILE* output_file){
    double avg_PR = 0;
    double avg_RC = 0;
    double avg_ARE = 0;
    double avg_F1 = 0;

    char sketch_name[100];
    get_sketch_name(sketch_name, sketch_id);
    printf("%s\n", sketch_name);

    for (int rep = 0; rep < REP_TIME; rep++){
        printf("%d\n", rep);
        ElasticSketch<memory / 256, memory>* sketch = new ElasticSketch<memory / 256, memory>();

        insert_all_packets(sketch, traces);

        vector<pair<string, int> > result;
        sketch->get_heavy_hitters(threshold, result);
        unordered_map<FIVE_TUPLE, int, Hash_Fun> result_map;
        for (int i = 0; i < result.size(); i++){
            FIVE_TUPLE tmp;
            for (int j = 0; j < 13; j++){
                tmp.key[j] = 0;
            }
            memcpy(tmp.key, result[i].first.c_str(), 4);
            result_map[tmp] = result[i].second;
        }

        int tot_real = 0, hit = 0;
        double ARE = 0;
        for (int i = 0; i < item_cnt; i++){
            int real_freq = items[i].freq;
            if (real_freq < threshold){
                break;
            }

            tot_real++;
            int est_freq = result_map[items[i].key];
            if (est_freq){
                hit++;
                ARE += fabs(est_freq - real_freq) / (double)real_freq;
            }
        }

        avg_PR += (double)(hit) / (double)result_map.size() / (double)REP_TIME;
        avg_RC += (double)(hit) / (double)tot_real / (double)REP_TIME;
        avg_ARE += ARE / (double)REP_TIME / (double)hit;
        avg_F1 += 2 * (double)hit / (double)(result_map.size() + tot_real) / (double)REP_TIME;

        delete(sketch);
    }

    printf("\n");
    fprintf(output_file, "%s\n%.9f %.9f %.9f %.9f\n", sketch_name, avg_PR, avg_RC, avg_F1, avg_ARE);
}

#endif