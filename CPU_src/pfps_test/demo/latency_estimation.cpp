#include "../sketch/host_sketches.h"
#include "../task/latency_task.h"
#include <fstream>
#include <thread>

const uint16_t SNO = 1;
const uint16_t ENO = 8;
const uint16_t SW_NUM = 10;
vector<thread> threads;
typedef vector<Packet> TRACE;
TRACE traces[ENO - SNO + 1];
unordered_map<string, int> realFreq[SW_NUM];          
unordered_map<string, int> realTotL[SW_NUM]; 



bool vector_comp(vector<uint16_t> v1, vector<uint16_t> v2){
    if (v1.size()!=v2.size()){
        return false;
    }
    int len=v1.size();
    for (int i=0; i<len; i++){
        if (v1[i]!=v2[i])
            return false;
    }
    return true;
}


void ReadInTraces(const char *filename, uint32_t no)
{
    ifstream inTrace(filename, ios::binary);

    Packet tmp_pkt;
    uint8_t INT_count;

    unordered_map<string, vector<uint16_t>> mp;

    uint32_t false_cnt=0, right_cnt=0;

    while (true)
    {
        inTrace.read(tmp_pkt.key, KEY_LEN);
        if (inTrace.gcount() < KEY_LEN)
            break;
        
        string kee(tmp_pkt.key, KEY_LEN);
        

        inTrace.read((char *)&INT_count, 1);
        tmp_pkt.sid.resize(INT_count), tmp_pkt.tstamp.resize(INT_count), tmp_pkt.deltat.resize(INT_count);
        for (uint8_t i = 0; i < INT_count; ++i)
        {
            inTrace.read((char *)&tmp_pkt.sid[i], 2);
            inTrace.read((char *)&tmp_pkt.tstamp[i], 8);
            inTrace.read((char *)&tmp_pkt.deltat[i], 4);
        }
        if (!vector_comp(mp[kee], tmp_pkt.sid)){
            false_cnt++;
        }
        else{
            right_cnt++;
        }     
        mp[kee]=tmp_pkt.sid; 

        traces[no].push_back(tmp_pkt);
    }

    //printf("Successfully read in %s, %ld packets\n", filename, traces[no].size());
    printf("Successfully read in %s, %ld packets and %ld falsecmp\n", filename, traces[no].size(), false_cnt);
}


void getgroundtruth()
{
    for (int no = 0; no < ENO - SNO + 1; ++no)  //tackle trace[no]  
    {
        for (int i = 0; i < traces[no].size(); ++i)
        {
            char l = no + '0';
            string key(traces[no][i].key, KEY_LEN);
            key = key + l;
            for (int j = 0; j < traces[no][i].sid.size(); ++j)  
            {
                int rid = PH(traces[no][i].sid[j]); 
                long long deltat = traces[no][i].deltat[j];
                realFreq[rid][key]++;           // here, key = flow_ID + trace_ID
                realTotL[rid][key] += deltat;
            } 
        
        }
    }
    return;
}

double latencyTest(uint32_t mem_in_byte, int sketch_id, int seed)
{
    HostSketches sketch[8]; 
    
    //int seed = rand() % 750;
    for (int no = 0; no < ENO - SNO + 1; ++no)  //tackle trace[no]  
    {
        sketch[no].init(mem_in_byte, 1, LATENCY, seed, sketch_id); //build SW_NUM(10) sketches *** some typos
        for (int i = 0; i < traces[no].size(); ++i)
        {
            sketch[no].insert(traces[no][i]);
	    }
    }
    double res = calcLatencyAcc(sketch, realFreq, realTotL, SW_NUM);
    for (int i = 0; i < 8; i++)
    {
        sketch[i].freq->clear();
        sketch[i].delay->clear();
    }
    return res;
}
void test1ing (string nm, int offset, int base, int maxi)
{
	string fname = "result_latency_new_";
	fname += nm;
    FILE *file_out = fopen(fname.c_str(), "w");
    printf("memory\tARE\n");
    for (uint32_t mem_in_byte = base; mem_in_byte <= maxi; mem_in_byte += offset)
    {
        int seed[50];
        for (int i=0; i<50; i++){
            seed[i]=rand() % 750;
        }
        for (int sketch_id = 0; sketch_id <= 6; sketch_id += 1)
        {
            double ARE = 0;
            //for (int i = 0; i < 10; i++)
            for (int i = 0; i < 50; i++) 
                ARE += latencyTest(mem_in_byte, sketch_id, seed[i]);
            ARE /= 50;
            //ARE /= 1;
            cout << mem_in_byte / (1 << 10) << "KB    " << ARE << endl;
            char name[50];
            get_sketch_name(name, sketch_id);
            fprintf(file_out, "%s\n", name);
            fprintf(file_out, "%lf\n", ARE);
        }
    }
    fclose(file_out);
}

int main()
{
    char filename[80];
    for (uint32_t no = SNO; no <= ENO; ++no)
    {
        sprintf(filename, "/share/sketchINT_data/trace/DCTCP_5s/%u.bin", no);
        ReadInTraces(filename, no - SNO);
    }
    getgroundtruth();
    srand((unsigned)time(NULL));
    int thread_num = 5;
    int mem_unit = 183705;
    int bas = mem_unit * 10;
    //1837056 1897472
    int maxi = 50 * mem_unit;
    for (int i = 0; i < thread_num ; i++)
    {
        string name = "s_NB";
        name[0] = 'a' + i;
        threads.push_back(thread(test1ing, name, thread_num* mem_unit, (i)*mem_unit+bas, maxi));
    }
    for (int i = 0; i < thread_num ; i++)
    {
        threads[i].join();
    }

    return 0;
}

