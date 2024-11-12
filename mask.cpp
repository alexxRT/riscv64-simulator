#include <cstdint>
#include "encoding.out.h"
uint32_t mask[128] = {
#define _INSTR_(name, ...) [(MATCH_##name & 127)] = MASK_##name,
#include "instrs.h"
#undef _INSTR_
};
