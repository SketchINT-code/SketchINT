#ifndef UTILS_H
#define UTILS_H
// general_includes
#include <cstdlib>
#include <cstring>
#include <vector>
#include <ctime>
#include <string>
#include <unordered_map>
#include <cstdio>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <chrono>
#include "BOBHash32.h"
using namespace std;

#define REP_TIME 10
#define INF 1000000000
#define MAX_FLOW_SIZE 1000000
//#define DEBUG_MODE
#define TUPLE_LEN 13
#define THRESHOLD_MULTI 0.0002
//#define MEMORY 600 * 1024

typedef std::chrono::high_resolution_clock::time_point TP;

inline TP now()
{
    return std::chrono::high_resolution_clock::now();
}

class FIVE_TUPLE
{
public:
    char key[TUPLE_LEN];
    FIVE_TUPLE() {}
    FIVE_TUPLE(const string &s)
    {
        for (int i = 0; i < TUPLE_LEN; i++)
        {
            key[i] = s[i];
        }
    }

    FIVE_TUPLE(const char *s)
    {
        for (int i = 0; i < TUPLE_LEN; i++)
        {
            key[i] = s[i];
        }
    }

    bool operator==(const FIVE_TUPLE &other) const
    {
        return memcmp(key, other.key, TUPLE_LEN) == 0;
    }
};

BOBHash32 general_hash = BOBHash32(rand() % MAX_PRIME32);

class Hash_Fun
{
public:
    size_t operator()(const FIVE_TUPLE &tuple) const
    {
        return general_hash.run(tuple.key, TUPLE_LEN);
    }
};

class FLOW_ITEM
{
public:
    FIVE_TUPLE key;
    int freq;

    bool operator<(const FLOW_ITEM &other) const
    {
        return freq < other.freq;
    }

    FLOW_ITEM() {}
    FLOW_ITEM(const FIVE_TUPLE &_key, int _freq)
    {
        key = _key;
        freq = _freq;
    }
};

bool cmp_item(const FLOW_ITEM &cur, const FLOW_ITEM &other)
{
    return other < cur;
}

typedef vector<FIVE_TUPLE> TRACE;

#endif