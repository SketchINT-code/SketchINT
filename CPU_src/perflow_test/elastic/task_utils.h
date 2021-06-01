#ifndef TASK_UTILS_H
#define TASK_UTILS_H

#include "utils.h"
#include "elastic_sketch/ElasticSketch.h"
#include "data.h"
using namespace std;

/*
 * Simulating the process of inserting all the packets to the sketch for a specific measurement task.
 */
void insert_all_packets(ElasticSketch<MEMORY / 256, MEMORY>* sketch, TRACE* traces){
    int size = traces->size();
    for (int i = 0; i < size; i++){
        sketch->insert((uint8_t*)((*traces)[i].key), 1);
    }
}

/*
 * Display the name of sketch currently tested.
 */
void get_sketch_name(char* name, int sketch_id){
    sprintf(name, "Elastic");
}
#endif