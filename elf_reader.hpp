#ifndef ELF_READER_H
#define ELF_READER_H

#include <elfio/elfio.hpp>
#include "heart.hpp"

enum class ReaderStatus : int {
    SUCCESS = 0,
    BAD_FILE = 1,
    BAD_SEGMENT = 2
};

class ElfReader {
    public:
        ElfReader(std::string filename): reader(), file_loaded_(false) {
            file_loaded_ = reader.load(filename);
        };

        ReaderStatus load_instructions(Heart& heart) {
            if (!file_loaded) {
                return ReaderStatus::BAD_FILE;
            }

            char* data = nullptr;
            uint64_t segment_start = 0, segment_end = 0;
            for (const auto& segment : reader.segments) {
                if (segment->get_type() == ELFIO::PT_LOAD && (segment->get_flags() & ELFIO::PF_X)) {
                    segment_start = segment->get_virtual_address();
                    segment_end = segment_start + segment->get_memory_size();

                    data = const_cast<char*>(segment->get_data());
                    break;
                }
            }

            for (const auto& section : reader.sections) {
                if (section->get_name() == ".text") {
                    uint64_t section_start = section->get_address();
                    uint64_t section_end = section_start + section->get_size();

                    if (data && section_start >= segment_start && section_end <= segment_end) {
                        heart.memory = reinterpret_cast<uint8_t*>(data);
                        heart.pc = section_start;

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

    private:
        bool file_loaded_;
}

#endif //ELF_READER_H