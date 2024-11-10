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
    ADD  = 0b0000000000,
    SLT  = 0b0000000010,
    SLTU = 0b0000000011,
    AND  = 0b0000000111,
    OR   = 0b0000000110,
    XOR  = 0b0000000100,
    SLL  = 0b0000000001,
    SRL  = 0b0000000101,
    SUB  = 0b0100000000,
    SRA  = 0b0100000101
};


enum class OpImmFunctionCodes : uint16_t {
    ADDI  = 0b00000000000,
    SLTI  = 0b00000000010,
    SLTIU = 0b00000000011,
    ANDI  = 0b00000000111,
    ORI   = 0b00000000110,
    XORI  = 0b00000000100,
    SLLI  = 0b00000000001,
    SRLI  = 0b00000000101,
    // problem: same func3 value -> differ by imm[30] bit
    // SRAI  = 0b00000000101
};


enum class BranchFunctionCodes : uint16_t {
    BEQ  = 0b000,
    BNE  = 0b001,
    BLT  = 0b100,
    BLTU = 0b110,
    BGE  = 0b101,
    BGEU = 0b111
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

// implemented incorectly, now they shift on full imm, but need only imm[4:0]
void SLLI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt);

// implemented incorectly, now they shift on full imm, but need only imm[4:0]
void SRLI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt);

// ISSUE: have same function code as SRLI, differs by imm[30] only
// void SRAI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt);


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