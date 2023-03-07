#ifndef CARDINALITY_H
#define CARDINALITY_H

#include "utils.h"
#include "tower.h"
#include "data.h"
#include "task_utils.h"
using namespace std;

/*
 * To test the performance (Relative Error) of tower+CM/tower+CU/CM/CU over measuring the cardinality of a stream of flows.
 */
void cardinality_test(TRACE *traces, FLOW_ITEM *items, int item_cnt, int memory, int sketch_id, int opt, FILE *output_file)
{
    double avg_RE = 0;
    char sketch_name[100];
    get_sketch_name(sketch_name, sketch_id);
    printf("%s\n", sketch_name);

    for (int rep = 0; rep < REP_TIME; rep++)
    {
        printf("%d\n", rep);
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
        avg_RE += fabs(item_cnt - cardinality) / (double)item_cnt / (double)REP_TIME;

        delete (sketch);
    }

    fprintf(output_file, "%s\n%.9f\n", sketch_name, avg_RE);
}

#endif