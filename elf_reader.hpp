#ifndef ELF_READER_H
#define ELF_READER_H

#include <elfio/elfio.hpp>
#include "hart.hpp"

enum class ReaderStatus : int {
    SUCCESS = 0,
    BAD_FILE = 1,
    BAD_SEGMENT = 2
};

class ElfReader {
    public:
        ElfReader(std::string filename): reader(), file_loaded_(false) {
            file_loaded_ = reader.load(filename);
            vmem = new uint8_t[1024*1024*1024];
        };

        ~ElfReader() {
            delete[] vmem;
        }

        ReaderStatus load_instructions(Hart& hart) {
            if (!file_loaded_) {
                return ReaderStatus::BAD_FILE;
            }

            uint64_t segment_start = 0, segment_end = 0;
            for (const auto& segment : reader.segments) {
                if (segment->get_type() == ELFIO::PT_LOAD && (segment->get_flags() & ELFIO::PF_X)) {
                    size_t msize = segment->get_memory_size();

                    segment_start = segment->get_virtual_address();
                    segment_end = segment_start + msize;

                    std::memcpy(&vmem[segment_start], segment->get_data(), msize);

                    break;
                }
            }

            for (const auto& section : reader.sections) {
                if (section->get_name() == ".text") {
                    uint64_t section_start = section->get_address();
                    uint64_t section_end = section_start + section->get_size();

                    if (vmem && section_start >= segment_start && section_end <= segment_end) {
                        hart.memory = vmem;
                        hart.pc = section_start;

                        return ReaderStatus::SUCCESS;
                    }
                }
            }

            return ReaderStatus::BAD_SEGMENT;
        }

        bool file_loaded() {
            return file_loaded_;
        }

        ELFIO::elfio reader;
        uint8_t* vmem;

    private:
        bool file_loaded_;
};

#endif //ELF_READER_H
