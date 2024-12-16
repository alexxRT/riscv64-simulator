#ifndef HART_H
#define HART_H

#include "instruction.hpp"
#include "mask.h"

#include <cstdint>
#include <array>

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

namespace DecodeRetNS {
enum DecodeRet {
    OK = 0,
    EXIT = 1,
    ERROR = 2,
};
};

class Hart {
public:
    ssize_t pc;
    ssize_t new_pc;
    std::array<regT, REGISTERS_NUM> registers;
    uint8_t *memory;

    Hart() : registers({}), pc(0), memory(nullptr) { }

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
            auto dec_s = decode(*(uint32_t*)(memory+pc), decoded);
            DEB("decoded");
            if (dec_s == DecodeRetNS::EXIT) {
                DEB("finished")
                return EXECUTE_STATUS::SUCCESS;
            }
            if (dec_s) {
                std::cerr << "not decoded:(\n";
                printf("%x\n", *(uint32_t*)(memory+pc));
                return EXECUTE_STATUS::BAD_ADDRES;
            }

            new_pc = pc + 4;
            decoded->execute(this, *decoded);
            pc = new_pc;
            if (status != EXECUTE_STATUS::SUCCESS)
                return status;
        }
        delete decoded;
        return EXECUTE_STATUS::SUCCESS;
    }

    DecodeRetNS::DecodeRet decode(uint32_t instruction, Instruction *ptr) {
        using namespace DecodeRetNS;
        // TODO kostyl instead of syscall
        if (instruction == 0xFFFFFFFF) {
            DEB("exit");
            return EXIT;
        }

        uint32_t fingerprint = instruction & mask[instruction & 127];
        fingerprint = FP_HASH(fingerprint);

        auto dec = decoders[fingerprint];
        dec.decod(*ptr, instruction);
        ptr->execute = dec.exec;
        return OK;
    }
};

#endif //HART_H
