#ifndef JIT_H
#define JIT_H

#include "instruction.hpp"
#include "mask.hpp"
#include <cstdint>
#include <llvm-16/llvm/IR/DerivedTypes.h>
#include <llvm-16/llvm/IR/Type.h>

const size_t JIT_LEN = 31; // 1 for cosim, 31 is good enough for run
const size_t Jit_arr_mask = (1<<17)-1; // TODO it's 90MB, maybe more?

class RVJitBlock {
public:
    typedef void (*BBFType)(uint64_t *regs, uint8_t *mem, size_t *pc, bool *done);
    size_t addr;
    BBFType jitted;
    Instruction instrs[JIT_LEN+1]; // +1 is reserved for empty
    uint8_t len;

    static void init();
    size_t do_jit(const instT *arr);
};

#endif // JIT_H
