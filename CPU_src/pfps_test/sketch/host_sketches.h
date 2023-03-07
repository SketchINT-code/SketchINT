#ifndef _HOST_SKETCHES_H
#define _HOST_SKETCHES_H

#include <string>
#include <algorithm>
#include "tower.h"
#include "heavyhitter.h"

#include <iostream>

using namespace std;

const uint32_t KEY_LEN = 16;
struct Packet
{
    char key[KEY_LEN];
    vector<uint16_t> sid;
    vector<uint64_t> tstamp;
    vector<uint32_t> deltat;
};

enum Task
{
    FREQUENCY,
    HEAVYHITTER,
    HEAVYCHANGE,
    LATENCY
};

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
        sprintf(name, "TowerSketchACU");
    }
    else if (sketch_id == 5)
    {
        sprintf(name, "CM(O)");
    }
    else if (sketch_id == 6)
    {
        sprintf(name, "CU(O)");
    }
}

TowerSketch *create_sketch(uint32_t mem_in_byte, uint32_t h, uint32_t seed_num, int sketch_id)
{
    TowerSketch *sketch = NULL;
    uint32_t min_w = mem_in_byte / (4 * 3);
    vector<uint32_t> w_tower = {min_w * 4, min_w * 2, min_w};
    vector<uint8_t> counter_len_freq_tower = {8, 16, 32};
    vector<uint32_t> w = {min_w, min_w, min_w};
    vector<uint8_t> counter_len_freq = {32, 32, 32};

    uint32_t min_w_o = mem_in_byte / (4+2+1);
    vector<uint32_t> w_tower_o = {min_w_o, min_w_o, min_w_o};

    switch (sketch_id)
    {
    case 0:
        sketch = new TowerSketch(w_tower, h, counter_len_freq_tower, seed_num); //TowerCM
        break;
    case 1:
        sketch = new TowerSketchCU(w_tower, h, counter_len_freq_tower, seed_num);   //TowerCU
        break;
    case 2:
        sketch = new TowerSketch(w, h, counter_len_freq, seed_num); //CM
        break;
    case 3:
        sketch = new TowerSketchCU(w, h, counter_len_freq, seed_num);   //CU
        break;
    case 4:
        sketch = new TowerSketchACU(w_tower, h, counter_len_freq_tower, seed_num);  //ACU
        break;
    case 5:
        sketch = new TowerSketch(w_tower_o, h, counter_len_freq_tower, seed_num);   //CM(O)
        break;
    case 6:
        sketch = new TowerSketchCU(w_tower_o, h, counter_len_freq_tower, seed_num);   //CU(O)
    default:
        break;
    }
    return sketch;
}


class HostSketches
{
private:
    HeavyHitter hh;

public:
    TowerSketch *freq, *delay;
    Task task;
    uint32_t ehn;
    HostSketches() {}
    HostSketches(uint32_t mem_in_byte, uint32_t h, Task in_task, uint32_t seed_num, int sketch_id)
    {
        init(mem_in_byte, h, in_task, seed_num, sketch_id);
    }

    void init(uint32_t mem_in_byte, uint32_t h, Task in_task, uint32_t seed_num, int sketch_id)
    {
        task = in_task;
        if (task == FREQUENCY)
        {
            freq = create_sketch(mem_in_byte, 1, seed_num, sketch_id);
        }
        if (task == HEAVYHITTER || task == HEAVYCHANGE)
        {
            freq = create_sketch(mem_in_byte, 1, seed_num, sketch_id);
        }
        if (task == LATENCY)
        {
            uint32_t mem_in_byte_freq = mem_in_byte * 1 / 4;   // new
            uint32_t mem_in_byte_lat = mem_in_byte * 3 / 4;      // new
            if (sketch_id == 0 || sketch_id == 1 || sketch_id == 4)
            {
                /*uint32_t min_w = mem_in_byte / (4 * 3 + 16 + 8 + 4);
                vector<uint32_t> w_tower = {min_w * 4, min_w * 2, min_w};
                vector<uint8_t> counter_len_freq_tower = {8, 16, 32};
                vector<uint8_t> counter_len_delay_tower = {32, 32, 32};
                freq = new TowerSketch(w_tower, h, counter_len_freq_tower, seed_num);
                delay = new TowerSketch(w_tower, h, counter_len_delay_tower, seed_num);*/

                uint32_t min_w_tower = mem_in_byte_freq / (3 * 4);  // new
                uint32_t min_w_cm = mem_in_byte_lat / (3 * 4);  // new

                vector<uint32_t> w_tower = {min_w_tower * 4, min_w_tower * 2, min_w_tower};
                vector<uint32_t> w_cm = {min_w_cm*4/3, min_w_cm*8/7, min_w_cm};

                vector<uint8_t> counter_len_freq_tower = {8, 16, 32};
                vector<uint8_t> counter_len_delay_tower = {24, 28, 32};
                if (sketch_id == 0){
                    freq = new TowerSketch(w_tower, h, counter_len_freq_tower, seed_num);
                    delay = new TowerSketch(w_cm, h, counter_len_delay_tower, seed_num);
                }
                else if (sketch_id == 1){
                    freq = new TowerSketchCU(w_tower, h, counter_len_freq_tower, seed_num);
                    delay = new TowerSketchCU(w_cm, h, counter_len_delay_tower, seed_num);
                }
                else{
                    freq = new TowerSketchACU(w_tower, h, counter_len_freq_tower, seed_num);
                    delay = new TowerSketchACU(w_cm, h, counter_len_delay_tower, seed_num);
                }
            }
            else if (sketch_id == 2 || sketch_id == 3)
            {
                uint32_t min_w_freq = mem_in_byte_freq / (4*3);
                uint32_t min_w_lat = mem_in_byte_lat / (4*3);

                //uint32_t min_w = mem_in_byte / (4 * 3 * 2);
                vector<uint32_t> w_freq = {min_w_freq, min_w_freq, min_w_freq};
                vector<uint32_t> w_lat = {min_w_lat, min_w_lat, min_w_lat};
                vector<uint8_t> counter_len = {32, 32, 32};
                if (sketch_id == 2){
                    freq = new TowerSketch(w_freq, h, counter_len, seed_num);
                    delay = new TowerSketch(w_lat, h, counter_len, seed_num);
                }
                else{
                    freq = new TowerSketchCU(w_freq, h, counter_len, seed_num);
                    delay = new TowerSketchCU(w_lat, h, counter_len, seed_num);
                }
            }
            else    //sketch_id == 5 or 6
            {
                uint32_t min_w_freq = mem_in_byte_freq / (4+2+1);
                uint32_t min_w_lat = mem_in_byte_lat / (10.5);

                vector<uint32_t> w_freq = {min_w_freq, min_w_freq, min_w_freq};
                vector<uint32_t> w_lat = {min_w_lat, min_w_lat, min_w_lat};

                vector<uint8_t> counter_len_freq = {8, 16, 32};
                vector<uint8_t> counter_len_lat = {24, 28, 32};
                if (sketch_id == 5){
                    freq = new TowerSketch(w_freq, h, counter_len_freq, seed_num);
                    delay = new TowerSketch(w_lat, h, counter_len_lat, seed_num);
                }
                else{
                    freq = new TowerSketchCU(w_freq, h, counter_len_freq, seed_num);
                    delay = new TowerSketchCU(w_lat, h, counter_len_lat, seed_num);
                }
            }
        }
    }

    void insert(const Packet &pkt)
    {
        if (task == FREQUENCY)
        {
            for (uint32_t i = 0; i < pkt.sid.size(); ++i)
                freq->insert(pkt.key, KEY_LEN, PH(pkt.sid[i]), 1);
        }
        if (task == HEAVYHITTER || task == HEAVYCHANGE)
        {
            freq->insert(pkt.key, KEY_LEN, 0, 1);
            uint32_t ret_freq = queryFreq(pkt.key, 0);
            hh.insert(pkt.key, KEY_LEN, ret_freq, ehn, pkt.sid);
        }
        if (task == LATENCY)
        {
            char key[KEY_LEN + 1];
            memcpy(key, pkt.key, KEY_LEN);

            freq->insert(key, KEY_LEN, 0, 1);   // new

            for (uint32_t i = 0; i < pkt.sid.size(); ++i)
            {
                key[KEY_LEN] = PH(pkt.sid[i]);
                //freq->insert(key, KEY_LEN + 1, 0, 1);     // new
                delay->insert(key, KEY_LEN + 1, 0, pkt.deltat[i]);
            }
        }
    }

    uint32_t queryFreq(const char *flow_id, uint32_t row_id)
    {
        char key[KEY_LEN + 1];
        memcpy(key, flow_id, KEY_LEN);
        key[KEY_LEN] = row_id;
        return freq->query(key, KEY_LEN, 0);
    }

    double queryAveDelay(const char *flow_id, uint32_t row_id)
    {
        char key[KEY_LEN + 1];
        memcpy(key, flow_id, KEY_LEN);
        key[KEY_LEN] = row_id;
        uint32_t ret_freq = freq->query(key, KEY_LEN, 0);   // new
        uint32_t ret_delay = delay->query(key, KEY_LEN + 1, 0); // new
        return ((double)ret_delay / ret_freq);
        /*vector<uint32_t> ret_freq = freq->queryAll(key, KEY_LEN + 1, 0);
        vector<uint32_t> ret_delay = delay->queryAll(key, KEY_LEN + 1, 0);
        vector<pair<uint32_t, double>> fd_pair;
        for (uint8_t i_d = 0; i_d < ret_freq.size(); ++i_d)
            if (ret_freq[i_d] != UINT32_MAX && ret_delay[i_d] != UINT32_MAX)
            {
                double ave_delay = (double)ret_delay[i_d] / ret_freq[i_d];
                fd_pair.push_back(make_pair(ret_freq[i_d], ave_delay));
            }
        sort(fd_pair.begin(), fd_pair.end());
        return fd_pair[0].second;*/
        
    }

    unordered_set<string> getHH(uint32_t row_id)
    {
        return hh.getHH(row_id);
    }

    unordered_set<string> getHC(HostSketches &B, uint32_t row_id)
    {
        auto ret_hh1 = getHH(row_id);
        auto ret_hh2 = B.getHH(row_id);
        ret_hh1.insert(ret_hh2.begin(), ret_hh2.end());
        unordered_set<string> ret_hc;
        for (auto key : ret_hh1)
        {
            int ret_freq1 = queryFreq(key.c_str(), 0);
            int ret_freq2 = B.queryFreq(key.c_str(), 0);
            int delta = abs(ret_freq1 - ret_freq2);
            if (delta >= hh_thres)
                ret_hc.insert(key);
        }
        return ret_hc;
    }
};

#endif
