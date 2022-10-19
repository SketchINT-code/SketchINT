#ifndef CARDINALITY_H
#define CARDINALITY_H

#include "utils.h"
#include "data.h"
#include "task_utils.h"
using namespace std;

/*
 * To test the performance (Relative Error) of elastic sketch over measuring the cardinality of a stream of flows.
 */
template <int memory>
void cardinality_test(TRACE *traces, FLOW_ITEM *items, int item_cnt, int sketch_id, int opt, FILE *output_file)
{
    double avg_RE = 0;
    char sketch_name[100];
    get_sketch_name(sketch_name, sketch_id);
    printf("%s\n", sketch_name);

    for (int rep = 0; rep < REP_TIME; rep++)
    {
        printf("%d\n", rep);
        ElasticSketch<memory / 1024, memory> *sketch = new ElasticSketch<memory / 1024, memory>();

        insert_all_packets(sketch, traces);

        int cardinality = 0;
        cardinality = sketch->get_cardinality();

        if (rep % 20 == 19)
            printf("%d %d %d\n", memory, item_cnt, cardinality);

        avg_RE += fabs(item_cnt - cardinality) / (double)item_cnt / (double)REP_TIME;

        delete (sketch);
    }

    printf("\n");
    fprintf(output_file, "%s\n%.9f\n", sketch_name, avg_RE);
}

#endif