#ifndef HEART_H
#define HEART_H

#include "instruction.hpp"
#include "functions.hpp"

#include <cstdint>
#include <array>

const int REGISTERS_NUM = 32;

enum class EXECUTE_STATUS : int {
    SUCCESS    = 0,
    BAD_ADDRES = 1
};

class Heart {
    public:
        uint32_t pc;
        uint32_t new_pc;
        std::array<reg_t, REGISTERS_NUM> registers;
        uint8_t *memory;

        Heart() : registers({}), pc(0), memory(nullptr) { }

        EXECUTE_STATUS simulate() {
            EXECUTE_STATUS status = EXECUTE_STATUS::SUCCESS;
            // execute
            while (true) {
                Instruction* decoded = decode(*(uint32_t*)(memory+pc));
                new_pc = pc + 4;
                if (!decoded)
                    return EXECUTE_STATUS::BAD_ADDRES;

                decoded->execute(this);

                pc = new_pc;
                if (status != EXECUTE_STATUS::SUCCESS)
                    return status;
            }
            return EXECUTE_STATUS::SUCCESS;
        }

        Instruction* decode(int instruction) {
            Instruction base(instruction);
            switch (base.op_code) {
                case InstrOpCode::OP:
                    return new InstructionR(instruction);
                case InstrOpCode::OP_IMM:
                    return new InstructionI(instruction);
                case InstrOpCode::BRANCH:
                    return new InstructionB(instruction);
                case InstrOpCode::LUI:
                    return new InstructionU(instruction);
                case InstrOpCode::JAL:
                    return new InstructionJ(instruction);
                case InstrOpCode::JALR:
                    return new InstructionU(instruction);
                case InstrOpCode::AUIPC:
                    return new InstructionU(instruction);
                default:
                    return nullptr;
            }
        }
};

#endif //HEART_H
