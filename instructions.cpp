#include "instruction.hpp"
#include "functions.hpp"

Instruction::Instruction(const bits_t& code) {
    instr_code = code;
    op_code = static_cast<InstrOpCode>((code & opcode_mask).to_ullong());
};
bits_t Instruction::opcode_mask = 0x000000EF;
void Instruction::build_functions_map(std::map<uint16_t, function_t>& functions) {
    uint16_t key = 0;
    for (auto& fun : implemented_functions) {
        key = get_key_val(fun.func_code, fun.op_code);
        functions.insert({key, fun.executor});
    }
}

uint16_t Instruction::get_key_val(uint16_t func_code, InstrOpCode op_code) {
    uint16_t key_val = static_cast<uint16_t>(op_code);
    key_val = key_val >> 8;
    key_val += func_code;
    return key_val;
}

InstructionR::InstructionR(const bits_t& code) : Instruction(code) {


};
bits_t InstructionR::funct7_mask = 0xFE000000;
bits_t InstructionR::rs2_mask    = 0x01F00000;
bits_t InstructionR::rs1_mask    = 0x000F8000;
bits_t InstructionR::funct3_mask = 0x00007000;
bits_t InstructionR::rd_mask     = 0x00000F80;

InstructionI::InstructionI(const bits_t& code) : Instruction(code) {

};
bits_t InstructionI::imm_mask    = 0xFFF00000;
bits_t InstructionI::rs1_mask    = 0x000F8000;
bits_t InstructionI::funct3_mask = 0x00007000;
bits_t InstructionI::rd_mask     = 0x00000F80;

InstructionS::InstructionS(const bits_t& code) : Instruction(code) {

};
bits_t InstructionS::imm_lead_mask = 0xFE000000;
bits_t InstructionS::rs2_mask      = 0x000F8000;
bits_t InstructionS::rs1_mask      = 0x000F8000;
bits_t InstructionS::funct3_mask   = 0x00007000;
bits_t InstructionS::imm_tail_mask = 0xFE000000;


InstructionU::InstructionU(const bits_t& code) : Instruction(code) {

};
bits_t InstructionU::imm_mask    = 0xFFFFF000;
bits_t InstructionU::rd_mask     = 0x00000F80;

InstructionB::InstructionB(const bits_t& code) : InstructionS(code) {

};

InstructionJ::InstructionJ(const bits_t& code) : InstructionU(code) {

};
