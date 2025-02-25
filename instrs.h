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

#define LGET(reg) ( builder.CreateLoad(Type::getInt64Ty(ctx), \
    builder.CreateConstGEP1_64(Type::getInt64PtrTy(ctx), regs, reg) ) )

#define LSET(reg, val) builder.CreateStore(val,  \
    builder.CreateConstGEP1_64(Type::getInt64PtrTy(ctx), regs, reg));

#define LC64(num) builder.getInt64(num)
#define LPC (builder.CreateLoad(Type::getInt64Ty(ctx), pc))

#define CODE_BIN_IU(op) SET_REG(RD, REG(RS1) op IMM);
#define CODE_BIN_IS(op) SET_REG(RD, ((int64_t)REG(RS1)) op ((int64_t)IMM));

#define CODE_BIN_RU(op) SET_REG(RD, REG(RS1) op REG(RS2));
#define CODE_BIN_RS(op) SET_REG(RD, ((int64_t)REG(RS1)) op ((int64_t)REG(RS2)));

#define CODE_CJU(op) ((REG(RS1) op REG(RS2)) && (NEW_PC = OPC + IMM));
#define CODE_CJS(op) ((((int64_t)REG(RS1)) op ((int64_t)REG(RS2))) && (NEW_PC = OPC + IMM));

#define SIX_BITS ((1<<6)-1)
#define FIV_BITS ((1<<5)-1)

#define JIT_BIN_IU(name) LSET(RD, builder.Create##name(LC64(IMM), LGET(RS1)));

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
            ans = builder.CreateLShr(LGET(RS1), LC64(IMM&SIX_BITS));
        else
            ans = builder.CreateAShr(LGET(RS1), LC64(IMM&SIX_BITS));
        LSET(RD, ans);
}) // TODO check for correctness // WARNING decoded same as SRAI
_INSTR_(SLLIW, I, {SET_REG(RD, (uint32_t)REG(RS1) << (IMM & SIX_BITS))}, true, {
 LSET(RD, builder.CreateZExt( builder.CreateShl( builder.CreateTrunc( LGET(RS1), Type::getInt32PtrTy(ctx) ), LC64(IMM & SIX_BITS) ) , Type::getInt64PtrTy(ctx)));
}) // TODO check
_INSTR_(SRLIW, I, {SET_REG(RD,
                          !!(IMM & 1<<10)*((uint32_t)REG(RS1) >> (IMM & SIX_BITS)) // logical
                          |
                          !(IMM & 1<<10)*((int32_t)REG(RS1) >> (IMM & SIX_BITS)) // arithmetic
)}, true, {
        Value *ans=nullptr;
        if (IMM & 1<<10)
 ans = builder.CreateZExt( builder.CreateLShr( builder.CreateTrunc( LGET(RS1), Type::getInt32PtrTy(ctx)), LC64(IMM & SIX_BITS) ) , Type::getInt64PtrTy(ctx));
        else
 ans = builder.CreateZExt( builder.CreateAShr( builder.CreateTrunc( LGET(RS1), Type::getInt32PtrTy(ctx)), LC64(IMM & SIX_BITS) ) , Type::getInt64PtrTy(ctx));
        LSET(RD, ans);
    }) // TODO check for correctness // WARNING decoded same as SRAIW
_INSTR_(LUI, U, { SET_REG(RD, (IMM&INSN_FIELD_IMM20)) }, true, { LSET(RD, LC64(IMM&INSN_FIELD_IMM20)); })
_INSTR_(AUIPC, U,{ SET_REG(RD, (IMM + OPC)) }, true, {LSET(RD, builder.CreateAdd(LC64(IMM), LPC));}) 

#define JIT_BIN_OP(name) LSET(RD, builder.Create##name(LGET(RS1), LGET(RS2)));

_INSTR_(ADD, R, {CODE_BIN_RU(+)}, true, { JIT_BIN_OP(Add) })
_INSTR_(SLT, R, {CODE_BIN_RS(<)}, true, { JIT_BIN_OP(ICmpSLT) })
_INSTR_(SLTU, R, {CODE_BIN_RU(<)}, true, { JIT_BIN_OP(ICmpULT) })
_INSTR_(AND, R, {CODE_BIN_RU(&)}, true, { JIT_BIN_OP(And) })
_INSTR_(OR, R, {CODE_BIN_RU(|)}, true, { JIT_BIN_OP(Or) })
_INSTR_(XOR, R, {CODE_BIN_RU(^)}, true, { JIT_BIN_OP(Xor) })
_INSTR_(SLL, R, {SET_REG(RD, REG(RS1) << (REG(RS2) & SIX_BITS))}, true, { LSET(RD, builder.CreateShl(LGET(RS1), builder.CreateAnd(LGET(RS2), LC64(SIX_BITS)))) }) // TODO check
_INSTR_(SRL, R, {SET_REG(RD, (REG(RS1) >> (REG(RS2) & SIX_BITS)))}, true, { LSET(RD, builder.CreateLShr(LGET(RS1), builder.CreateAnd(LGET(RS2), LC64(SIX_BITS)))) }) // TODO check
_INSTR_(SRA, R, {SET_REG(RD, ((int64_t)REG(RS1) >> (REG(RS2) & SIX_BITS)))}, true, { LSET(RD, builder.CreateAShr(LGET(RS1), builder.CreateAnd(LGET(RS2), LC64(SIX_BITS)))) }) // TODO check
_INSTR_(SLLW, R, {SET_REG(RD, (uint32_t)REG(RS1) << (REG(RS2) & FIV_BITS))}, true, {

 LSET(RD, builder.CreateZExt( builder.CreateShl( builder.CreateTrunc( LGET(RS1), Type::getInt32PtrTy(ctx) ), builder.CreateAnd(LGET(RS2), LC64(FIV_BITS)) ) , Type::getInt64PtrTy(ctx)));
}) // TODO check
_INSTR_(SRLW, R, {SET_REG(RD, ((uint32_t)REG(RS1) >> (REG(RS2) & FIV_BITS)))}, true, {
 LSET(RD, builder.CreateZExt( builder.CreateLShr( builder.CreateTrunc( LGET(RS1), Type::getInt32PtrTy(ctx) ), builder.CreateAnd(LGET(RS2), LC64(FIV_BITS)) ) , Type::getInt64PtrTy(ctx)));
}) // TODO check
_INSTR_(SRAW, R, {SET_REG(RD, ((int32_t)REG(RS1) >> (REG(RS2) & FIV_BITS)))}, true, {
 LSET(RD, builder.CreateZExt( builder.CreateAShr( builder.CreateTrunc( LGET(RS1), Type::getInt32PtrTy(ctx) ), builder.CreateAnd(LGET(RS2), LC64(FIV_BITS)) ) , Type::getInt64PtrTy(ctx)));
}) // TODO check
_INSTR_(SUB, R, {CODE_BIN_RU(-)}, true, { JIT_BIN_OP(Sub) })
_INSTR_(SUBW, R, {SET_REG(RD, (int64_t)(int32_t)(REG(RS1) - REG(RS2)))}, true, {
 LSET(RD, builder.CreateSExt( builder.CreateSub( builder.CreateTrunc( LGET(RS1), Type::getInt32PtrTy(ctx) ), LGET(RS2)) , Type::getInt64PtrTy(ctx)));
}) // TODO check if it's correct but it should be nice
_INSTR_(ADDW, R, {SET_REG(RD, (int64_t)(int32_t)(REG(RS1) + REG(RS2)))}, true, {
 LSET(RD, builder.CreateSExt( builder.CreateAdd( builder.CreateTrunc( LGET(RS1), Type::getInt32PtrTy(ctx) ), LGET(RS2)) , Type::getInt64PtrTy(ctx)));
}) // TODO check if it's correct but it should be nice
_INSTR_(JAL, J, { NEW_PC = OPC + IMM; SET_REG(RD, OPC + 4); }, false, { builder.CreateStore(new_pc, builder.CreateAdd(LPC, LC64(IMM))); LSET(RD, builder.CreateAdd(LPC, LC64(4)));  })
_INSTR_(JALR, I, { NEW_PC = (IMM + REG(RS1)) & ~1ULL; SET_REG(RD, OPC + 4); }, false, { builder.CreateStore(new_pc, builder.CreateAnd( builder.CreateAdd(LGET(RS1), LC64(IMM)), LC64(~1ULL))); LSET(RD, builder.CreateAdd(LPC, LC64(4))); })

#define JIT_COND_JMP(name) BasicBlock *trueBr = BasicBlock::Create(ctx, "", fn); \
BasicBlock *retBr = BasicBlock::Create(ctx, "", fn); \
builder.CreateCondBr(builder.CreateICmp##name(LGET(RS1), LGET(RS2)), trueBr, retBr); \
builder.SetInsertPoint(trueBr); \
builder.CreateStore(builder.CreateAdd(LPC, LC64(IMM)), new_pc); \
builder.CreateBr(retBr); \
builder.SetInsertPoint(retBr);

_INSTR_(BEQ, B, {CODE_CJU(==)}, false, { JIT_COND_JMP(EQ) })
_INSTR_(BNE, B, {CODE_CJU(!=)}, false, { JIT_COND_JMP(NE) })
_INSTR_(BLT, B, {CODE_CJS(<)}, false, { JIT_COND_JMP(SLT) })
_INSTR_(BLTU, B, {CODE_CJU(<)}, false, { JIT_COND_JMP(ULT) })
_INSTR_(BGE, B, {CODE_CJS(>=)}, false, { JIT_COND_JMP(SGE) })
_INSTR_(BGEU, B, {CODE_CJU(>=)}, false, { JIT_COND_JMP(UGE) })
_INSTR_(ADDIW, I, {SET_REG(RD, (int64_t)(int32_t)(REG(RS1) + IMM))}, true, {
 LSET(RD, builder.CreateSExt( builder.CreateAdd( builder.CreateTrunc( LGET(RS1), Type::getInt32PtrTy(ctx) ), LC64(IMM)) , Type::getInt64PtrTy(ctx)));
}) // TODO check if it's correct but it should be nice
// TODO check type conversions
_INSTR_(LD, I, { SET_REG(RD, *(uint64_t*)MEM(REG(RS1)+IMM)); }, false, {
    LSET(RD, builder.CreateLoad(Type::getInt64PtrTy(ctx), builder.CreateGEP(Type::getInt8PtrTy(ctx), mem, builder.CreateAdd(LGET(RS1), LC64(IMM)))));
})

_INSTR_(LW, I, { SET_REG(RD, *(int32_t*)MEM(REG(RS1)+IMM)); }, false, {
    LSET(RD, builder.CreateSExt(
        builder.CreateLoad(Type::getInt32PtrTy(ctx), builder.CreateGEP(Type::getInt8PtrTy(ctx), mem, builder.CreateAdd(LGET(RS1), LC64(IMM)))),
        Type::getInt64PtrTy(ctx)
    ));
})
_INSTR_(LWU, I, { SET_REG(RD, *(uint32_t*)MEM(REG(RS1)+IMM)); }, false, {
    LSET(RD, builder.CreateZExt(
        builder.CreateLoad(Type::getInt32PtrTy(ctx), builder.CreateGEP(Type::getInt8PtrTy(ctx), mem, builder.CreateAdd(LGET(RS1), LC64(IMM)))),
        Type::getInt64PtrTy(ctx)
    ));
})
_INSTR_(LH, I, { SET_REG(RD, *(int16_t*)MEM(REG(RS1)+IMM)); }, false, {
    LSET(RD, builder.CreateSExt(
        builder.CreateLoad(Type::getInt16PtrTy(ctx), builder.CreateGEP(Type::getInt8PtrTy(ctx), mem, builder.CreateAdd(LGET(RS1), LC64(IMM)))),
        Type::getInt64PtrTy(ctx)
    ));
})
_INSTR_(LHU, I, { SET_REG(RD, *(uint16_t*)MEM(REG(RS1)+IMM)); }, false, {
    LSET(RD, builder.CreateZExt(
        builder.CreateLoad(Type::getInt16PtrTy(ctx), builder.CreateGEP(Type::getInt8PtrTy(ctx), mem, builder.CreateAdd(LGET(RS1), LC64(IMM)))),
        Type::getInt64PtrTy(ctx)
    ));
})
_INSTR_(LB, I, { SET_REG(RD, *(int8_t*)MEM(REG(RS1)+IMM)); }, false, {
    LSET(RD, builder.CreateSExt(
        builder.CreateLoad(Type::getInt8PtrTy(ctx), builder.CreateGEP(Type::getInt8PtrTy(ctx), mem, builder.CreateAdd(LGET(RS1), LC64(IMM)))),
        Type::getInt64PtrTy(ctx)
    ));
})
_INSTR_(LBU, I, { SET_REG(RD, *(uint8_t*)MEM(REG(RS1)+IMM)); }, false, {
    LSET(RD, builder.CreateZExt(
        builder.CreateLoad(Type::getInt8PtrTy(ctx), builder.CreateGEP(Type::getInt8PtrTy(ctx), mem, builder.CreateAdd(LGET(RS1), LC64(IMM)))),
        Type::getInt64PtrTy(ctx)
    ));
})
_INSTR_(SD, S, { *(uint64_t*)MEM(REG(RS1)+IMM) = REG(RS2); }, false, {
    builder.CreateStore(LGET(RS2), builder.CreateGEP(Type::getInt8PtrTy(ctx), mem, builder.CreateAdd(LGET(RS1), LC64(IMM))));
})
_INSTR_(SW, S, { *(uint32_t*)MEM(REG(RS1)+IMM) = REG(RS2); }, false, {
    builder.CreateStore( builder.CreateTrunc(LGET(RS2), Type::getInt32PtrTy(ctx)), builder.CreateGEP(Type::getInt8PtrTy(ctx), mem, builder.CreateAdd(LGET(RS1), LC64(IMM))));
})
_INSTR_(SH, S, { *(uint16_t*)MEM(REG(RS1)+IMM) = REG(RS2); }, false, {
    builder.CreateStore( builder.CreateTrunc(LGET(RS2), Type::getInt16PtrTy(ctx)), builder.CreateGEP(Type::getInt8PtrTy(ctx), mem, builder.CreateAdd(LGET(RS1), LC64(IMM))));
})
_INSTR_(SB, S, { *(uint8_t*)MEM(REG(RS1)+IMM) = REG(RS2); }, false, {
    builder.CreateStore( builder.CreateTrunc(LGET(RS2), Type::getInt8PtrTy(ctx)), builder.CreateGEP(Type::getInt8PtrTy(ctx), mem, builder.CreateAdd(LGET(RS1), LC64(IMM))));
})
_INSTR_(ECALL, I, {printf("Ecall or ebreak: doing exit...\n"); heart->done = true;}, false, { builder.CreateStore(builder.getInt1(true), done); }) // WARNING same as ebreak // TODO add actuall functionality
