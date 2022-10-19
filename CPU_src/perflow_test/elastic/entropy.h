#ifndef ENTROPY_H
#define ENTROPY_H

#include "utils.h"
#include "data.h"
#include "task_utils.h"
using namespace std;

/*
 * To test the performance (Relative Error) of elastic sketch over measuring the entropy of flows.
 */
template <int memory>
void entropy_test(TRACE* traces, FLOW_ITEM* items, int item_cnt, int sketch_id, int opt, FILE* output_file){
    double avg_RE = 0;
    char sketch_name[100];
    get_sketch_name(sketch_name, sketch_id);
    printf("%s\n", sketch_name);

    for (int rep = 0; rep < REP_TIME; rep++){
        printf("%d\n", rep);
        ElasticSketch<memory / 256, memory>* sketch = new ElasticSketch<memory / 256, memory>;

        insert_all_packets(sketch, traces);

        int tot_packets = 0;
        for (int i = 0; i < item_cnt; i++){
            tot_packets += items[i].freq;
        }
        double real_entro = 0;
        for (int i = 0; i < item_cnt; i++){
            real_entro += items[i].freq / (double)tot_packets * log(items[i].freq / (double)tot_packets) / log(2);
        }
        real_entro = -real_entro;

        double est_entro;
        est_entro = sketch->get_entropy();

        avg_RE += fabs(est_entro - real_entro) / real_entro / (double)REP_TIME;

        delete(sketch);
    }

    printf("\n");
    fprintf(output_file, "%s\n%.9f\n", sketch_name, avg_RE);
}

#endif