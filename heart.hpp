#ifndef HEART_H
#define HEART_H

#include "instruction.hpp"
#include "mask.h"

#include <cstdint>
#include <array>

#define DEBUG
#ifdef DEBUG
#define DEB(x) std::cout << x << '\n';
#else 
#define DEB(x) 
#endif

const int REGISTERS_NUM = 32;

enum class EXECUTE_STATUS : int {
    SUCCESS    = 0,
    BAD_ADDRES = 1
};

class Heart {
public:
    ssize_t pc;
    ssize_t new_pc;
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
            DEB("decoding at pc=" << pc);
            bool ok = decode(*(uint32_t*)(memory+pc), decoded);
            DEB("decoded");
            new_pc = pc + 4;
            if (!ok) {
               std:std::cerr << "not decoded:(\n";
                return EXECUTE_STATUS::BAD_ADDRES;
            }
            decoded->execute(this, *decoded);

            pc = new_pc;
            if (status != EXECUTE_STATUS::SUCCESS)
                return status;
        }
        delete decoded;
        return EXECUTE_STATUS::SUCCESS;
    }

    bool decode(uint32_t instruction, Instruction *ptr) {
        uint32_t fingerprint = instruction & mask[instruction & 127];
        switch (fingerprint) {
        #define _INSTR_(name, type, code) \
            case MATCH_##name: \
            DEB("decoded: " #name); \
            new(ptr) Instruction##type(instruction, Executors::exec_##name); \
            DEB("RS1 RS2 RD IMM: " << (int)ptr->rs1 << ' ' << (int)ptr->rs2 << ' ' << (int)ptr->rd << ' ' << (int)ptr->imm) \
            return true;
        #include "instrs.h"
        #undef _INSTR_
        case 0xFFFFFFFF: 
            DEB("exit");
            return false;
        }
        return false;
    }
};

#endif //HEART_H
