#include "../sketch/host_sketches.h"
#include "../task/heavyhitter_task.h"
#include <fstream>

const uint16_t SNO = 1;
const uint16_t ENO = 8;
const uint16_t SW_NUM = 10;

typedef vector<Packet> TRACE;
TRACE traces[ENO - SNO + 1];
const uint32_t FULL_KEY_LEN = 16;
void ReadInTraces(const char* filename, uint32_t no)
{
	ifstream inTrace(filename, ios::binary);

	BOBHash32 bobhash(200); Packet tmp_pkt; uint8_t INT_count;
	char key[FULL_KEY_LEN];
	while (true)
	{
		inTrace.read(key, FULL_KEY_LEN);
		if (inTrace.gcount() < FULL_KEY_LEN) break;

		uint32_t hashed_key = bobhash.run((const char*)key, FULL_KEY_LEN);
		memcpy(tmp_pkt.key, &hashed_key, KEY_LEN);
		inTrace.read((char*)&INT_count, 1);
		tmp_pkt.sid.resize(INT_count), tmp_pkt.tstamp.resize(INT_count), tmp_pkt.deltat.resize(INT_count);
		for (uint8_t i = 0; i < INT_count; ++i)
		{
			inTrace.read((char*)&tmp_pkt.sid[i], 2);
			inTrace.read((char*)&tmp_pkt.tstamp[i], 8);
			inTrace.read((char*)&tmp_pkt.deltat[i], 4);
		}

		traces[no].push_back(tmp_pkt);
	}

	printf("Successfully read in %s, %ld packets\n", filename, traces[no].size());
}

double HHTest(uint32_t mem_in_byte)
{
	int seed = rand() % 750;
	HostSketches sketch[ENO - SNO + 1];
	unordered_map<string, int> realFreq[SW_NUM];

	for (int no = 0; no < ENO - SNO + 1; ++no)
	{
		sketch[no].init(mem_in_byte, SW_NUM, HEAVYHITTER, seed);
		sketch[no].ehn = no;
		for (int i = 0; i < traces[no].size(); ++i)
		{
			sketch[no].insert(traces[no][i]);
			char l = no + '0';
			string key(traces[no][i].key, KEY_LEN);
			key = key + l;
			for (int j = 0; j < traces[no][i].sid.size(); ++j)
			{
				int rid = PH(traces[no][i].sid[j]);
				realFreq[rid][key]++;
			}
		}
	}

	return calcHHAcc(sketch, realFreq, SW_NUM, ENO - SNO + 1);

}

int main()
{
	char filename[80];
	for (uint32_t no = SNO; no <= ENO; ++no)
	{
		sprintf(filename, "dataset directory/%d.bin", no);
		//sprintf(filename, "../../%d.bin", no);
		ReadInTraces(filename, no - SNO);
	}
	srand((unsigned)time(NULL));
	printf("memory\tF1-score\n");
	for (uint32_t mem_in_byte = (10 << 10); mem_in_byte <= (60 << 10); mem_in_byte += (10 << 10))
	{
		double F1 = 0;
		for (int i = 0; i < 10; i++)
			F1 += HHTest(mem_in_byte);
		F1 /= 10;
		cout << mem_in_byte / (1 << 10) << "KB\t" << F1 << endl;
	}
	return 0;
}
