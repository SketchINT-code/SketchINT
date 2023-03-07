#ifndef TASK_UTILS_H
#define TASK_UTILS_H

#include "utils.h"
#include "tower.h"
#include "data.h"
using namespace std;

/*
 * Simulating the process of inserting all the packets to the sketch for a specific measurement task.
 */
void insert_all_packets(TowerSketch *sketch, TRACE *traces)
{
    int size = traces->size();
    for (int i = 0; i < size; i++)
    {
        sketch->insert((*traces)[i].key, TUPLE_LEN, 0, 1);
    }
}

/*
 * Build the sketch.
 */
TowerSketch *create_sketch(int memory, int opt, int threshold, int sketch_id)
{
    TowerSketch *sketch;
    if (sketch_id == 0)
    {
        vector<uint32_t> width;
        for (int i = 0; i < width_mul_tower.size(); i++)
        {
            width.push_back(memory * width_mul_tower[i]);
        }
        sketch = new TowerSketch(width, 1, cs_tower, opt, threshold);
    }

    else if (sketch_id == 1)
    {
        vector<uint32_t> width;
        for (int i = 0; i < width_mul_tower.size(); i++)
        {
            width.push_back(memory * width_mul_tower[i]);
        }
        sketch = new TowerSketchCU(width, 1, cs_tower, opt, threshold);
    }

    else if (sketch_id == 2)
    {
        vector<uint32_t> width;
        for (int i = 0; i < width_mul_cm.size(); i++)
        {
            width.push_back(memory * width_mul_cm[i]);
        }
        sketch = new TowerSketch(width, 1, cs_cm, opt, threshold);
    }

    else if (sketch_id == 3)
    {
        vector<uint32_t> width;
        for (int i = 0; i < width_mul_cm.size(); i++)
        {
            width.push_back(memory * width_mul_cm[i]);
        }
        sketch = new TowerSketchCU(width, 1, cs_cm, opt, threshold);
    }
    else if (sketch_id == 4)
    {
        vector<uint32_t> width;
        for (int i = 0; i < width_mul_tower.size(); i++)
        {
            width.push_back(memory * width_mul_tower[i]);
        }
        sketch = new TowerSketchHalfCU0(width, 1, cs_tower, opt, threshold);
    }
    else if (sketch_id == 5)
    {
        vector<uint32_t> width;
        for (int i = 0; i < width_mul_tower.size(); i++)
        {
            width.push_back(memory * width_mul_tower[i]);
        }
        sketch = new TowerSketchHalfCU1(width, 1, cs_tower, opt, threshold);
    }
    else if (sketch_id == 6)
    {
        vector<uint32_t> width;
        for (int i = 0; i < width_mul_cm_tower.size(); i++)
        {
            width.push_back(memory * width_mul_cm_tower[i]);
        }
        sketch = new CMTowerSketch(width, 1, cs_tower, opt, threshold);
    }
    else
    {
        vector<uint32_t> width;
        for (int i = 0; i < width_mul_cm_tower.size(); i++)
        {
            width.push_back(memory * width_mul_cm_tower[i]);
        }
        sketch = new CUTowerSketch(width, 1, cs_tower, opt, threshold);
    }

    return sketch;
}

/*
 * sketch_id = 0: TowerSketch
 * sketch_id = 1: TowerSketchCU
 * sketch_id = 2: CM
 * sketch_id = 3: CU
 */
void get_sketch_name(char *name, int sketch_id)
{
    if (sketch_id == 0)
    {
        sprintf(name, "TowerSketch");
    }
    else if (sketch_id == 1)
    {
        sprintf(name, "TowerSketchCU");
    }
    else if (sketch_id == 2)
    {
        sprintf(name, "CM");
    }
    else if (sketch_id == 3)
    {
        sprintf(name, "CU");
    }
    else if (sketch_id == 4)
    {
        sprintf(name, "HalfCU0");
    }
    else if (sketch_id == 5)
    {
        sprintf(name, "HalfCU1");
    }
    else if (sketch_id == 6)
    {
        sprintf(name, "CMTower");
    }
    else
    {
        sprintf(name, "CUTower");
    }
}

#endif