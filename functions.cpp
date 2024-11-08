
#include "heart.hpp"
#include "functions.hpp"




const std::vector<store_function> implemented_functions = {
    {InstrOpCode::OP, OpFunctionCodes::ADD,  ADD},
    {InstrOpCode::OP, OpFunctionCodes::SLT,  SLT},
    {InstrOpCode::OP, OpFunctionCodes::SLTU, SLTU},
    {InstrOpCode::OP, OpFunctionCodes::AND,  AND},
    {InstrOpCode::OP, OpFunctionCodes::OR,   OR},
    {InstrOpCode::OP, OpFunctionCodes::XOR,  XOR},
    {InstrOpCode::OP, OpFunctionCodes::SLL,  SLL},
    {InstrOpCode::OP, OpFunctionCodes::SRL,  SRL},
    {InstrOpCode::OP, OpFunctionCodes::SUB,  SUB},
    {InstrOpCode::OP, OpFunctionCodes::SRA,  SRA},
    {InstrOpCode::OP_IMM, OpImmFunctionCodes::ADDI,  ADDI},
    {InstrOpCode::OP_IMM, OpImmFunctionCodes::SLTI,  SLTI},
    {InstrOpCode::OP_IMM, OpImmFunctionCodes::SLTIU, SLTIU},
    {InstrOpCode::OP_IMM, OpImmFunctionCodes::ANDI,  ANDI},
    {InstrOpCode::OP_IMM, OpImmFunctionCodes::ORI,   ORI},
    {InstrOpCode::OP_IMM, OpImmFunctionCodes::XORI,  XORI},
    {InstrOpCode::OP_IMM, OpImmFunctionCodes::SLLI,  SLLI},
    {InstrOpCode::OP_IMM, OpImmFunctionCodes::SRLI,  SRLI},
    {InstrOpCode::OP_IMM, OpImmFunctionCodes::SRAI,  SRAI},
    {InstrOpCode::OP_IMM, BranchFunctionCodes::BEQ,  BEQ},
    {InstrOpCode::OP_IMM, BranchFunctionCodes::BNE,  BNE},
    {InstrOpCode::OP_IMM, BranchFunctionCodes::BLT,  BLT},
    {InstrOpCode::OP_IMM, BranchFunctionCodes::BLTU, BLTU},
    {InstrOpCode::OP_IMM, BranchFunctionCodes::BGE,  BGE},
    {InstrOpCode::OP_IMM, BranchFunctionCodes::BGEU, BGEU},
    {InstrOpCode::LUI,   0x00, LUI},
    {InstrOpCode::AUIPC, 0x00, AUIPC},
    {InstrOpCode::JAL,   0x00, JAL},
    {InstrOpCode::JALR,  0x00, JALR}
};


void ADD(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2 = heart->registers[rs2];

    bool overflow = false;
    for (size_t i = 0; i < REGISTER_SIZE; i ++) {
        heart->registers[rd][i] = reg_source1[i] or reg_source2[i] or overflow;
        overflow = reg_source1[i] and reg_source2[i];
    }
    return;
}

void SUB(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2 = heart->registers[rs2];

    bool overflow = false;
    for (size_t i = 0; i < REGISTER_SIZE; i ++) {
        heart->registers[rd][i] = reg_source1[i] xor reg_source2[i] xor overflow;
        overflow = reg_source1[i] xor reg_source2[i];
    }
    return;
}

void AND(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2 = heart->registers[rs2];

    heart->registers[rd] = reg_source1 & reg_source2;
    return;
}

void OR(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2 = heart->registers[rs2];

    heart->registers[rd] = reg_source1 | reg_source2;
    return;
}

void XOR(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2 = heart->registers[rs2];

    heart->registers[rd] = reg_source1 ^ reg_source2;
    return;
}

// signed comparison
// writes 1 if rs1 < rs2 to rd
void SLT(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2 = heart->registers[rs2];

    bool sign_rs1 = reg_source1[REGISTER_SIZE - 1];
    bool sign_rs2 = reg_source2[REGISTER_SIZE - 1];

    // if rs1, rs2 have different signes
    if (sign_rs1 xor sign_rs2) {
        if (sign_rs1)
            heart->registers[rd][0] = true;
        else
            heart->registers[rd][0] = false;
        return;
    }

    // ignoring sign bit
    for (size_t i = REGISTER_SIZE - 2; i >= 0; i --) {
        if (reg_source1[i] < reg_source2[i] and sign_rs1 or reg_source1[i] > reg_source2[i] and !sign_rs1) {
            heart->registers[rd][0] = false;
            return;
        }
        else if (reg_source1[i] < reg_source2[i] and !sign_rs1 or reg_source1[i] > reg_source2[i] and sign_rs1) {
            heart->registers[rd][0] = true;
            return;
        }
    }
}

void SLTU(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2 = heart->registers[rs2];

    for (size_t i = REGISTER_SIZE - 1; i >= 0; i--) {
        if (reg_source1[i] < reg_source2[i]) {
            heart->registers[rd][0] = true;
            return;
        }
        else if (reg_source1[i] > reg_source2[i]) {
            heart->registers[rd][0] = false;
            return;
        }
    }
}

void SLL(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2 = heart->registers[rs2];

    heart->registers[rd] = reg_source1 << reg_source2.to_ullong();
    return;
}

void SRL(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2 = heart->registers[rs2];

    heart->registers[rd] = reg_source1 >> reg_source2.to_ullong();
    return;
}

void SRA(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2 = heart->registers[rs2];

    heart->registers[rd] = reg_source1;

    bool sign_bit = reg_source1[REGISTER_SIZE - 1];
    for (uint64_t i = 0; i < reg_source2.to_ullong(); i ++) {
        heart->registers[rd] >> 1;
        heart->registers[rd][REGISTER_SIZE - 1] = sign_bit;
    }
    return;
}

// integer immidiate register operations
void ADDI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2(imm);

    bool overflow = false;
    for (size_t i = 0; i < REGISTER_SIZE; i ++) {
        heart->registers[rd][i] = reg_source1[i] or reg_source2[i] or overflow;
        overflow = reg_source1[i] and reg_source2[i];
    }
    return;
}

void SLTI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2(imm);

    bool sign_rs1 = reg_source1[REGISTER_SIZE - 1];
    bool sign_rs2 = reg_source2[REGISTER_SIZE - 1];

    // if rs1, rs2 have different signes
    if (sign_rs1 xor sign_rs2) {
        if (sign_rs1)
            heart->registers[rd][0] = true;
        else
            heart->registers[rd][0] = false;
        return;
    }

    // ignoring sign bit
    for (size_t i = REGISTER_SIZE - 2; i >= 0; i --) {
        if (reg_source1[i] < reg_source2[i] and sign_rs1 or reg_source1[i] > reg_source2[i] and !sign_rs1) {
            heart->registers[rd][0] = false;
            return;
        }
        else if (reg_source1[i] < reg_source2[i] and !sign_rs1 or reg_source1[i] > reg_source2[i] and sign_rs1) {
            heart->registers[rd][0] = true;
            return;
        }
    }
}

void SLTIU(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2(imm);

    for (size_t i = REGISTER_SIZE - 1; i >= 0; i--) {
        if (reg_source1[i] < reg_source2[i]) {
            heart->registers[rd][0] = true;
            return;
        }
        else if (reg_source1[i] > reg_source2[i]) {
            heart->registers[rd][0] = false;
            return;
        }
    }
}

void ANDI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2(imm);

    heart->registers[rd] = reg_source1 & reg_source2;
    return;
}

void ORI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2(imm);

    heart->registers[rd] = reg_source1 | reg_source2;
    return;
}

void XORI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2(imm);

    heart->registers[rd] = reg_source1 ^ reg_source2;
    return;
}

void SLLI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2(imm);

    heart->registers[rd] = reg_source1 << reg_source2.to_ullong();
    return;
}

void SRLI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2(imm);

    heart->registers[rd] = reg_source1 >> reg_source2.to_ullong();
    return;
}

void SRAI(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt = -1) {
    reg_t reg_source1   = heart->registers[rs1];
    heart->registers[rd] = reg_source1;

    bool sign_bit = reg_source1[REGISTER_SIZE - 1];
    for (uint32_t i = 0; i < imm; i ++) {
        heart->registers[rd] >> 1;
        heart->registers[rd][REGISTER_SIZE - 1] = sign_bit;
    }
    return;
}

void LUI(Heart* heart, uint64_t imm, uint64_t rd, uint64_t opt1 = -1, uint64_t opt2 = -1) {
    heart->registers[rd] = imm;
    return;
}

void AUIPC(Heart* heart, uint64_t imm, uint64_t rd, uint64_t opt1 = -1, uint64_t opt2 = -1) {
    heart->pc = imm;
    return;
}

//unconditional jumps
void JAL(Heart* heart, uint64_t imm, uint64_t rd, uint64_t opt1 = -1, uint64_t opt2 = -1) {
    reg_t reg_source2(imm);

    bool overflow = false;
    for (size_t i = 0; i < REGISTER_SIZE; i ++) {
        heart->registers[rd][i] = heart->pc[i] or reg_source2[i] or overflow;
        overflow = heart->pc[i] and reg_source2[i];
    }
    return;
}

void JALR(Heart* heart, uint64_t rs1, uint64_t imm, uint64_t rd, uint64_t opt = -1) {
    ADDI(heart, rs1, imm, rd);
    return;
}

// branch functions
void BEQ(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t imm, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2 = heart->registers[rs2];

    if (reg_source1 == reg_source2) {
        heart->pc = imm;
        return;
    }
    return;
}

void BNE(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t imm, uint64_t opt = -1) {
    reg_t reg_source1 = heart->registers[rs1];
    reg_t reg_source2 = heart->registers[rs2];

    if (reg_source1 != reg_source2) {
        heart->pc = imm;
        return;
    }
    return;
}

//change temparaly x0 value to store cmp result
void BLT(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t imm, uint64_t opt = -1) {
    SLT(heart, rs1, rs2, 0);
    if (heart->registers[0][0]) {
        heart->registers[0][0] = false;
        heart->pc = imm;
        return;
    }
    return;
}

void BLTU(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t imm, uint64_t opt = -1) {
    SLTU(heart, rs1, rs2, 0);
    if (heart->registers[0][0]) {
        heart->registers[0][0] = false;
        heart->pc = imm;
        return;
    }
    return;
}

void BGE(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t imm, uint64_t opt = -1) {
    SLT(heart, rs1, rs2, 0);
    if (!heart->registers[0][0]) {
        heart->pc = imm;
        return;
    }
    heart->registers[0][0] = false;
    return;
}

void BGEU(Heart* heart, uint64_t rs1, uint64_t rs2, uint64_t imm, uint64_t opt = -1) {
    SLTU(heart, rs1, rs2, 0);
    if (!heart->registers[0][0]) {
        heart->pc = imm;
        return;
    }
    heart->registers[0][0] = false;
    return;
}