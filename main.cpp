#include "heart.hpp"

int main() {
    Heart heart;

    // load 5 to x10
    heart.exec_instructions.push_back(0x00500513);
    // load 2 to x3
    heart.exec_instructions.push_back(0x00200193);
    // add x3 to x10 and put result in x17
    heart.exec_instructions.push_back(0x003508b3);

    heart.simulate();

    std::cout << "Register 10 value: " << heart.registers[10] << std::endl;
    std::cout << "Register 3 value: "  << heart.registers[3] << std::endl;
    std::cout << "Summing 5 + 2 value: " << heart.registers[17] << std::endl;

    return 0;
}