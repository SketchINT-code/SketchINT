#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include "utils.h"
#include "tower.h"
#include "data.h"
#include "task_utils.h"
using namespace std;

/*
 * To test the performance (WMRE) of tower+CM/tower+CU/CM/CU over measuring the distribution of a stream of flows.
 */
void distribution_test(TRACE *traces, FLOW_ITEM *items, int item_cnt, int memory, int sketch_id, int opt, FILE *output_file)
{
    double avg_WMRE = 0;
    char sketch_name[100];
    get_sketch_name(sketch_name, sketch_id);
    printf("%s\n", sketch_name);

    unordered_map<int, double> real_dist;
    unordered_map<int, double> uni_dist;

    int max_f = 0;

    for (int rep = 0; rep < REP_TIME; rep++)
    {
        printf("%d\n", rep);
        TowerSketch *sketch;
        sketch = create_sketch(memory, opt, 0, sketch_id);

        insert_all_packets(sketch, traces);




        for (int i = 0; i < item_cnt; i++){
            real_dist[items[i].freq]++;
            max_f = max(max_f, items[i].freq);

            int val_uni = sketch->query(items[i].key.key, TUPLE_LEN, 0);
            uni_dist[val_uni]++;
            
        }

        double zi_uni = 0, mu_uni = 0;

        for (int i=1; i<=max_f; i++){
            zi_uni += fabs(real_dist[i] - uni_dist[i]);   //fen zi

            mu_uni += (real_dist[i] + uni_dist[i]) / 2.0;   //fen mu
        }

        double WMRE = zi_uni/mu_uni;

        avg_WMRE += WMRE / (double)REP_TIME;










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

        double WMRE = 0;

        int cur_sum = 0;
        for (int i = 0; i <= 2 * items[0].freq; i++)
        {
            WMRE += fabs((double)real_distri[i] - (double)result_distri[i]);
            cur_sum += result_distri[i];
        }*/



        /*
        FILE *tmp = fopen("result_dis.txt", "a");
        fprintf(tmp, "%s\n", sketch_name);
        for (int i = 0; i <= 2 * items[0].freq; i++)
        {
            fprintf(tmp, "%lf ", result_distri[i]);
        }
        fprintf(tmp, "\n");
        for (int i = 0; i <= 2 * items[0].freq; i++)
        {
            fprintf(tmp, "%d ", real_distri[i]);
        }
        fprintf(tmp, "\n");
        fclose(tmp);
        */

        // printf("%f\n", WMRE);
        /*
        WMRE /= (double)(cur_sum + item_cnt) / 2.0;

        avg_WMRE += WMRE / (double)REP_TIME;*/

        delete (sketch);
        //delete[] result_distri;
        //delete[] real_distri;
    }

    printf("\n");
    fprintf(output_file, "%s\n%.9f\n", sketch_name, avg_WMRE);
}

#endif