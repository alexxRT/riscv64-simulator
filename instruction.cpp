#include "instruction.hpp"

namespace Executors {
void empty_executor(Heart *heart, const Instruction &instr) {} // example


#define _INSTR_(name, type, code) \
void exec_##name(Heart *heart, const Instruction &instr) { \
    code }

#include "instrs.h"
#undef _INSTR_

};

