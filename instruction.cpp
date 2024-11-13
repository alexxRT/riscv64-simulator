#include "instruction.hpp"
#include "hart.hpp"

namespace Executors {
void empty_executor(Hart *hart, const Instruction &instr) {} // example


#define _INSTR_(name, type, code) \
void exec_##name(Hart *heart, const Instruction &instr) { \
    code }

#include "instrs.h"
#undef _INSTR_

};

