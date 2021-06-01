#include "data.h"
#include "utils.h"
#include "BOBHash32.h"
#include "freq_test.h"
#include "heavy_hitter.h"
#include "heavy_change.h"
#include "distribution.h"
#include "entropy.h"
#include "cardinality.h"
#include <ctime>
using namespace std;

void main_task(TRACE* traces, FLOW_ITEM* items, int item_cnt, int task_id, int sketch_id, FILE* output_file, char* extra_file_name){
    int size = traces->size();

    switch (task_id){
        case 0:
            frequency_test(traces, items, item_cnt, sketch_id, 0, output_file);
            break;
        case 1:
            heavy_hitter_test(traces, items, item_cnt, sketch_id, 1, size * THRESHOLD_MULTI, output_file);
            break;
        case 2:
            heavy_change_test(traces, items, item_cnt, sketch_id, 1, size * THRESHOLD_MULTI * 0.5, output_file, extra_file_name);
            break;
        case 3:
            distribution_test(traces, items, item_cnt, sketch_id, 2, output_file);
            break;
        case 4:
            entropy_test(traces, items, item_cnt, sketch_id, 2, output_file);
            break;
        case 5:
            cardinality_test(traces, items, item_cnt, sketch_id, 0, output_file);
            break;
        default:
            break;
    }
}

int main(){
    srand(time(NULL));

    FILE* test_args = fopen("config.txt", "r");
    char trace_file_name[100], extra_trace_file_name[100], output_file_name[100];
    FILE* output_file;

    fscanf(test_args, "%s%s", trace_file_name, output_file_name);
    output_file = fopen(output_file_name, "a+");

    TRACE* traces;
    FLOW_ITEM* items;
    int item_cnt;

    ReadInTraces(trace_file_name, traces, items, item_cnt);

    int task_cnt;
    int memory, task_id; //memory in KB
    fscanf(test_args, "%d", &task_cnt);

    while(task_cnt--){
        fscanf(test_args, "%d", &task_id);
        extra_trace_file_name[0] = 0;

        if (task_id == 2){
            fscanf(test_args, "%s", extra_trace_file_name);
        }

        main_task(traces, items, item_cnt, task_id, 0, output_file, extra_trace_file_name);
            
        fflush(output_file);
    }

    delete(traces);
    delete[] items;
    fclose(output_file);

    return 0;
}