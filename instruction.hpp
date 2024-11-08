#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <bitset>
#include <iostream>
#include <functional>
#include <map>

class Heart;

typedef std::bitset<64> reg_t;
typedef std::function<void (Heart*, const uint64_t, const uint64_t, const uint64_t, const uint64_t)> function_t;

typedef std::bitset<32> bits_t;
typedef std::bitset<5>  argument_t;
typedef std::bitset<3>  funcode_t;
typedef std::bitset<10> funcode_ext_t;
typedef std::bitset<12> imm_short_t;
typedef std::bitset<20> imm_long_t;

//TO DO opcodes should be changed to correct values
enum class InstrOpCode : size_t {
    OP     = 1,
    OP_IMM = 2,
    LUI    = 3,
    AUIPC  = 4,
    JAL    = 5,
    JALR   = 6,
    BRANCH = 7
};

typedef void (handler_t) (InstrOpCode op_code);

class Instruction {
    public:
        Instruction(const bits_t& code);
        virtual void execute(Heart* heart) { return; };

        InstrOpCode op_code;
        bits_t      instr_code;

        static bits_t opcode_mask;

        static void build_functions_map(std::map<uint16_t, function_t>& functions);
        static uint16_t get_key_val(uint16_t func_code, InstrOpCode op_code);
};

class InstructionR : public Instruction {
    public:
        InstructionR(const bits_t& code);
        function_t   executor;

        argument_t    rs_1;
        argument_t    rs_2;
        argument_t    rd;
        funcode_ext_t fun_code;

        static bits_t opcode_mask;
        static bits_t funct7_mask;
        static bits_t rs2_mask;
        static bits_t rs1_mask;
        static bits_t funct3_mask;
        static bits_t rd_mask;

        virtual void execute(Heart* heart) override {
            executor(heart, rs_1.to_ullong(), rs_2.to_ullong(), rd.to_ullong(), 0);
        }
};

class InstructionI : public Instruction {
    public:
        InstructionI(const bits_t& code);
        function_t  executor;

        argument_t  rs_1;
        argument_t  rd;
        funcode_t   fun_code;
        imm_short_t imm;

        static bits_t imm_mask;
        static bits_t rs1_mask;
        static bits_t funct3_mask;
        static bits_t rd_mask;
        static bits_t opcode_mask;

        virtual void execute(Heart* heart) override {
            executor(heart, rs_1.to_ullong(), imm.to_ullong(), rd.to_ullong(), 0);
        }
};

class InstructionS : public Instruction {
    public:
        InstructionS(const bits_t& code);
        function_t  executor;

        argument_t  rs_1;
        argument_t  rs_2;
        funcode_t   fun_code;
        imm_short_t imm;

        static bits_t imm_lead_mask;
        static bits_t rs2_mask;
        static bits_t rs1_mask;
        static bits_t funct3_mask;
        static bits_t imm_tail_mask;
        static bits_t opcode_mask;

        virtual void execute(Heart* heart) override {
            executor(heart, rs_1.to_ullong(), rs_2.to_ullong(), imm.to_ullong(), 0);
        }
};

class InstructionU : public Instruction {
    public:
        InstructionU(const bits_t& code);
        function_t  executor;

        argument_t  rd;
        imm_long_t  imm;

        static bits_t imm_mask;
        static bits_t rd_mask;
        static bits_t opcode_mask;

        virtual void execute(Heart* heart) override {
            executor(heart, imm.to_ullong(), rd.to_ullong(), 0, 0);
        }
};

class InstructionB : public InstructionS {
    public:
        InstructionB(const bits_t& code);

        uint64_t get_branch_offset() { };
        uint64_t branch_offset;

        virtual void execute(Heart* heart) override {
            executor(heart, rs_1.to_ullong(), rs_2.to_ullong(), branch_offset, 0);
        }
};

class InstructionJ : public InstructionU {
    public:
        InstructionJ(const bits_t& code);

        uint64_t  get_offset() { };
        uint64_t offset;

        virtual void execute(Heart* heart) override {
            executor(heart, offset, rd.to_ullong(), 0, 0);
        }
};
#endif