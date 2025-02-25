#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <csignal>
#include <cstdint>
#include <iostream>
#include <functional>
#include <llvm-16/llvm/IR/Function.h>
#include <llvm-16/llvm/IR/IRBuilder.h>
#include <llvm-16/llvm/IR/Value.h>
#include "encoding.out.h"

#define RD_SHIFT 7
#define RS1_SHIFT 15
#define RS2_SHIFT 20

#define RD_SHIFT 7
#define RD_SHIFT 7

#define RD_INIT {ins.rd = (code&INSN_FIELD_RD) >> RD_SHIFT;}
#define RS1_INIT {ins.rs1 = (code&INSN_FIELD_RS1) >> RS1_SHIFT;}
#define RS2_INIT {ins.rs2 = (code&INSN_FIELD_RS2) >> RS2_SHIFT;}
#define SIGN_INIT bool sign = code>>31;
#define SIGNN(n) (~((1ULL<<n)-1))

class Hart;

typedef uint64_t regT;
typedef uint32_t instT;
typedef uint8_t regIDT;

class Instruction {
public:
    typedef void (*executorT)(Hart*, const Instruction&);
    Instruction(instT code, executorT execute_) : execute(execute_) {}
    Instruction() {}

    instT instr_code;
    regIDT rs1, rs2, rd;
    uint64_t imm;
    executorT execute;
//    static uint32_t get_key_val(uint16_t func_code, InstrOpCode op_code);
};

void decode_instruction_R(Instruction &ins, instT code);

void decode_instruction_I(Instruction &ins, instT code);

void decode_instruction_S(Instruction &ins, instT code);

void decode_instruction_B(Instruction &ins, instT code);

void decode_instruction_U(Instruction &ins, instT code);

void decode_instruction_J(Instruction &ins, instT code);

namespace Executors {
void empty_executor(Hart *heart, const Instruction &instr); // for basic blocks


#define _INSTR_(name, type, code, linear, jit) \
void exec_##name(Hart *heart, const Instruction &instr);

#include "instrs.h"
#undef _INSTR_

};

namespace Jiters {
void empty_jiter(Instruction &instr, llvm::IRBuilder<> &builder); // for basic blocks

using llvm::Type;
using llvm::Value;
using llvm::Function;

#define _INSTR_(name, type, code, linear, jit) \
void jit_##name(Instruction &instr, llvm::IRBuilder<> &builder, llvm::LLVMContext &ctx, Value* regs, Value *mem, Value *pc, Function *fn, Value *done);

#include "instrs.h"
#undef _INSTR_

};

#endif
