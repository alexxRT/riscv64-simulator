#include "hart.hpp"
#include "elf_reader.hpp"

#include <cstdint>
#include <vector>

int main() {
    Hart hart;
    ElfReader reader("sample_rv64");

    if (reader.load_instructions(hart) != ReaderStatus::SUCCESS) {
        return 1;
    }

    hart.simulate();

    return 0;
}
