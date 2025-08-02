#ifndef HART_H
#define HART_H

#include "instruction.hpp"
#include "mask.hpp"
#include "basic_block.hpp"

#include <cstdint>
#include <array>

//#define DEBUG
#ifdef DEBUG
#define DEB(x) std::cout << x << '\n';
#else 
#define DEB(x) 
#endif

#define fprintf(...)

const int REGISTERS_NUM = 32;

enum class EXECUTE_STATUS : int {
    SUCCESS    = 0,
    BAD_ADDRES = 1
};

namespace DecodeRetNS {
enum DecodeRet {
    OK = 0,
    EXIT = 1,
    ERROR = 2,
};
};

class Hart {
public:
    size_t pc;
    size_t new_pc;
    size_t ins_cnt;
    std::array<regT, REGISTERS_NUM> registers;
    uint8_t *memory;
    bool done;
    bool use_jit;

    RVBasicBlock *bbs_arr;

    Hart(bool use_jit_)
        : registers({}),
          pc(0),
          memory(nullptr),
          done(false),
          ins_cnt(0),
          use_jit(use_jit_),
          bbs_arr(new RVBasicBlock[BB_arr_mask+1]()){}

    ~Hart() {
        delete[] bbs_arr;
    }

    uint64_t get_reg(int ind) {
        return registers[ind];
    }

    void set_reg(int ind, uint64_t value) {
        registers[ind] = value * !!ind;
    }

    EXECUTE_STATUS simulate();

    void exec_instr();

    void bb_run_instr();

    void jit_run_instr();
};

#endif //HART_H
