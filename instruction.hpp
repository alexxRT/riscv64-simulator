#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>
#include <iostream>
#include <functional>
#include "encoding.out.h"

#define RD_SHIFT 7
#define RS1_SHIFT 15
#define RS2_SHIFT 20

#define RD_SHIFT 7
#define RD_SHIFT 7

#define RD_INIT {rd = (code&INSN_FIELD_RD) >> RD_SHIFT;}
#define RS1_INIT {rs1 = (code&INSN_FIELD_RS1) >> RS1_SHIFT;}
#define RS2_INIT {rs2 = (code&INSN_FIELD_RS2) >> RS2_SHIFT;}
#define SIGN_INIT {sign = code & (1<<31); code &= ~(1<<31);}

class Heart;

typedef uint64_t regT;
typedef uint32_t instT;
typedef uint8_t regIDT;

class Instruction {
public:
    typedef std::function<void (Heart*, const Instruction&)> executorT;
    Instruction(instT code);

    instT instr_code;
    regIDT rs1, rs2, rd;
    uint32_t imm;
    uint32_t sign;
    executorT execute;
//    static uint32_t get_key_val(uint16_t func_code, InstrOpCode op_code);
};

class InstructionR : public Instruction {
public:
    InstructionR(instT code) : Instruction(code) {
        RS1_INIT RS2_INIT RD_INIT
    }
};

class InstructionI : public Instruction {
public:
    InstructionI(instT code) : Instruction(code) {
        RS1_INIT RD_INIT SIGN_INIT
        imm = ((code&INSN_FIELD_IMM12) >> 20) | sign;
    }
};

class InstructionS : public Instruction {
public:
    InstructionS(instT code) : Instruction(code) {
        RS1_INIT RS2_INIT SIGN_INIT
        imm = ((code&INSN_FIELD_IMM12HI) >> 20)
            | ((code&INSN_FIELD_IMM12LO) >> 7)
            | sign;
    }
};

class InstructionB : public Instruction {
public:
    InstructionB(instT code) : Instruction(code) {
        RS1_INIT RS2_INIT SIGN_INIT
        imm = ((code & (1<<7)) << 4)
            | ((code & 0x7e000000) >> 20)
            | ((code & 0xf00) >> 7)
            | sign;
    }
};

class InstructionU : public Instruction {
public:
    InstructionU(instT code) : Instruction(code) {
        RD_INIT
        imm = (code & INSN_FIELD_IMM20);
    }
};

class InstructionJ : public Instruction {
public:
    InstructionJ(instT code) : Instruction(code) {
        SIGN_INIT
        imm = ((code & INSN_FIELD_ZIMM10) >> 20)
            | ((code & (1<<20)) >> 9)
            | ((code & (0xff000)))
            | sign
    }
};

#endif
