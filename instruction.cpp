#include "instruction.hpp"
#include "hart.hpp"

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
void empty_executor(Hart *hart, const Instruction &instr) {} // example


#define _INSTR_(name, type, code) \
void exec_##name(Hart *heart, const Instruction &instr) { \
    DEB("exec "#name); \
    code }

#include "instrs.h"
#undef _INSTR_

};

