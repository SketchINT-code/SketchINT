#ifndef DATA_H
#define DATA_H
#include "utils.h"
using namespace std;

unordered_map<string, int> table;
unordered_map<string, int>::iterator it;

BOBHash32 hash_data = BOBHash32(1);

void ReadInTraces(const char* filename, TRACE* &traces, FLOW_ITEM* &items, int& item_cnt){
	traces = new TRACE;
    FILE* fin = fopen(filename, "rb");

	FIVE_TUPLE tmp_five_tuple;
	char timestamp[8];
	traces->clear();
	while (fread(&tmp_five_tuple, 1, TUPLE_LEN, fin) == TUPLE_LEN)
		if (fread(timestamp, 1, 8, fin) == 8){
			int hash_res = hash_data.run(tmp_five_tuple.key, 13);
			FIVE_TUPLE tmp2;
			memcpy((void*)tmp2.key, &hash_res, 4);
			for (int i = 4; i < 13; i++){
				tmp2.key[i] = 0;
			}
			traces->push_back(tmp2);
		}
			
	fclose(fin);

	printf("Successfully read in %s, %ld packets\n\n", filename, traces->size());

	table.clear();

	for (int i = 0; i < traces->size(); i++){
		table[string((*traces)[i].key, TUPLE_LEN)] += 1;
	}

	items = new FLOW_ITEM[table.size()];
	item_cnt = 0;
	for (it = table.begin(); it != table.end(); it++){
		FIVE_TUPLE tmp(it->first);
		items[item_cnt++] = FLOW_ITEM(tmp, it->second);
	}

	sort(items, items + item_cnt, cmp_item);
}


#endif