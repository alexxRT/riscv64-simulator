#ifndef HEART_H
#define HEART_H

#include "instruction.hpp"
#include "functions.hpp"
#include <vector>
#include <map>

const int REGISTERS_NUM = 32;

enum class EXECUTE_STATUS : int {
    SUCCESS    = 0,
    BAD_ADDRES = 1
};

class Heart {
    public:
        Heart() : registers(REGISTERS_NUM), pc(0) {
            Instruction::build_functions_map(functions);
        }

        EXECUTE_STATUS simulate() {
            EXECUTE_STATUS status = EXECUTE_STATUS::SUCCESS;
            // execute
            for (auto& instr : exec_instructions) {
                Instruction* decoded = decode(instr);
                if (!decoded)
                    return EXECUTE_STATUS::BAD_ADDRES;

                decoded->execute(this);

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

        void clear_instructions() {
            exec_instructions.clear();
            return;
        }

        reg_t pc;
        std::vector<reg_t> registers;
        std::map<uint32_t, function_t> functions;
        std::vector<int> exec_instructions;
};

#endif //HEART_H