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
    no_symbol_table,
    no_string_table,
    missing_symbols,
};

struct Executable {
    enum class Endianess {
        big,
        little,
    };

    uint32_t offset;
    uint32_t entry;
    Endianess endianess;
    std::vector<uint32_t> symbols;
    std::vector<uint8_t> executable;
};

ELFResult LoadELF32(const uint8_t* iELFData, const char* const* const iSymbolNames, size_t iSymbolNameCount, Executable& oExecutable);
