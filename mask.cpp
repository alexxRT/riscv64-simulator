#include <cstdint>

#include "encoding.out.h"
#include "mask.hpp"

uint32_t mask[128] = {
#define _INSTR_(name, ...) [(MATCH_##name & 127)] = MASK_##name,
#include "instrs.h"
#undef _INSTR_
};


Decode decoders[(1<<18)-1] = {
#define _INSTR_(name, type, code, linear) [FP_HASH(MATCH_##name)] = {Executors::exec_##name, decode_instruction_##type, linear},
#include "instrs.h"
#undef _INSTR_
};

