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

            vmem_ = new uint8_t[max_vaddr_ + 1];
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
                    } else { // e.g. .sbss section
                        std::memset(&(vmem_[segment_start]), 0, segment_size);
                    }
                }
            }

            hart.pc = reader.get_entry();

            hart.memory = vmem_;
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
