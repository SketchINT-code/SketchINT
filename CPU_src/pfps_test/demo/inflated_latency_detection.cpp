#include "../sketch/host_sketches.h"
#include "../task/latency_task.h"
#include <fstream>

const uint16_t SNO = 1;
const uint16_t ENO = 8;
const uint16_t SW_NUM = 10;
const double BURST_THRES = 5;

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

double latencyBurstTest(uint32_t mem_in_byte, int sketch_id)
{
    unordered_map<string, int> realFreq[SW_NUM];
    unordered_map<string, int> realTotL[SW_NUM];
    unordered_set<string> estBurst[SW_NUM];
    unordered_set<string> realBurst[SW_NUM];
    int seed = rand() % 750;
    for (int no = 0; no < ENO - SNO + 1; ++no)
    {
        HostSketches sketch(mem_in_byte, SW_NUM, LATENCY, seed, sketch_id);
        for (int i = 0; i < traces[no].size(); ++i)
        {
            string key(traces[no][i].key, KEY_LEN);
            for (int j = 0; j < traces[no][i].sid.size(); ++j)
            {
                int rid = PH(traces[no][i].sid[j]);
                long long deltat = traces[no][i].deltat[j];

                if (sketch.queryFreq(traces[no][i].key, rid) > 0)
                {
                    double est_aveL = sketch.queryAveDelay(traces[no][i].key, rid);
                    if (deltat > est_aveL * BURST_THRES)
                        estBurst[rid].insert(key + to_string(no));
                }

                if (realFreq[rid].find(key) != realFreq[rid].end())
                {
                    double real_aveL = (double)realTotL[rid][key] / realFreq[rid][key];
                    if (deltat > real_aveL * BURST_THRES)
                        realBurst[rid].insert(key + to_string(no));
                }
            }

            sketch.insert(traces[no][i]);
            for (int j = 0; j < traces[no][i].sid.size(); ++j)
            {
                int rid = PH(traces[no][i].sid[j]);
                long long deltat = traces[no][i].deltat[j];
                realFreq[rid][key]++;
                realTotL[rid][key] += deltat;
            }
        }
    }

    // printf("%dKB\t", mem_in_byte / (1 << 10));
    // cout<<calcBurstAcc(estBurst, realBurst, SW_NUM)<<endl;
    return calcBurstAcc(estBurst, realBurst, SW_NUM);
    // printf("\n");
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
    FILE *file_out = fopen("result_inflated_latency.txt", "w");
    printf("memory\tPR\tRR\tF1-score\n");
    for (uint32_t mem_in_byte = (100 << 10); mem_in_byte <= (600 << 10); mem_in_byte += (100 << 10))
    {
        for (int sketch_id = 0; sketch_id <= 2; sketch_id += 2)
        {
            double F1 = 0;
            for (int i = 0; i < 10; i++)
                F1 += latencyBurstTest(mem_in_byte, sketch_id);
            F1 /= 10;
            cout << mem_in_byte / (1 << 10) << "KB    " << F1 << endl;
            char name[50];
            get_sketch_name(name, sketch_id);
            fprintf(file_out, "%s\n", name);
            fprintf(file_out, "%lf\n", F1);
        }
    }
    fclose(file_out);

    return 0;
}
