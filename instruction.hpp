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

class Heart;

typedef uint64_t regT;
typedef uint32_t instT;
typedef uint8_t regIDT;

class Instruction {
public:
    typedef std::function<void (Heart*, const Instruction&)> executorT;
    Instruction(const instT& code);

    instT instr_code;
    regIDT rs1, rs2, rd;
    uint32_t imm;
    executorT execute;
//    static uint32_t get_key_val(uint16_t func_code, InstrOpCode op_code);
};

class InstructionR : public Instruction {
public:
    InstructionR(const instT& code) : Instruction(code) {
        RS1_INIT RS2_INIT RD_INIT
    }
};

class InstructionI : public Instruction {
public:
    InstructionI(const instT& code) : Instruction(code) {
        RS1_INIT RD_INIT
        imm = (code&INSN_FIELD_IMM12) >> 20;
    }
};

class InstructionS : public Instruction {
public:
    InstructionS(const instT& code) : Instruction(code) {
        RS1_INIT RS2_INIT
        imm = ((code&INSN_FIELD_IMM12HI) >> 20)
            | ((code&INSN_FIELD_IMM12LO) >> 7);
    }
};

class InstructionB : public Instruction {
public:
    InstructionB(const instT& code) : Instruction(code) {
        RS1_INIT RS2_INIT
        imm = ((code >> 31) << 12)
            | ((code & (1<<7)) << 4)
            | ((code & 0x7e000000) >> 20)
            | ((code & 0xf00) >> 7);
    }
};

class InstructionU : public Instruction {
public:
    InstructionU(const instT& code) : Instruction(code) {
        imm = (code & INSN_FIELD_IMM20);
        RD_INIT
    }
};

class InstructionJ : public Instruction {
public:
    InstructionJ(const instT& code) : Instruction(code) {
        imm = ((code >> 31) << 20)
            | ((code & INSN_FIELD_ZIMM10) >> 20)
            | ((code & (1<<20)) >> 9)
            | ((code & (0xff000)));
    }
};

#endif
