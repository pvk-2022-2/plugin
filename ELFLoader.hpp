#pragma once
#include <cstdint>
#include <cstring>
#include <vector>

enum class ELFResult : uint8_t {
    ok,
    invalid_magic,
    not_32_bits,
    big_endian,
    no_segments,
};

struct Executable {
    enum class Endianess {
        big,
        little,
    };

    uint32_t offset;
    uint32_t entry;
    Endianess endianess;
    std::vector<uint8_t> executable;
};

ELFResult load_elf32(const uint8_t* elf_data, Executable* executable);
