#ifndef HEART_H
#define HEART_H

#include "instruction.hpp"
#include "mask.h"

#include <cstdint>
#include <array>

const int REGISTERS_NUM = 32;

enum class EXECUTE_STATUS : int {
    SUCCESS    = 0,
    BAD_ADDRES = 1
};

class Heart {
public:
    uint64_t pc;
    uint64_t new_pc;
    std::array<regT, REGISTERS_NUM> registers;
    uint8_t *memory;

    Heart() : registers({}), pc(0), memory(nullptr) { }

    uint64_t get_reg(int ind) {
        return registers[ind];
    }

    void set_reg(int ind, uint64_t value) {
        registers[ind] = value * !!ind;
    }

    EXECUTE_STATUS simulate() {
        EXECUTE_STATUS status = EXECUTE_STATUS::SUCCESS;
        // execute
        Instruction *decoded = new Instruction(0, nullptr);
        while (true) {
            decode(*(uint32_t*)(memory+pc), decoded);
            new_pc = pc + 4;
            if (!decoded)
                return EXECUTE_STATUS::BAD_ADDRES;

            decoded->execute(this, *decoded);

            pc = new_pc;
            if (status != EXECUTE_STATUS::SUCCESS)
                return status;
        }
        delete decoded;
        return EXECUTE_STATUS::SUCCESS;
    }

    Instruction* decode(uint32_t instruction, Instruction *ptr) {
        uint32_t fingerprint = instruction & mask[instruction & 127];
        switch (fingerprint) {
        #define _INSTR_(name, type, code) case MATCH_##name: return new(ptr) Instruction##type(instruction, Executors::exec_##name);
        #include "instrs.h"
        #undef _INSTR_
        }
        return nullptr;
    }
};

#endif //HEART_H
