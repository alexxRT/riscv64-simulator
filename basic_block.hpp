#ifndef BB_H
#define BB_H

#include "instruction.hpp"
#include "mask.hpp"
#include <cstdint>

const size_t BB_len = 31;
const size_t BB_arr_mask = (1<<17)-1; // TODO it's 90MB, maybe more?

class BasicBlock {
public:
    size_t addr;
    Instruction instrs[BB_len+1]; // +1 is reserved for empty
    uint8_t len;

    size_t construct(const instT *arr);
};

extern BasicBlock bbs_arr[BB_arr_mask+1];

#endif // BB_H
