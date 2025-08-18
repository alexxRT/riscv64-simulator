#include "hart.hpp"
#include <iostream>
#include "instruction.hpp"
#include "basic_block.hpp"
#include "jit.hpp"

Hart::Hart(bool use_jit_)
    : registers({}),
      pc(0),
      memory(nullptr),
      done(false),
      ins_cnt(0),
      use_jit(use_jit_),
      jit_arr(nullptr),
      bbs_arr(nullptr) {
    if (use_jit)
        jit_arr = new RVJitBlock[Jit_arr_mask + 1]();
    else
        bbs_arr = new RVBasicBlock[BB_arr_mask + 1]();
}

EXECUTE_STATUS Hart::simulate() {
    EXECUTE_STATUS status = EXECUTE_STATUS::SUCCESS;
    while (!done)
        exec_instr();
    return EXECUTE_STATUS::SUCCESS;
}

void Hart::exec_instr() {
    DEB("decoding at pc=" << pc);
    if (use_jit)
        jit_run_instr();
    else
        bb_run_instr();
}

void Hart::bb_run_instr() {
    DEB("execbb");
    RVBasicBlock &bb = bbs_arr[(pc >> 2) & BB_arr_mask];
    if (bb.addr == pc) {
        bb.instrs[0].execute(this, bb.instrs[0]);
        ins_cnt += bb.len;
    }
    else {
        bb.construct((instT*)(memory+pc));
        bb.addr = pc;
        bb.instrs[0].execute(this, bb.instrs[0]);
        ins_cnt += bb.len;
    }
}

void Hart::jit_run_instr() {
    DEB("execjit");
    RVJitBlock &bb = jit_arr[(pc >> 2) & Jit_arr_mask];
    DEB("decoding at pc=" << pc << " bb.addr=" << bb.addr);
    if (bb.addr == pc) {
        DEB("already jitted")
        bb.jitted(this->registers.data(), this->memory, &this->pc, &this->done);
        ins_cnt += bb.len;
    } else {
        DEB("jitting");
        bb.do_jit((instT *)(memory + pc));
        DEB("jitted");
        bb.addr = pc;
        bb.jitted(this->registers.data(), this->memory, &this->pc, &this->done);
        DEB(pc << "runned");
        ins_cnt += bb.len;
    }
}
