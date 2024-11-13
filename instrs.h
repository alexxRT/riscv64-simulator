#include "encoding.out.h"
#include <cstdint>
// #define _INSTR_(name, type, code)

#define RS1 (instr.rs1)
#define RS2 (instr.rs2)
#define RD (instr.rd)
#define IMM (instr.imm)
#define PC (heart->pc)
#define NEW_PC (heart->new_pc)
#define MEM(ind) (heart->memory+(ind))
#define REG(reg) (heart->get_reg(reg))
#define SET_REG(reg, val) (heart->set_reg(reg, val));

#define CODE_BIN_IU(op) SET_REG(RD, REG(RS1) op IMM);
#define CODE_BIN_IS(op) SET_REG(RD, ((int64_t)REG(RS1)) op ((int64_t)IMM));

#define CODE_BIN_RU(op) SET_REG(RD, REG(RS1) op REG(RS2));
#define CODE_BIN_RS(op) SET_REG(RD, ((int64_t)REG(RS1)) op ((int64_t)REG(RS2)));

#define CODE_CJU(op) ((REG(RS1) op REG(RS2)) && (NEW_PC = PC + IMM));
#define CODE_CJS(op) ((((int64_t)REG(RS1)) op ((int64_t)REG(RS2))) && (NEW_PC = PC + IMM));

_INSTR_(SLTI, I, {CODE_BIN_IS(<)})
_INSTR_(SLTIU, I, {CODE_BIN_IU(<)})
_INSTR_(ADDI, I, {CODE_BIN_IU(+)})
_INSTR_(ANDI, I, {CODE_BIN_IU(&)})
_INSTR_(ORI, I, {CODE_BIN_IU(|)})
_INSTR_(XORI, I, {CODE_BIN_IU(^)})
_INSTR_(SLLI, I, {}) // TODO
_INSTR_(SRLI, I, {}) // TODO
_INSTR_(SRAI, I, {}) // TODO
_INSTR_(SLLIW, I, {}) // TODO
_INSTR_(SRLIW, I, {}) // TODO
_INSTR_(SRAIW, I, {}) // TODO
_INSTR_(LUI, U, { SET_REG(RD, (IMM&INSN_FIELD_IMM20)) })
_INSTR_(AUIPC, U,{ SET_REG(RD, (IMM + PC)) }) 
_INSTR_(ADD, R, {CODE_BIN_RU(+)})
_INSTR_(SLT, R, {CODE_BIN_RS(<)})
_INSTR_(SLTU, R, {CODE_BIN_RU(<)})
_INSTR_(AND, R, {CODE_BIN_RU(&)})
_INSTR_(OR, R, {CODE_BIN_RU(|)})
_INSTR_(XOR, R, {CODE_BIN_RU(^)})
_INSTR_(SLL, R, {}) // TODO
_INSTR_(SRL, R, {}) // TODO
_INSTR_(SRA, R, {}) // TODO
_INSTR_(SLLW, R, {}) // TODO
_INSTR_(SRLW, R, {}) // TODO
_INSTR_(SRAW, R, {}) // TODO
_INSTR_(SUB, R, {CODE_BIN_RU(-)})
_INSTR_(SUBW, R, {}) // TODO
_INSTR_(ADDW, R, {}) // TODO
_INSTR_(JAL, J, { NEW_PC = PC + IMM; SET_REG(RD, PC + 4); })
_INSTR_(JALR, I, { NEW_PC = (IMM + REG(RS1)) & ~1ULL; SET_REG(RD, PC + 4); })
_INSTR_(BEQ, B, {CODE_CJU(==)})
_INSTR_(BNE, B, {CODE_CJU(!=)})
_INSTR_(BLT, B, {CODE_CJS(<)})
_INSTR_(BLTU, B, {CODE_CJU(<)})
_INSTR_(BGE, B, {CODE_CJS(>=)})
_INSTR_(BGEU, B, {CODE_CJU(>=)})
_INSTR_(ADDIW, I, {}) // TODO
// TODO check type conversions
_INSTR_(LD, I, { SET_REG(RD, *(uint64_t*)MEM(REG(RS1)+IMM)); })
_INSTR_(LW, I, { SET_REG(RD, *(int32_t*)MEM(REG(RS1)+IMM)); })
_INSTR_(LWU, I, { SET_REG(RD, *(uint32_t*)MEM(REG(RS1)+IMM)); })
_INSTR_(LH, I, { SET_REG(RD, *(int16_t*)MEM(REG(RS1)+IMM)); })
_INSTR_(LHU, I, { SET_REG(RD, *(uint16_t*)MEM(REG(RS1)+IMM)); })
_INSTR_(LB, I, { SET_REG(RD, *(int8_t*)MEM(REG(RS1)+IMM)); })
_INSTR_(LBU, I, { SET_REG(RD, *(uint8_t*)MEM(REG(RS1)+IMM)); })
_INSTR_(SD, S, { *(uint64_t*)MEM(REG(RS1)+IMM) = REG(RS2); })
_INSTR_(SW, S, { *(uint32_t*)MEM(REG(RS1)+IMM) = REG(RS2); })
_INSTR_(SH, S, { *(uint16_t*)MEM(REG(RS1)+IMM) = REG(RS2); })
_INSTR_(SB, S, { *(uint8_t*)MEM(REG(RS1)+IMM) = REG(RS2); })
