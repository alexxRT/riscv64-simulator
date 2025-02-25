#ifndef MASK_H
#define MASK_H

#include "instruction.hpp"
#include <cstdint>
#include <llvm-16/llvm/IR/Function.h>
#include <llvm-16/llvm/IR/IRBuilder.h>
#include <llvm-16/llvm/IR/Value.h>


#define FP_HASH(x) ((x) & INSN_FIELD_OPCODE) \
                | (((x) & INSN_FIELD_FUNCT3) >> 5) \
                | (((x) & INSN_FIELD_FUNCT7) >> 15)

extern uint32_t mask[128];

struct Decode {
    Instruction::executorT exec;
    void (*decod)(Instruction&, instT);
    bool linear;
    void (*jit)(Instruction &, llvm::IRBuilder<> &, llvm::LLVMContext &, llvm::Value* , 
                llvm::Value *, llvm::Value *, llvm::Function *fn, llvm::Value *done);
};

extern Decode decoders[(1<<18)-1];

#endif // MASK_H
