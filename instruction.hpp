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

//TO DO opcodes should be changed to correct values
enum class InstrOpCode : uint8_t {
    OP     = 0b0110011,
    OP_IMM = 0b0010011,
    LUI    = 0b0110111,
    AUIPC  = 0b0010111,
    JAL    = 0b1101111,
    JALR   = 0b1100111,
    BRANCH = 0b1100011
};

class Instruction {
    public:
        Instruction(const bits_t& code);
        virtual void execute(Heart* heart) {
            std::cout << "Base class execute called" << std::endl;
            return; 
        };

        InstrOpCode op_code;
        bits_t      instr_code;

        static bits_t opcode_mask;

        static void build_functions_map(std::map<uint32_t, function_t>& functions);
        static uint32_t get_key_val(uint16_t func_code, InstrOpCode op_code);
};

class InstructionR : public Instruction {
    public:
        InstructionR(const bits_t& code);
        function_t   executor;

        bits_t rs_1;
        bits_t rs_2;
        bits_t rd;
        bits_t fun_code;

        static bits_t opcode_mask;
        static bits_t funct7_mask;
        static bits_t rs2_mask;
        static bits_t rs1_mask;
        static bits_t funct3_mask;
        static bits_t rd_mask;

        virtual void execute(Heart* heart) override;
};

class InstructionI : public Instruction {
    public:
        InstructionI(const bits_t& code);
        function_t  executor;

        bits_t rs_1;
        bits_t rd;
        bits_t fun_code;
        bits_t imm;

        static bits_t imm_mask;
        static bits_t rs1_mask;
        static bits_t funct3_mask;
        static bits_t rd_mask;
        static bits_t opcode_mask;

        virtual void execute(Heart* heart) override;
};

class InstructionS : public Instruction {
    public:
        InstructionS(const bits_t& code);
        function_t  executor;

        bits_t rs_1;
        bits_t rs_2;
        bits_t fun_code;
        bits_t imm;

        static bits_t imm_lead_mask;
        static bits_t rs2_mask;
        static bits_t rs1_mask;
        static bits_t funct3_mask;
        static bits_t imm_tail_mask;
        static bits_t opcode_mask;

        virtual void execute(Heart* heart) override;
};

class InstructionU : public Instruction {
    public:
        InstructionU(const bits_t& code);
        function_t  executor;

        bits_t rd;
        bits_t imm;

        static bits_t imm_mask;
        static bits_t rd_mask;
        static bits_t opcode_mask;

        virtual void execute(Heart* heart);
};

class InstructionB : public InstructionS {
    public:
        InstructionB(const bits_t& code);

        int16_t get_branch_offset();
        int16_t branch_offset;

        virtual void execute(Heart* heart) override;
};

class InstructionJ : public InstructionU {
    public:
        InstructionJ(const bits_t& code);

        int16_t get_offset();
        int16_t offset;

        virtual void execute(Heart* heart) override;
};
#endif