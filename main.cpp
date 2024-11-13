#include "heart.hpp"
#include "elf_reader.hpp"

#include <cstdint>
#include <vector>

int main() {
    std::vector<uint32_t> instrs;
    Heart heart;
    ElfReader reader("sample_rv64");

    // load 5 to x10
    instrs.push_back(0x00500513);
    // load 2 to x3
    instrs.push_back(0x00200193);
    // add x3 to x10 and put result in x17
    instrs.push_back(0x003508b3);
    instrs.push_back(0xFFFFFFFF);

    // fibonacci
    /*
_start:
    addi x7, x0, 2
    addi x1, x0, 10
    addi x2, x0, 0
    addi x3, x0, 1
lop:
    add x4, x2, x3
    add x2, x3, x0
    add x3, x4, x0
    addi x7, x7, 1
    blt x7, x1, lop
// answer is in x3
*/

    std::vector<uint32_t> fib = {
    0x00200393,
        0x00a00093,
        0x00000113,
        0x00100193,
        // lop
        0x00310233,
        0x00018133,
        0x000201b3,
        0x00138393,
        0xfe13c8e3,
        0xFFFFFFFF
    };

    heart.memory = (uint8_t*)fib.data();
    heart.simulate();
    std::cout << "fib(10): " << heart.registers[3] << std::endl;

    std::cout << "\n\n\nELF\n\n\n";

    std::cout << "elf reader:\n";
    if (reader.load_instructions(heart) != ReaderStatus::SUCCESS) {
        return 1;
    }
    heart.simulate();
    exit(0);
    std::vector<uint32_t> elfsim = {
        0x00a00513,
        0x01400593,
        0x00b50633,
        0x40a586b3,
        0xFFFFFFFF
    };

    Heart heart2;
    heart2.new_pc = 0;
    heart2.pc = 0;
    heart2.memory = (uint8_t*)elfsim.data();
    heart2.simulate();
    //    heart.simulate();
    std::cout << heart2.registers[10] << ' ' << heart2.registers[11] << ' ' << heart2.registers[12] << ' ' << heart2.registers[13] << '\n';


   // std::cout << "Register 10 value: " << heart.registers[10] << std::endl;
   // std::cout << "Register 3 value: "  << heart.registers[3] << std::endl;

    return 0;
}
