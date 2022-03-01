#include "ELFLoader.hpp"

// Macros used to make sure structs are packed to fit the MIPS instruction size

// GCC and Clang version
#if defined(__GNUC__) || defined(__clang__)
#    define PACKED(__Declaration__) __Declaration__ __attribute__((__packed__))
#endif

// MSVC version
#ifdef _MSC_VER
#    define PACKED(__Declaration__) \
        __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
#endif

// Error for unsupported compilers
#ifndef PACKED
#    error "Unsupported compiler"
#endif

PACKED(struct ELF32FileHeader { 
    static constexpr std::size_t EI_CLASS = 4;
    static constexpr std::size_t EI_DATA = 5;

    static constexpr uint8_t ELFCLASS32 = 1;

    uint8_t e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shstrndx;
});

PACKED(struct ELF32ProgramHeader {
    static constexpr uint32_t PT_LOAD = 1;

    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
});

ELFResult load_elf32(const uint8_t* elf_data,
                            Executable* executable) {
    const auto file_header =
        reinterpret_cast<const ELF32FileHeader*>(elf_data);

    // Validate magic numbers in file header
    {
        const uint8_t magic[4] = {0x7f, 'E', 'L', 'F'};

        for (int i = 0; i < 4; ++i) {
            if (magic[i] != file_header->e_ident[i]) {
                return ELFResult::invalid_magic;
            }
        }
    }

    // Make sure executable is 32 bits
    if (file_header->e_ident[ELF32FileHeader::EI_CLASS] !=
        ELF32FileHeader::ELFCLASS32) {
        return ELFResult::not_32_bits;
    }

    // TODO: Handle endianess
    if (file_header->e_ident[ELF32FileHeader::EI_DATA] != 1) {
        return ELFResult::big_endian;
    }

    const auto program_headers =
        reinterpret_cast<const ELF32ProgramHeader*>(elf_data +
                                                    file_header->e_phoff);

    uint32_t low_addr = ~((uint32_t)0);
    uint32_t high_addr = 0;

    std::size_t loaded_segments = 0;

    for (int i = 0; i < file_header->e_phnum; ++i) {
        const auto header = &program_headers[i];

        if (header->p_type != ELF32ProgramHeader::PT_LOAD) continue;
        ++loaded_segments;

        if (header->p_vaddr < low_addr) {
            low_addr = header->p_vaddr;
        }

        if (header->p_vaddr + header->p_memsz > high_addr) {
            high_addr = header->p_vaddr + header->p_memsz;
        }
    }

    // No loadable segments were found
    if (loaded_segments == 0) return ELFResult::no_segments;

    std::vector<uint8_t> executable_data(high_addr);
    for (int i = 0; i < file_header->e_phnum; ++i) {
        const auto header = &program_headers[i];

        // Check if segment is loadable
        if (header->p_type != ELF32ProgramHeader::PT_LOAD) continue;

        // File size and memory size can differ,
        // there is no reason to make a copy if the file size is 0
        if (header->p_filesz > 0) {
            memcpy(&executable_data[header->p_vaddr],
                   elf_data + header->p_offset, header->p_filesz);
        }

        // Zero initialize remaining memory according to ELF spec
        memset(&executable_data[header->p_vaddr + header->p_filesz], 0,
               header->p_memsz - header->p_filesz);
    }

    *executable =
        Executable{low_addr, file_header->e_entry,
                   Executable::Endianess::little, executable_data};

    return ELFResult::ok;
}
