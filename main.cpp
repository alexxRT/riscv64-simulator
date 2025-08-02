//#define DEBUG

#include "hart.hpp"
#include "basic_block.hpp"
#include "elf_reader.hpp"

#include <cstdint>
#include <vector>

#include <chrono>

template<typename T1, typename T2>
bool check_states(T1 &sim1, T2 &sim2) {
    static uint32_t regs1[32];
    static uint32_t regs2[32];

    for (int i = 0; i < 32; i++) {
        if (regs1[i] != (uint32_t)sim1.registers[i]) {
            #ifdef DEBUG
            std::cout << "RegChange of sim1's x" << i << " "
                      << regs1[i] << " to " << (uint32_t)sim1.registers[i] << '\n';
            #endif
            regs1[i] = sim1.registers[i];
        }

        if ((uint32_t)sim1.registers[i] != (uint32_t)sim2.registers[i]) {
            std::cerr << "reg " << i << " has different values!\n";
            std::cerr << std::hex << sim1.registers[i] << ' ' << sim2.registers[i] << '\n';
            return false;
        }
    }
    return true;
}

template<typename T1, typename T2>
bool cosim_sims(T1 &sim1, T2 &sim2) {
    int cnt = 0;
    bool cosim_ok = true;

    do { // TODO: check infinite loop
        sim1.exec_instr();
        sim2.exec_instr();
    } while (!sim1.done and !sim2.done and (cosim_ok = check_states(sim1, sim2)));

    if (sim1.done != sim2.done) {
        std::cerr << "done has different states!\n";
        cosim_ok = false;
    }

    if (cosim_ok)
        std::cout << "Cosim is ALL GOOD!\n";
    else
        std::cout << "Cosim failed :(\n";
    return cosim_ok;
}


bool test_fib_imm() {
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

    DEB("starting fib")
    Hart hartj(true);
    DEB("innited jit")
    Hart hartb(false);
    DEB("innited bb")
    std::vector<uint32_t> fib = {
        0x00000000,
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
        0x00000073 // ecall
    };

    hartj.memory = (uint8_t*)fib.data();
    hartj.pc = 4;
    hartb.memory = (uint8_t*)fib.data();
    hartb.pc = 4;

    cosim_sims(hartb, hartj);
    std::cout << "fib(9): " << hartj.registers[3] << " (34=>ok)\n";
    if (hartj.registers[3] != 34) {
        std::cerr << "Fibonacci test from array was not passed!!!\n";
        return false;
    }
    return true;
}

bool test_elf_reader() {
    Hart hartj(true);
    Hart hartb(false);

    ElfReader reader("build/sample_rv64");
    ReaderStatus read_st = reader.load_instructions(hartb);
    ReaderStatus read_st2 = reader.load_instructions(hartj);
    if (read_st != ReaderStatus::SUCCESS or read_st2 != ReaderStatus::SUCCESS) {
        std::cout << "failed to load instrs, ELF LOAD test failed :(\n";
        std::cout << "load err: " << int(read_st) << '\n';
        return false;
    }

    cosim_sims(hartb, hartj);
    std::cout << (hartb.registers[20] )
        << (hartb.registers[11])
        << (hartb.registers[12] )
        << (hartb.registers[13] ) << '\n';
    bool status = (hartb.registers[20] == 10)
        and (hartb.registers[11] == 20)
        and (hartb.registers[12] == 30)
        and (hartb.registers[13] == 10);
    if (!status) {
        std::cerr << "read elf test was not passed!!!\n";
        return false;
    }
    return true;
}

void run_8q() {
    Hart hartj(true);
    Hart hartb(false);

    ElfReader reader("build/8queens");
    ReaderStatus read_st = reader.load_instructions(hartb);
    ReaderStatus read_st2 = reader.load_instructions(hartj);
    if (read_st != ReaderStatus::SUCCESS) {
        std::cout << "failed to load instrs, ELF LOAD test failed :(\n";
        std::cout << "load err: " << int(read_st) << '\n';
        return;
    }

    auto start = std::chrono::steady_clock::now();

    cosim_sims(hartb, hartj);

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "Time taken: " << elapsed.count() << " ms" << std::endl;
    std::cout << "Intructions executed: " << hartb.ins_cnt << std::endl;
    std::cout << "Performance total: " << (hartb.ins_cnt / elapsed.count()) / 1e3 << " Mips" << std::endl;
}

int main() {
    DEB("starting")
    RVBasicBlock::init();
    DEB("innited")
    fill_arrays();
    DEB("filled")

    if (test_fib_imm() and test_elf_reader())
        std::cout << "tests are OK!\n";
    else {
        std::cout << "tests are bad :(\n";
        return 1;
    }

    run_8q();

    return 0;
}
