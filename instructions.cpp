#include "instruction.hpp"
#include "functions.hpp"
#include "heart.hpp"

Instruction::Instruction(const bits_t& code) {
    instr_code = code;
    op_code = static_cast<InstrOpCode>((code & opcode_mask).to_ullong());
};
void Instruction::build_functions_map(std::map<uint32_t, function_t>& functions) {
    uint32_t key = 0;
    for (auto& fun : implemented_functions) {
        key = get_key_val(fun.func_code, fun.op_code);
        functions.insert({key, fun.executor});
    }
}
bits_t Instruction::opcode_mask = 0x0000007F;

uint32_t Instruction::get_key_val(uint16_t func_code, InstrOpCode op_code) {
    uint32_t key_val = static_cast<uint16_t>(op_code);
    key_val = key_val << 16;
    key_val += func_code;
    return key_val;
}

InstructionR::InstructionR(const bits_t& code) : Instruction(code) {
    rs_1 = code & rs1_mask;
    rs_1 >>= 15;

    rs_2 = code & rs2_mask;
    rs_2 >>= 20;

    rd = code & rd_mask;
    rd >>= 7;

    fun_code = ((code & funct3_mask) >> 12) | ((code & funct7_mask) >> 22);
};

void InstructionR::execute(Heart* heart) {
    executor = heart->functions[get_key_val(fun_code.to_ulong(), op_code)];
    executor(heart, rs_1.to_ullong(), rs_2.to_ullong(), rd.to_ullong(), 0);
}
bits_t InstructionR::funct7_mask = 0xFE000000;
bits_t InstructionR::rs2_mask    = 0x01F00000;
bits_t InstructionR::rs1_mask    = 0x000F8000;
bits_t InstructionR::funct3_mask = 0x00007000;
bits_t InstructionR::rd_mask     = 0x00000F80;

InstructionI::InstructionI(const bits_t& code) : Instruction(code) {
    rs_1 = code & rs1_mask;
    rs_1 >>= 15;

    imm = code & imm_mask;
    imm >>= 20;

    rd = code & rd_mask;
    rd >>= 7;

    fun_code = code & funct3_mask;
    fun_code >>= 12;
};

void InstructionI::execute(Heart* heart) {
    executor = heart->functions[get_key_val(fun_code.to_ulong(), op_code)];
    executor(heart, rs_1.to_ullong(), imm.to_ullong(), rd.to_ullong(), 0);
}
bits_t InstructionI::imm_mask    = 0xFFF00000;
bits_t InstructionI::rs1_mask    = 0x000F8000;
bits_t InstructionI::funct3_mask = 0x00007000;
bits_t InstructionI::rd_mask     = 0x00000F80;

InstructionS::InstructionS(const bits_t& code) : Instruction(code) {
    rs_1 = code & rs1_mask;
    rs_1 >>= 15;

    rs_2 = code & rs2_mask;
    rs_2 >>= 20;

    fun_code = code & funct3_mask;
    fun_code >>= 12;

    imm = ((code & imm_tail_mask) >> 7) | ((code & imm_lead_mask) >> 20);
};
void InstructionS::execute(Heart* heart) {
    executor = heart->functions[get_key_val(fun_code.to_ulong(), op_code)];
    executor(heart, rs_1.to_ullong(), rs_2.to_ullong(), imm.to_ullong(), 0);
}
bits_t InstructionS::imm_lead_mask = 0xFE000000;
bits_t InstructionS::rs2_mask      = 0x000F8000;
bits_t InstructionS::rs1_mask      = 0x000F8000;
bits_t InstructionS::funct3_mask   = 0x00007000;
bits_t InstructionS::imm_tail_mask = 0xFE000000;


InstructionU::InstructionU(const bits_t& code) : Instruction(code) {
    rd = code & rd_mask;
    rd >>= 7;

    imm = code & imm_mask;
    imm >>= 12;
};
void InstructionU::execute(Heart* heart) {
    executor = heart->functions[get_key_val(0x00, op_code)];
    executor(heart, imm.to_ullong(), rd.to_ullong(), 0, 0);
}
bits_t InstructionU::imm_mask    = 0xFFFFF000;
bits_t InstructionU::rd_mask     = 0x00000F80;

// see documentation, imm fields here are encoded, get_branch_offset(), get_offset() - decode and retrieve jump value
InstructionB::InstructionB(const bits_t& code) : InstructionS(code) { 
    branch_offset = get_branch_offset();
};

//[12][imm 10 : 5][imm 4 : 1][11] - pizdak, kto eto pridumal??? 
// ^                          
// |                          
// +---- sign bit in immidiate value
int16_t InstructionB::get_branch_offset() {
    std::bitset<16> decoded_offset;

    bits_t imm_middle = 0x000003FE;
    bits_t imm_11     = 0x00000001;

    bool sign_bit = imm[12];

    imm_middle &= imm;
    imm_middle >>= 1;

    imm_11 &= imm;
    imm_11 <<= 11;

    imm = imm_middle | imm_11;

    // correct sign bit
    decoded_offset[decoded_offset.size() - 1] = sign_bit;
    for (size_t bit = 0; bit < imm.size(); bit ++)
        decoded_offset[bit] = imm[bit];
    
    return 2 * static_cast<int16_t>(decoded_offset.to_ulong());
}
void InstructionB::execute(Heart* heart) {
    executor = heart->functions[get_key_val(fun_code.to_ulong(), op_code)];
    executor(heart, rs_1.to_ullong(), rs_2.to_ullong(), branch_offset, 0);
}

// contains imm 32 bits length for some freedom to shift and some modifications
InstructionJ::InstructionJ(const bits_t& code) : InstructionU(code) {
    offset = get_offset();
};

int16_t InstructionJ::get_offset() {
    std::bitset<16> decoded_offset;

    bits_t imm_begin = 0x0007FE00;
    bits_t imm_end   = 0x000000FF;
    bits_t imm_11    = 0x00000100;
    bool   sign_bit    = imm[20];

    imm_begin &= imm;
    // imm[19:12] = 8
    // imm[11]    = 1
    imm_begin >>= 9;
    //[8:1] -> [19:12]
    imm_end &= imm;
    imm_end <<= 11;
    // shift to 11th position
    imm_11 &= imm;
    imm_11 <<= 2;

    // correct sign bit
    decoded_offset[decoded_offset.size() - 1] = sign_bit;
    imm = imm_begin | imm_end | imm_11;
    for (size_t bit = 0; bit < imm_begin.size(); bit ++)
        decoded_offset[bit] = imm_begin[bit];

    return 2 * static_cast<int16_t>(decoded_offset.to_ulong());
}

void InstructionJ::execute(Heart* heart) {
    executor = heart->functions[get_key_val(0x00, op_code)];
    executor(heart, offset, rd.to_ullong(), 0, 0);
}
