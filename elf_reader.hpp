#ifndef ELF_READER_H
#define ELF_READER_H

#include <elfio/elfio.hpp>
#include "hart.hpp"

enum class ReaderStatus : int {
    SUCCESS = 0,
    BAD_FILE = 1,
    BAD_SEGMENT = 2,
    BAD_SECTION = 3,
    BAD_ALLOC = 4
};

const size_t process_stack_size = 8 * 1024 * 1024;

const size_t sp_reg_number = 2;
const size_t ra_reg_number = 1;

class ElfReader {
    public:
        ElfReader(std::string filename): reader(), vmem_(nullptr), file_loaded_(false), max_vaddr_(0) {
            file_loaded_ = reader.load(filename);
        };

        ~ElfReader() {
            delete[] vmem_;
        }

        ReaderStatus load_instructions(Hart& hart) {
            if (!file_loaded_) {
                return ReaderStatus::BAD_FILE;
            }

            for (const auto& segment : reader.segments) {
                if (segment->get_type() == ELFIO::PT_LOAD) {

                    uint64_t segment_end = segment->get_virtual_address() + segment->get_memory_size();
                    max_vaddr_ = std::max(max_vaddr_, segment_end);
                }
            }
            if (!max_vaddr_) {
                return ReaderStatus::BAD_SEGMENT;
            }

            uint64_t vmem_stack_addr = max_vaddr_ + 1 + process_stack_size;
            uint64_t vmem_fin_ecall_addr = vmem_stack_addr + 1;
            uint64_t vmem_max_addr = vmem_fin_ecall_addr + 4; // for exit syscall
            size_t vmem_size = vmem_max_addr + 1;

            vmem_ = new uint8_t[vmem_size];
            if (!vmem_) {
                return ReaderStatus::BAD_ALLOC;
            }

            for (const auto& segment : reader.segments) {
                if (segment->get_type() == ELFIO::PT_LOAD) {

                    uint64_t segment_start = segment->get_virtual_address();
                    size_t segment_size = segment->get_memory_size();
                    const char* data = segment->get_data();

                    if (data) {
                        std::memcpy(&(vmem_[segment_start]), data, segment_size);
                    }
                }
            }

            hart.pc = reader.get_entry();
 
            *(uint32_t*)(vmem_ + vmem_fin_ecall_addr) = 0x00000073; // ecall

            hart.memory = vmem_;
            hart.registers[sp_reg_number] = vmem_stack_addr;
            hart.registers[ra_reg_number] = vmem_fin_ecall_addr;

            return ReaderStatus::SUCCESS;
        }

        bool file_loaded() { return file_loaded_; }

        uint8_t* vmem() { return vmem_; }

        uint64_t max_vaddr() { return max_vaddr_; }

        ELFIO::elfio reader;

    private:
        uint8_t* vmem_;
        bool file_loaded_;
        uint64_t max_vaddr_;

};

#endif //ELF_READER_H
