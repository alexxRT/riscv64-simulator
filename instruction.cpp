#include "instruction.hpp"
#include "hart.hpp"
#include <llvm-16/llvm/IR/Function.h>
#include <llvm-16/llvm/IR/IRBuilder.h>

void decode_instruction_R(Instruction &ins, instT code) {
    DEB("decs R");
    RS1_INIT RS2_INIT RD_INIT
}

void decode_instruction_I(Instruction &ins, instT code) {
    DEB("decs I");
    RS1_INIT RD_INIT SIGN_INIT
    ins.imm = ((code&INSN_FIELD_IMM12) >> 20) | (sign * SIGNN(11));
}

void decode_instruction_S(Instruction &ins, instT code) {
    DEB("decs S");
    RS1_INIT RS2_INIT SIGN_INIT
    ins.imm = ((code&INSN_FIELD_IMM12HI) >> 20)
        | ((code&INSN_FIELD_IMM12LO) >> 7)
        | (sign * SIGNN(12));
}

void decode_instruction_B(Instruction &ins, instT code) {
    DEB("decs B");
    RS1_INIT RS2_INIT SIGN_INIT
    ins.imm = ((code & (1<<7)) << 4)
        | ((code & 0xfe000000) >> 20)
        | ((code & 0xf00) >> 7)
        | (sign * SIGNN(12));
}

void decode_instruction_U(Instruction &ins, instT code) {
    DEB("decs U");
    RD_INIT SIGN_INIT
    ins.imm = (code & INSN_FIELD_IMM20) | (sign * SIGNN(31));
}

void decode_instruction_J(Instruction &ins, instT code) {
    DEB("decs J");
    SIGN_INIT
    ins.imm = ((code & INSN_FIELD_ZIMM10) >> 20)
        | ((code & (1<<20)) >> 9)
        | ((code & (0xff000)))
        | (sign * SIGNN(20));
}

namespace Executors {

// for basic blocks
void empty_executor(Hart *hart, const Instruction &instr) {
    return;
}

#define EXEC_RET_true (&instr+1)->execute(heart, *(&instr+1))
#define EXEC_RET_false 

#define _INSTR_(name, type, code, linear, jit) \
__attribute__((noinline)) void exec_##name(Hart *heart, const Instruction &instr) { \
    DEB("exec "#name); \
    {int new_pc = heart->pc+4; code; heart->pc = new_pc;} \
    EXEC_RET_##linear; }

#include "instrs.h"
#undef _INSTR_

};

namespace Jiters {
void empty_jiter(Instruction &instr, llvm::IRBuilder<> &builder) {
    builder.CreateRetVoid();
} // for basic blocks

using llvm::Type;
using llvm::Value;
using llvm::BasicBlock;
using llvm::Function;

#define _INSTR_(name, type, code, linear, jit) \
void jit_##name(Instruction &instr, llvm::IRBuilder<> &builder, llvm::LLVMContext &ctx, Value* regs, Value *mem, Value *pc, Value *new_pc, Function *fn) { \
    builder.CreateStore(builder.CreateAdd(builder.CreateLoad(Type::getInt64Ty(ctx), pc), builder.getInt64(4)), new_pc); \
    jit  \
    builder.CreateStore(builder.CreateLoad(Type::getInt64Ty(ctx), new_pc), pc); \
}

#include "instrs.h"
#undef _INSTR_

};

