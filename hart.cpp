#include "hart.hpp"
#include "instruction.hpp"
#include "basic_block.hpp"

EXECUTE_STATUS Hart::simulate() {
    EXECUTE_STATUS status = EXECUTE_STATUS::SUCCESS;
    // execute
    while (!done) {
        DEB("decoding at pc=" << pc);
//        std::cout << "dec at pc= "  << pc << '\n';
        RVBasicBlock &bb = bbs_arr[(pc >> 2) & BB_arr_mask];
        if (bb.addr == pc) {
            bb.jitted(this->registers.data(), this->memory, &this->pc, &this->done);
//            bb.instrs[0].execute(this, bb.instrs[0]);
//            ins_cnt += bb.len;
        }
        else {
            std::cout << "jitting\n";
            bb.do_jit((instT*)(memory+pc));
            std::cout << "jitted\n";
//            bb.construct((instT*)(memory+pc));
            bb.addr = pc;
            bb.jitted(this->registers.data(), this->memory, &this->pc, &this->done);
            std::cout << pc << "runned\n";
//            bb.instrs[0].execute(this, bb.instrs[0]);
            ins_cnt += bb.len;
        }
    }
    return EXECUTE_STATUS::SUCCESS;
}
