#include "hart.hpp"
#include "instruction.hpp"

EXECUTE_STATUS Hart::simulate() {
    EXECUTE_STATUS status = EXECUTE_STATUS::SUCCESS;
    // execute
    while (!done) {
        DEB("decoding at pc=" << pc);
        BasicBlock &bb = bbs_arr[(pc >> 2) & BB_arr_mask];
        if (bb.addr == pc)
            bb.instrs[0].execute(this, bb.instrs[0]);
        else {
            bb.construct((instT*)(memory+pc));
            bb.addr = pc;
            bb.instrs[0].execute(this, bb.instrs[0]);
        }
    }
    return EXECUTE_STATUS::SUCCESS;
}
