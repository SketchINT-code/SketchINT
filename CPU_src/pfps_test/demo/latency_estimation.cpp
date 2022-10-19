#include "../sketch/host_sketches.h"
#include "../task/latency_task.h"
#include <fstream>

const uint16_t SNO = 1;
const uint16_t ENO = 8;
const uint16_t SW_NUM = 10;

typedef vector<Packet> TRACE;
TRACE traces[ENO - SNO + 1];

void ReadInTraces(const char *filename, uint32_t no)
{
    ifstream inTrace(filename, ios::binary);

    Packet tmp_pkt;
    uint8_t INT_count;
    while (true)
    {
        inTrace.read(tmp_pkt.key, KEY_LEN);
        if (inTrace.gcount() < KEY_LEN)
            break;

        inTrace.read((char *)&INT_count, 1);
        tmp_pkt.sid.resize(INT_count), tmp_pkt.tstamp.resize(INT_count), tmp_pkt.deltat.resize(INT_count);
        for (uint8_t i = 0; i < INT_count; ++i)
        {
            inTrace.read((char *)&tmp_pkt.sid[i], 2);
            inTrace.read((char *)&tmp_pkt.tstamp[i], 8);
            inTrace.read((char *)&tmp_pkt.deltat[i], 4);
        }

        traces[no].push_back(tmp_pkt);
    }

    printf("Successfully read in %s, %ld packets\n", filename, traces[no].size());
}

double latencyTest(uint32_t mem_in_byte, int sketch_id)
{
    HostSketches sketch[8];
    unordered_map<string, int> realFreq[SW_NUM];
    unordered_map<string, int> realTotL[SW_NUM];
    int seed = rand() % 750;
    for (int no = 0; no < ENO - SNO + 1; ++no)
    {
        sketch[no].init(mem_in_byte, SW_NUM, LATENCY, seed, sketch_id);
        for (int i = 0; i < traces[no].size(); ++i)
        {
            sketch[no].insert(traces[no][i]);
            char l = no + '0';
            string key(traces[no][i].key, KEY_LEN);
            key = key + l;
            for (int j = 0; j < traces[no][i].sid.size(); ++j)
            {
                int rid = PH(traces[no][i].sid[j]);
                long long deltat = traces[no][i].deltat[j];
                realFreq[rid][key]++;
                realTotL[rid][key] += deltat;
            }
        }
    }

    return calcLatencyAcc(sketch, realFreq, realTotL, SW_NUM);
}

int main()
{
    char filename[80];
    for (uint32_t no = SNO; no <= ENO; ++no)
    {
        sprintf(filename, "/share/sketchINT_data/trace/DCTCP_5s/%u.bin", no);
        ReadInTraces(filename, no - SNO);
    }
    srand((unsigned)time(NULL));
    FILE *file_out = fopen("result_latency.txt", "w");
    int mem_unit = 1 << 19;
    printf("memory\tARE\n");
    for (uint32_t mem_in_byte = mem_unit; mem_in_byte <= mem_unit * 6; mem_in_byte += mem_unit)
    {
        for (int sketch_id = 0; sketch_id <= 2; sketch_id += 2)
        {
            double ARE = 0;
            for (int i = 0; i < 10; i++)
                ARE += latencyTest(mem_in_byte, sketch_id);
            ARE /= 10;
            cout << mem_in_byte / (1 << 10) << "KB    " << ARE << endl;
            char name[50];
            get_sketch_name(name, sketch_id);
            fprintf(file_out, "%s\n", name);
            fprintf(file_out, "%lf\n", ARE);
        }
    }
    fclose(file_out);
    return 0;
}
