#ifndef BB_H
#define BB_H

#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetSelect.h"

#include "instruction.hpp"
#include "mask.hpp"
#include <cstdint>
#include <llvm-16/llvm/IR/DerivedTypes.h>
#include <llvm-16/llvm/IR/Type.h>

const size_t BB_len = 31;
const size_t BB_arr_mask = (1<<17)-1; // TODO it's 90MB, maybe more?

class RVBasicBlock {
public:
    typedef void (*BBFType)(uint64_t *regs, uint8_t *mem, size_t *pc);
    size_t addr;
    BBFType jitted;
    Instruction instrs[BB_len+1]; // +1 is reserved for empty
    uint8_t len;

    static void init();
    size_t construct(const instT *arr);
    size_t do_jit(const instT *arr);

//    static llvm::Type *ptrType;
};

extern RVBasicBlock bbs_arr[BB_arr_mask+1];

#endif // BB_H
