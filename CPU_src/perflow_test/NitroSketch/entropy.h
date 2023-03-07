#ifndef ENTROPY_H
#define ENTROPY_H

#include "utils.h"
#include "tower.h"
#include "data.h"
#include "task_utils.h"
using namespace std;

/*
 * To test the performance (Relative Error) of tower+CM/tower+CU/CM/CU over measuring the entropy of flows.
 */
void entropy_test(TRACE *traces, FLOW_ITEM *items, int item_cnt, int memory, int sketch_id, int opt, FILE *output_file)
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

        int tot_packets = sketch->tot_packets;
        double real_entro = 0;
        for (int i = 0; i < item_cnt; i++){
            real_entro += items[i].freq / (double)tot_packets * log(items[i].freq / (double)tot_packets) / log(2);
        }
        real_entro = -real_entro;

        double est_entro;
        est_entro = sketch->query_entropy();

        /*
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

        double est_entro = 0, M = 0;
        for (int i = 0; i <= 2 * items[0].freq; i++)
            M += i * result_distri[i];
        for (int i = 0; i <= 2 * items[0].freq; i++)
            est_entro += -result_distri[i] * i / M * log(i / M);

        double real_entro = 0;
        M = 0;
        for (int i = 0; i <= 2 * items[0].freq; i++)
            M += i * real_distri[i];
        for (int i = 0; i <= 2 * items[0].freq; i++)
            real_entro += -real_distri[i] * i / M * log(i / M);
        */

        avg_RE += fabs(est_entro - real_entro) / real_entro / (double)REP_TIME;

        delete (sketch);
    }

    printf("\n");
    fprintf(output_file, "%s\n%.9f\n", sketch_name, avg_RE);
}

#endif