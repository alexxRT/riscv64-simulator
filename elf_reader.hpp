#ifndef ELF_READER_H
#define ELF_READER_H

#include <iostream>
#include <elfio/elfio.hpp>
#include "heart.hpp"

enum class ReaderStatus : int {
    SUCCESS = 0,
    BAD_FILE = 1,
    BAD_SEGMENT = 2
};

class ElfReader {
    public:
        ElfReader(std::string filename): reader(), file_loaded(false) {
            file_loaded = reader.load(filename);
        };

        ReaderStatus load_instructions(Heart& heart) {
            if (!file_loaded) {
                return ReaderStatus::BAD_FILE;
            }

            uint64_t segment_start = 0, segment_end = 0;
            for (const auto& segment : reader.segments) {
                if (segment->get_type() == ELFIO::PT_LOAD && (segment->get_flags() & ELFIO::PF_X)) {
                    segment_start = segment->get_virtual_address();
                    segment_end = segment_start + segment->get_memory_size();
                    break;
                }
            }

            for (const auto& section : reader.sections) {
                if (section->get_name() == ".text") {
                    uint64_t section_start = section->get_address();
                    uint64_t section_end = section_start + section->get_size();

                    if (section_start >= segment_start && section_end <= segment_end) {
                        size_t size = section->get_size();
                        const char* data = section->get_data();

                        heart.pc = heart.exec_instructions.size();
                        heart.exec_instructions.resize(size);
                        const int* instructions = reinterpret_cast<const int*>(data);
                        for (size_t i = 0; i < size; ++i) {
                            heart.exec_instructions.push_back(instructions[i]);
                        }

                        break;
                    }

                    return ReaderStatus::BAD_SEGMENT;
                }
            }

            return ReaderStatus::SUCCESS;
        }

        bool file_loaded() {
            return file_loaded;
        }

        ELFIO::elfio reader;

    private:
        bool file_loaded;
}

#endif //ELF_READER_H
