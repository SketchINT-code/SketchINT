#include "../sketch/host_sketches.h"
#include "../task/heavychange_task.h"
#include <fstream>

const uint16_t SNO = 1;
const uint16_t ENO = 8;
const uint16_t SW_NUM = 10;

typedef vector<Packet> TRACE;
TRACE traces[2][ENO - SNO + 1];

void ReadInTraces(const char* filename, uint32_t no, uint32_t window_no)
{
	ifstream inTrace(filename, ios::binary);

	Packet tmp_pkt; uint8_t INT_count;
	while (true)
	{
		inTrace.read(tmp_pkt.key, KEY_LEN);
		if (inTrace.gcount() < KEY_LEN) break;

		inTrace.read((char*)&INT_count, 1);
		tmp_pkt.sid.resize(INT_count), tmp_pkt.tstamp.resize(INT_count), tmp_pkt.deltat.resize(INT_count);
		for (uint8_t i = 0; i < INT_count; ++i)
		{
			inTrace.read((char*)&tmp_pkt.sid[i], 2);
			inTrace.read((char*)&tmp_pkt.tstamp[i], 8);
			inTrace.read((char*)&tmp_pkt.deltat[i], 4);
		}

		traces[window_no][no].push_back(tmp_pkt);
	}

	printf("Successfully read in %s, %ld packets\n", filename, traces[window_no][no].size());
}

double HCTest(uint32_t mem_in_byte)
{
	int seed = rand() % 750;
	HostSketches sketch[2][ENO - SNO + 1];
	unordered_map<string, int> realFreq[SW_NUM];

	for (int no = 0; no < ENO - SNO + 1; ++no)
	{
		for (int window_no = 0; window_no < 2; ++window_no)
		{
			sketch[window_no][no].init(mem_in_byte, SW_NUM, HEAVYCHANGE, seed);
			sketch[window_no][no].ehn = no;
			for (int i = 0; i < traces[window_no][no].size(); ++i)
			{
				sketch[window_no][no].insert(traces[window_no][no][i]);
				char l = no + '0';
				string key(traces[window_no][no][i].key, KEY_LEN);
				key = key + l;
				for (int j = 0; j < traces[window_no][no][i].sid.size(); ++j)
				{
					int rid = PH(traces[window_no][no][i].sid[j]);
					realFreq[rid][key]+=(window_no==1?1:-1);
				}
			}
		}
	}
	return calcHCAcc(sketch, realFreq, SW_NUM, ENO - SNO + 1);
}

int main()
{
	char filename[80];
	for (uint32_t no = SNO; no <= ENO; ++no)
	{
		sprintf(filename, "dataset directory/window0/%d.bin", no);
		ReadInTraces(filename, no - SNO, 0);
	}
	for (uint32_t no = SNO; no <= ENO; ++no)
	{
		sprintf(filename, "dataset directory/window1/%d.bin", no);
		ReadInTraces(filename, no - SNO, 1);
	}

	srand((unsigned)time(NULL));
	printf("memory\tF1-score\n");
	for (uint32_t mem_in_byte = (10 << 10); mem_in_byte <= (60 << 10); mem_in_byte += (10 << 10))
	{
		double F1 = 0;
		for (int i = 0; i < 10; i++)
			F1 += HCTest(mem_in_byte);
		F1 /= 10;
		cout << mem_in_byte / (1 << 10) << "KB\t" << F1 << endl;
	}
	return 0;
}
