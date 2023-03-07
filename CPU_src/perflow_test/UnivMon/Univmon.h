#ifndef UNIVMON_H
#define UNIVMON_H

#include "CountHeap.h"

using namespace std;

class UnivMon{
private:
    //const uint32_t LEVEL = 6;
    const uint32_t LEVEL = 1;
    CountHeap** sketches;


public:
    UnivMon(uint32_t _MEMORY){
        sketches = new CountHeap* [LEVEL];

        for(uint32_t i = 0;i < LEVEL; ++i){
            //sketches[i] = new CountHeap(_MEMORY >> (i + 1));
            sketches[i] = new CountHeap(_MEMORY);
        }
    }

    ~UnivMon(){
        for(uint32_t i = 0;i < LEVEL;++i){
            delete sketches[i];
        }
        delete [] sketches;
    }

    void insert(const char * str, uint16_t key_len){
        uint32_t pos = MurmurHash3_x86_32(str, key_len, 199);
        sketches[0]->insert(str, key_len);
        for(uint32_t i = 1; i < LEVEL; ++i){
            if(pos & 1)
                sketches[i]->insert(str, key_len);
            else
                break;
            pos >>= 1;
        }
    }

    int query(const char * str, uint16_t key_len){
        uint32_t pos = MurmurHash3_x86_32(str, key_len, 199);
        int32_t level;

        for(level = 1; level < LEVEL; ++level){
            if(pos & 1)
                pos >>= 1;
            else
                break;
        }

        int ret = sketches[level - 1]->query(str, key_len);
        for(int32_t i = level - 2;i >= 0;--i){
            ret = 2 * ret - sketches[i]->query(str, key_len);
        }

        return ret;
    }



};

#endif