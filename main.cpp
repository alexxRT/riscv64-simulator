#include "heart.hpp"

#include <cstdint>
#include <vector>

int main() {
    std::vector<uint32_t> instrs;
    Heart heart;

    // load 5 to x10
    instrs.push_back(0x00500513);
    // load 2 to x3
    instrs.push_back(0x00200193);
    // add x3 to x10 and put result in x17
    instrs.push_back(0x003508b3);

    heart.memory = (uint8_t*)instrs.data();
    heart.simulate();

    std::cout << "Register 10 value: " << heart.registers[10] << std::endl;
    std::cout << "Register 3 value: "  << heart.registers[3] << std::endl;
    std::cout << "Summing 5 + 2 value: " << heart.registers[17] << std::endl;

    return 0;
}
