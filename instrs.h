#include "encoding.out.h"
// #define _INSTR_(name, type, code, linear)

#define RS1 (instr.rs1)
#define RS2 (instr.rs2)
#define RD (instr.rd)
#define IMM (instr.imm)
#define OPC (heart->pc)
#define NEW_PC (new_pc)
#define MEM(ind) (heart->memory+(ind))
#define REG(reg) (heart->get_reg(reg))
#define SET_REG(reg, val) (heart->set_reg(reg, val)); DEB("set reg: " << REG(reg));

#define I64PTR (Type::getInt64PtrTy(ctx))
#define I64T (Type::getInt64Ty(ctx))
#define I32PTR (Type::getInt32PtrTy(ctx))
#define I16PTR (Type::getInt16PtrTy(ctx))
#define I8PTR (Type::getInt8PtrTy(ctx))

#define IRCR(name, ...) builder.Create##name(__VA_ARGS__)
#define IRGEP64(...) builder.CreateConstGEP1_64(I64PTR, __VA_ARGS__)

#define LC64(num) builder.getInt64(num)

#define LGET(reg) ( (reg) ? (IRCR(Load, I64T, IRGEP64(regs, reg), #reg "_" )) \
                          : ((Value*) LC64(0)) )

#define LSET(reg, val) {if (reg) IRCR(Store, val, IRGEP64(regs, reg, #reg "_"));}

#define LPC (IRCR(Load, I64T, pc, "pc"))

#define CODE_BIN_IU(op) SET_REG(RD, REG(RS1) op IMM);
#define CODE_BIN_IS(op) SET_REG(RD, ((int64_t)REG(RS1)) op ((int64_t)IMM));

#define CODE_BIN_RU(op) SET_REG(RD, REG(RS1) op REG(RS2));
#define CODE_BIN_RS(op) SET_REG(RD, ((int64_t)REG(RS1)) op ((int64_t)REG(RS2)));

#define CODE_CJU(op) ((REG(RS1) op REG(RS2)) && (NEW_PC = OPC + IMM));
#define CODE_CJS(op) ((((int64_t)REG(RS1)) op ((int64_t)REG(RS2))) && (NEW_PC = OPC + IMM));

#define SIX_BITS ((1<<6)-1)
#define FIV_BITS ((1<<5)-1)

#define JIT_BIN_IU(name) LSET(RD, IRCR(name, LC64(IMM), LGET(RS1)));

_INSTR_(SLTI, I, {CODE_BIN_IS(<)}, true, { JIT_BIN_IU(ICmpSLT) })
_INSTR_(SLTIU, I, {CODE_BIN_IU(<)}, true, { JIT_BIN_IU(ICmpULT) })
_INSTR_(ADDI, I, {CODE_BIN_IU(+)}, true, { JIT_BIN_IU(Add) })
_INSTR_(ANDI, I, {CODE_BIN_IU(&)}, true, { JIT_BIN_IU(And) })
_INSTR_(ORI, I, {CODE_BIN_IU(|)}, true, {JIT_BIN_IU(Or) })
_INSTR_(XORI, I, {CODE_BIN_IU(^)}, true, {JIT_BIN_IU(Xor)})
_INSTR_(SLLI, I, {SET_REG(RD, REG(RS1) << (IMM & SIX_BITS))}, true, { JIT_BIN_IU(Shl) }) // TODO check
_INSTR_(SRLI, I, {SET_REG(RD,
      !!(IMM & 1<<10)*(REG(RS1) >> (IMM & SIX_BITS)) // logical
      |
      !(IMM & 1<<10)*((int64_t)REG(RS1) >> (IMM & SIX_BITS)) // arithmetic
)}, true, {
        Value *ans=nullptr;
        if (IMM & 1<<10)
            ans = IRCR(LShr, LGET(RS1), LC64(IMM&SIX_BITS));
        else
            ans = IRCR(AShr, LGET(RS1), LC64(IMM&SIX_BITS));
        LSET(RD, ans);
}) // TODO check for correctness // WARNING decoded same as SRAI
_INSTR_(SLLIW, I, {SET_REG(RD, (uint32_t)REG(RS1) << (IMM & SIX_BITS))}, true, {
     LSET(RD, IRCR(ZExt, IRCR(Shl, IRCR(Trunc, LGET(RS1), I32PTR), LC64(IMM & SIX_BITS) ) , I64PTR));
}) // TODO check
_INSTR_(SRLIW, I, {SET_REG(RD,
                          !!(IMM & 1<<10)*((uint32_t)REG(RS1) >> (IMM & SIX_BITS)) // logical
                          |
                          !(IMM & 1<<10)*((int32_t)REG(RS1) >> (IMM & SIX_BITS)) // arithmetic
)}, true, {
    Value *ans=nullptr;
    if (IMM & 1<<10)
         ans = IRCR(ZExt, IRCR(LShr, IRCR(Trunc, LGET(RS1), I32PTR), LC64(IMM & SIX_BITS)), I64PTR);
    else
         ans = IRCR(ZExt, IRCR(AShr, IRCR(Trunc, LGET(RS1), I32PTR), LC64(IMM & SIX_BITS)), I64PTR);
    LSET(RD, ans);
}) // TODO check for correctness // WARNING decoded same as SRAIW
_INSTR_(LUI, U, { SET_REG(RD, (IMM&INSN_FIELD_IMM20)) }, true, { LSET(RD, LC64(IMM&INSN_FIELD_IMM20)); })
_INSTR_(AUIPC, U,{ SET_REG(RD, (IMM + OPC)) }, true, {LSET(RD, IRCR(Add, LC64(IMM), LPC));}) 

#define JIT_BIN_OP(name) LSET(RD, IRCR(name, LGET(RS1), LGET(RS2)));

_INSTR_(ADD, R, {CODE_BIN_RU(+)}, true, { JIT_BIN_OP(Add) })
_INSTR_(SLT, R, {CODE_BIN_RS(<)}, true, { JIT_BIN_OP(ICmpSLT) })
_INSTR_(SLTU, R, {CODE_BIN_RU(<)}, true, { JIT_BIN_OP(ICmpULT) })
_INSTR_(AND, R, {CODE_BIN_RU(&)}, true, { JIT_BIN_OP(And) })
_INSTR_(OR, R, {CODE_BIN_RU(|)}, true, { JIT_BIN_OP(Or) })
_INSTR_(XOR, R, {CODE_BIN_RU(^)}, true, { JIT_BIN_OP(Xor) })
_INSTR_(SLL, R, {SET_REG(RD, REG(RS1) << (REG(RS2) & SIX_BITS))}, true, {
    LSET(RD, IRCR(Shl, LGET(RS1), IRCR(And, LGET(RS2), LC64(SIX_BITS))))
}) // TODO check
_INSTR_(SRL, R, {SET_REG(RD, (REG(RS1) >> (REG(RS2) & SIX_BITS)))}, true, {
    LSET(RD, IRCR(LShr, LGET(RS1), IRCR(And, LGET(RS2), LC64(SIX_BITS))))
}) // TODO check
_INSTR_(SRA, R, {SET_REG(RD, ((int64_t)REG(RS1) >> (REG(RS2) & SIX_BITS)))}, true, {
    LSET(RD, IRCR(AShr, LGET(RS1), IRCR(And, LGET(RS2), LC64(SIX_BITS))))
}) // TODO check
_INSTR_(SLLW, R, {SET_REG(RD, (uint32_t)REG(RS1) << (REG(RS2) & FIV_BITS))}, true, {
    LSET(RD, IRCR(ZExt, IRCR(Shl, IRCR(Trunc, LGET(RS1), I32PTR), IRCR(And, LGET(RS2), LC64(FIV_BITS))), I64PTR));
}) // TODO check
_INSTR_(SRLW, R, {SET_REG(RD, ((uint32_t)REG(RS1) >> (REG(RS2) & FIV_BITS)))}, true, {
    LSET(RD, IRCR(ZExt, IRCR(LShr, IRCR(Trunc, LGET(RS1), I32PTR), IRCR(And, LGET(RS2), LC64(FIV_BITS))), I64PTR));
}) // TODO check
_INSTR_(SRAW, R, {SET_REG(RD, ((int32_t)REG(RS1) >> (REG(RS2) & FIV_BITS)))}, true, {
    LSET(RD, IRCR(ZExt, IRCR(AShr, IRCR(Trunc, LGET(RS1), I32PTR), IRCR(And, LGET(RS2), LC64(FIV_BITS))), I64PTR));
}) // TODO check
_INSTR_(SUB, R, {CODE_BIN_RU(-)}, true, { JIT_BIN_OP(Sub) })
_INSTR_(SUBW, R, {SET_REG(RD, (int64_t)(int32_t)(REG(RS1) - REG(RS2)))}, true, {
    LSET(RD, IRCR(SExt, IRCR(Sub, IRCR(Trunc, LGET(RS1), I32PTR), LGET(RS2)), I64PTR));
}) // TODO check if it's correct but it should be nice
_INSTR_(ADDW, R, {SET_REG(RD, (int64_t)(int32_t)(REG(RS1) + REG(RS2)))}, true, {
    LSET(RD, IRCR(SExt, IRCR(Add, IRCR(Trunc, LGET(RS1), I32PTR), LGET(RS2)), I64PTR));
}) // TODO check if it's correct but it should be nice
_INSTR_(JAL, J, { NEW_PC = OPC + IMM; SET_REG(RD, OPC + 4); }, false, {
    new_pc = IRCR(Add, LPC, LC64(IMM), "new_pc"); 
    LSET(RD, IRCR(Add, LPC, LC64(4)));
})
_INSTR_(JALR, I, { NEW_PC = (IMM + REG(RS1)) & ~1ULL; SET_REG(RD, OPC + 4); }, false, {
    new_pc = IRCR(And, IRCR(Add, LGET(RS1), LC64(IMM)), LC64(~1ULL), "new_pc");
    LSET(RD, IRCR(Add, LPC, LC64(4)));
})

#define JIT_COND_JMP(name) BasicBlock *trueBr = BasicBlock::Create(ctx, "", fn); \
    BasicBlock *retBr = BasicBlock::Create(ctx, "", fn); \
    Value* old_new_pc = new_pc; \
    BasicBlock* entryBlock = &fn->getEntryBlock(); \
    IRCR(CondBr, IRCR(ICmp##name, LGET(RS1), LGET(RS2)), trueBr, retBr); \
    builder.SetInsertPoint(trueBr); \
    new_pc = IRCR(Add, LPC, LC64(IMM), "new_pc"); \
    builder.CreateBr(retBr); \
    builder.SetInsertPoint(retBr); \
    PHINode* phi = IRCR(PHI, I64T, 2, "new_pc_" #name); \
    phi->addIncoming(old_new_pc, entryBlock); \
    phi->addIncoming(new_pc, trueBr); \
    new_pc = phi;

_INSTR_(BEQ, B, {CODE_CJU(==)}, false, { JIT_COND_JMP(EQ) })
_INSTR_(BNE, B, {CODE_CJU(!=)}, false, { JIT_COND_JMP(NE) })
_INSTR_(BLT, B, {CODE_CJS(<)}, false, { JIT_COND_JMP(SLT) })
_INSTR_(BLTU, B, {CODE_CJU(<)}, false, { JIT_COND_JMP(ULT) })
_INSTR_(BGE, B, {CODE_CJS(>=)}, false, { JIT_COND_JMP(SGE) })
_INSTR_(BGEU, B, {CODE_CJU(>=)}, false, { JIT_COND_JMP(UGE) })

_INSTR_(ADDIW, I, {SET_REG(RD, (int64_t)(int32_t)(REG(RS1) + IMM))}, true, {
    LSET(RD, IRCR(SExt, IRCR(Add, IRCR(Trunc, LGET(RS1), I32PTR), LC64(IMM)), I64PTR));
}) // TODO check if it's correct but it should be nice
// TODO check type conversions

#define IRADDR IRCR(GEP, I8PTR, mem, IRCR(Add, LGET(RS1), LC64(IMM)))

_INSTR_(LD, I, { SET_REG(RD, *(uint64_t*)MEM(REG(RS1)+IMM)); }, false, {
    LSET(RD, IRCR(Load, I64PTR, IRADDR));
})
_INSTR_(LW, I, { SET_REG(RD, *(int32_t*)MEM(REG(RS1)+IMM)); }, false, {
    LSET(RD, IRCR(SExt, IRCR(Load, I32PTR, IRADDR), I64PTR));
})
_INSTR_(LWU, I, { SET_REG(RD, *(uint32_t*)MEM(REG(RS1)+IMM)); }, false, {
    LSET(RD, IRCR(ZExt, IRCR(Load, I32PTR, IRADDR), I64PTR));
})
_INSTR_(LH, I, { SET_REG(RD, *(int16_t*)MEM(REG(RS1)+IMM)); }, false, {
    LSET(RD, IRCR(SExt, IRCR(Load, I16PTR, IRADDR), I64PTR));
})
_INSTR_(LHU, I, { SET_REG(RD, *(uint16_t*)MEM(REG(RS1)+IMM)); }, false, {
    LSET(RD, IRCR(ZExt, IRCR(Load, I16PTR, IRADDR), I64PTR));
})
_INSTR_(LB, I, { SET_REG(RD, *(int8_t*)MEM(REG(RS1)+IMM)); }, false, {
    LSET(RD, IRCR(SExt, IRCR(Load, I8PTR, IRADDR), I64PTR));
})
_INSTR_(LBU, I, { SET_REG(RD, *(uint8_t*)MEM(REG(RS1)+IMM)); }, false, {
    LSET(RD, IRCR(ZExt, IRCR(Load, I8PTR, IRADDR), I64PTR));
})
_INSTR_(SD, S, { *(uint64_t*)MEM(REG(RS1)+IMM) = REG(RS2); }, false, {
    IRCR(Store, LGET(RS2), IRADDR);
})
_INSTR_(SW, S, { *(uint32_t*)MEM(REG(RS1)+IMM) = REG(RS2); }, false, {
    IRCR(Store,  IRCR(Trunc, LGET(RS2), I32PTR), IRADDR);
})
_INSTR_(SH, S, { *(uint16_t*)MEM(REG(RS1)+IMM) = REG(RS2); }, false, {
    IRCR(Store,  IRCR(Trunc, LGET(RS2), I16PTR), IRADDR);
})
_INSTR_(SB, S, { *(uint8_t*)MEM(REG(RS1)+IMM) = REG(RS2); }, false, {
    IRCR(Store,  IRCR(Trunc, LGET(RS2), I8PTR), IRADDR);
})
_INSTR_(ECALL, I, {printf("Ecall or ebreak: doing exit...\n"); heart->done = true;}, false, { IRCR(Store, builder.getInt1(true), done); }) // WARNING same as ebreak // TODO add actuall functionality
