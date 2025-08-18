#include <cstdint>

#include "encoding.out.h"
#include "mask.hpp"

uint32_t mask[128];

Decode decoders[(1<<18)-1];

void fill_arrays() {
#define _INSTR_(name, ...) mask[(MATCH_##name & 127)] = MASK_##name;
#include "instrs.h"
#undef _INSTR_

#define _INSTR_(name, type, code, linear, jit) decoders[FP_HASH(MATCH_##name)] = {Executors::exec_##name, decode_instruction_##type, linear, Jiters::jit_##name};
#include "instrs.h"
#undef _INSTR_

}

