#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <bitset>
#include <vector>
#include <map>
#include <functional>
#include "instruction.hpp"

const int REGISTER_SIZE = 64;


// TODO fill correct values
enum class OpFunctionCodes : uint16_t {
    ADD  = 0x0000,
    SLT  = 0x0001,
    SLTU = 0x0002,
    AND  = 0x0003,
    OR   = 0x0004,
    XOR  = 0x0005,
    SLL  = 0x0006,
    SRL  = 0x0007,
    SUB  = 0x0100,
    SRA  = 0x0101
};


enum class OpImmFunctionCodes : uint16_t {
    ADDI  = 0x0000,
    SLTI  = 0x0001,
    SLTIU = 0x0002,
    ANDI  = 0x0003,
    ORI   = 0x0004,
    XORI  = 0x0005,
    SLLI  = 0x0006,
    SRLI  = 0x0007,
    SRAI  = 0x0100
};

enum class BranchFunctionCodes : uint16_t {
    BEQ  = 0x0000,
    BNE  = 0x0001,
    BLT  = 0x0002,
    BLTU = 0x0003,
    BGE  = 0x0004,
    BGEU = 0x0005
};

struct store_function {
    template <typename FunCode>
    store_function(InstrOpCode op_code, FunCode func_code, function_t exec) : 
    op_code(op_code), func_code(static_cast<uint16_t>(func_code)), executor(exec) {};

    InstrOpCode op_code;
    uint16_t    func_code;
    function_t  executor;
};
extern const std::vector<store_function> implemented_functions;

//integer register operations
void ADD(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt);

void SUB(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt);

void AND(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt);

void OR(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt);

void XOR(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt);

void SLT(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt);

void SLTU(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt);

void SLL(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt);

void SRL(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt);

void SRA(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt);


// integer immidiate register operations
void ADDI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt);

void SLTI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt);

void SLTIU(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt);

void ANDI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt);

void ORI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt);

void XORI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt);

void SLLI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt);

void SRLI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt);

void SRAI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt);


// lui instruction
void LUI(Heart* heart, uint64_t imm, uint64_t rd, uint64_t opt1, uint64_t opt2);

//auipc instruction
void AUIPC(Heart* heart, uint64_t imm, uint64_t rd, uint64_t opt1, uint64_t opt21);

//unconditional jumps
void JAL(Heart* heart, uint64_t imm, uint64_t rd, uint64_t opt1, uint64_t opt2);

void JALR(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt);

// branch functions
void BEQ(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t imm, uint64_t opt);

void BNE(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t imm, uint64_t opt);

void BLT(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t imm, uint64_t opt);

void BLTU(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t imm, uint64_t opt);

void BGE(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t imm, uint64_t opt);

void BGEU(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t imm, uint64_t opt);

#endif //FUNCTIONS_H