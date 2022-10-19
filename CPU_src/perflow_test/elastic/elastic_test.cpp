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

template <int memory>
void main_task(TRACE *traces, FLOW_ITEM *items, int item_cnt, int task_id, int sketch_id, FILE *output_file, char *extra_file_name)
{
    int size = traces->size();

    switch (task_id)
    {
    case 0:
        frequency_test<memory>(traces, items, item_cnt, sketch_id, 0, output_file);
        break;
    case 1:
        heavy_hitter_test<memory>(traces, items, item_cnt, sketch_id, 1, size * THRESHOLD_MULTI, output_file);
        break;
    case 2:
        heavy_change_test<memory>(traces, items, item_cnt, sketch_id, 1, size * THRESHOLD_MULTI * 0.5, output_file, extra_file_name);
        break;
    case 3:
        distribution_test<memory>(traces, items, item_cnt, sketch_id, 2, output_file);
        break;
    case 4:
        entropy_test<memory>(traces, items, item_cnt, sketch_id, 2, output_file);
        break;
    case 5:
        cardinality_test<memory>(traces, items, item_cnt, sketch_id, 0, output_file);
        break;
    default:
        break;
    }
}

int main()
{
    srand(time(NULL));

    FILE *test_args = fopen("config.txt", "r");
    char trace_file_name[100], extra_trace_file_name[100], output_file_name[100];
    FILE *output_file;

    fscanf(test_args, "%s%s", trace_file_name, output_file_name);
    output_file = fopen(output_file_name, "w");

    TRACE *traces;
    FLOW_ITEM *items;
    int item_cnt;

    ReadInTraces(trace_file_name, traces, items, item_cnt);

    int task_cnt;
    int memory, task_id; // memory in KB
    fscanf(test_args, "%d", &task_cnt);

    while (task_cnt--)
    {
        fscanf(test_args, "%d%d", &memory, &task_id);
        extra_trace_file_name[0] = 0;

        if (task_id == 2)
        {
            fscanf(test_args, "%s", extra_trace_file_name);
        }

        switch (memory)
        {
        case 150:
            main_task<150 * 1024>(traces, items, item_cnt, task_id, 0, output_file, extra_trace_file_name);
            break;
        case 300:
            main_task<300 * 1024>(traces, items, item_cnt, task_id, 0, output_file, extra_trace_file_name);
            break;
        case 450:
            main_task<450 * 1024>(traces, items, item_cnt, task_id, 0, output_file, extra_trace_file_name);
            break;
        case 600:
            main_task<600 * 1024>(traces, items, item_cnt, task_id, 0, output_file, extra_trace_file_name);
            break;
        case 750:
            main_task<750 * 1024>(traces, items, item_cnt, task_id, 0, output_file, extra_trace_file_name);
            break;
        case 900:
            main_task<900 * 1024>(traces, items, item_cnt, task_id, 0, output_file, extra_trace_file_name);
            break;
        case 50:
            main_task<50 * 1024>(traces, items, item_cnt, task_id, 0, output_file, extra_trace_file_name);
            break;
        case 100:
            main_task<100 * 1024>(traces, items, item_cnt, task_id, 0, output_file, extra_trace_file_name);
            break;
        case 200:
            main_task<200 * 1024>(traces, items, item_cnt, task_id, 0, output_file, extra_trace_file_name);
            break;
        case 250:
            main_task<250 * 1024>(traces, items, item_cnt, task_id, 0, output_file, extra_trace_file_name);
            break;
        default:
            main_task<600 * 1024>(traces, items, item_cnt, task_id, 0, output_file, extra_trace_file_name);
            break;
        }

        fflush(output_file);
    }

    delete (traces);
    delete[] items;
    fclose(output_file);

    return 0;
}