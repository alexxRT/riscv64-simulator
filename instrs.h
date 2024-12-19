#include "encoding.out.h"
#include <cstdint>
// #define _INSTR_(name, type, code, linear)

#define RS1 (instr.rs1)
#define RS2 (instr.rs2)
#define RD (instr.rd)
#define IMM (instr.imm)
#define PC (heart->pc)
#define NEW_PC (new_pc)
#define MEM(ind) (heart->memory+(ind))
#define REG(reg) (heart->get_reg(reg))
#define SET_REG(reg, val) (heart->set_reg(reg, val)); DEB("set reg: " << REG(reg));

#define CODE_BIN_IU(op) SET_REG(RD, REG(RS1) op IMM);
#define CODE_BIN_IS(op) SET_REG(RD, ((int64_t)REG(RS1)) op ((int64_t)IMM));

#define CODE_BIN_RU(op) SET_REG(RD, REG(RS1) op REG(RS2));
#define CODE_BIN_RS(op) SET_REG(RD, ((int64_t)REG(RS1)) op ((int64_t)REG(RS2)));

#define CODE_CJU(op) ((REG(RS1) op REG(RS2)) && (NEW_PC = PC + IMM));
#define CODE_CJS(op) ((((int64_t)REG(RS1)) op ((int64_t)REG(RS2))) && (NEW_PC = PC + IMM));

#define SIX_BITS ((1<<6)-1)
#define FIV_BITS ((1<<5)-1)

_INSTR_(SLTI, I, {CODE_BIN_IS(<)}, true)
_INSTR_(SLTIU, I, {CODE_BIN_IU(<)}, true)
_INSTR_(ADDI, I, {CODE_BIN_IU(+)}, true)
_INSTR_(ANDI, I, {CODE_BIN_IU(&)}, true)
_INSTR_(ORI, I, {CODE_BIN_IU(|)}, true)
_INSTR_(XORI, I, {CODE_BIN_IU(^)}, true)
_INSTR_(SLLI, I, {SET_REG(RD, REG(RS1) << (IMM & SIX_BITS))}, true) // TODO check
_INSTR_(SRLI, I, {SET_REG(RD,
                          !!(IMM & 1<<10)*(REG(RS1) >> (IMM & SIX_BITS)) // logical
                          |
                          !(IMM & 1<<10)*((int64_t)REG(RS1) >> (IMM & SIX_BITS)) // arithmetic
)}, true) // TODO check for correctness // WARNING decoded same as SRAI
_INSTR_(SLLIW, I, {SET_REG(RD, (uint32_t)REG(RS1) << (IMM & SIX_BITS))}, true) // TODO check
_INSTR_(SRLIW, I, {SET_REG(RD,
                          !!(IMM & 1<<10)*((uint32_t)REG(RS1) >> (IMM & SIX_BITS)) // logical
                          |
                          !(IMM & 1<<10)*((int32_t)REG(RS1) >> (IMM & SIX_BITS)) // arithmetic
)}, true) // TODO check for correctness // WARNING decoded same as SRAIW
_INSTR_(LUI, U, { SET_REG(RD, (IMM&INSN_FIELD_IMM20)) }, true)
_INSTR_(AUIPC, U,{ SET_REG(RD, (IMM + PC)) }, true) 
_INSTR_(ADD, R, {CODE_BIN_RU(+)}, true)
_INSTR_(SLT, R, {CODE_BIN_RS(<)}, true)
_INSTR_(SLTU, R, {CODE_BIN_RU(<)}, true)
_INSTR_(AND, R, {CODE_BIN_RU(&)}, true)
_INSTR_(OR, R, {CODE_BIN_RU(|)}, true)
_INSTR_(XOR, R, {CODE_BIN_RU(^)}, true)
_INSTR_(SLL, R, {SET_REG(RD, REG(RS1) << (REG(RS2) & SIX_BITS))}, true) // TODO check
_INSTR_(SRL, R, {SET_REG(RD, (REG(RS1) >> (REG(RS2) & SIX_BITS)))}, true) // TODO check
_INSTR_(SRA, R, {SET_REG(RD, ((int64_t)REG(RS1) >> (REG(RS2) & SIX_BITS)))}, true) // TODO check
_INSTR_(SLLW, R, {SET_REG(RD, (uint32_t)REG(RS1) << (REG(RS2) & FIV_BITS))}, true) // TODO check
_INSTR_(SRLW, R, {SET_REG(RD, ((uint32_t)REG(RS1) >> (REG(RS2) & FIV_BITS)))}, true) // TODO check
_INSTR_(SRAW, R, {SET_REG(RD, ((int32_t)REG(RS1) >> (REG(RS2) & FIV_BITS)))}, true) // TODO check
_INSTR_(SUB, R, {CODE_BIN_RU(-)}, true)
_INSTR_(SUBW, R, {SET_REG(RD, (int64_t)(int32_t)(REG(RS1) - REG(RS2)))}, true) // TODO check if it's correct but it should be nice
_INSTR_(ADDW, R, {SET_REG(RD, (int64_t)(int32_t)(REG(RS1) + REG(RS2)))}, true) // TODO check if it's correct but it should be nice
_INSTR_(JAL, J, { NEW_PC = PC + IMM; SET_REG(RD, PC + 4); }, false)
_INSTR_(JALR, I, { NEW_PC = (IMM + REG(RS1)) & ~1ULL; SET_REG(RD, PC + 4); }, false)
_INSTR_(BEQ, B, {CODE_CJU(==)}, false)
_INSTR_(BNE, B, {CODE_CJU(!=)}, false)
_INSTR_(BLT, B, {CODE_CJS(<)}, false)
_INSTR_(BLTU, B, {CODE_CJU(<)}, false)
_INSTR_(BGE, B, {CODE_CJS(>=)}, false)
_INSTR_(BGEU, B, {CODE_CJU(>=)}, false)
_INSTR_(ADDIW, I, {SET_REG(RD, (int64_t)(int32_t)(REG(RS1) + IMM))}, true) // TODO check if it's correct but it should be nice
// TODO check type conversions
_INSTR_(LD, I, { SET_REG(RD, *(uint64_t*)MEM(REG(RS1)+IMM)); }, true)
_INSTR_(LW, I, { SET_REG(RD, *(int32_t*)MEM(REG(RS1)+IMM)); }, true)
_INSTR_(LWU, I, { SET_REG(RD, *(uint32_t*)MEM(REG(RS1)+IMM)); }, true)
_INSTR_(LH, I, { SET_REG(RD, *(int16_t*)MEM(REG(RS1)+IMM)); }, true)
_INSTR_(LHU, I, { SET_REG(RD, *(uint16_t*)MEM(REG(RS1)+IMM)); }, true)
_INSTR_(LB, I, { SET_REG(RD, *(int8_t*)MEM(REG(RS1)+IMM)); }, true)
_INSTR_(LBU, I, { SET_REG(RD, *(uint8_t*)MEM(REG(RS1)+IMM)); }, true)
_INSTR_(SD, S, { *(uint64_t*)MEM(REG(RS1)+IMM) = REG(RS2); }, true)
_INSTR_(SW, S, { *(uint32_t*)MEM(REG(RS1)+IMM) = REG(RS2); }, true)
_INSTR_(SH, S, { *(uint16_t*)MEM(REG(RS1)+IMM) = REG(RS2); }, true)
_INSTR_(SB, S, { *(uint8_t*)MEM(REG(RS1)+IMM) = REG(RS2); }, true)
_INSTR_(ECALL, I, {printf("Ecall or ebreak: doing exit...\n"); heart->done = true;}, false) // WARNING same as ebreak // TODO add actuall functionality
