#include "ELFLoader.hpp"
#include <cstring>

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
    uint16_t shnum;
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

PACKED(struct ELF32SectionHeader {
    static constexpr uint32_t SHT_SYMTAB = 2;
    static constexpr uint32_t SHT_STRTAB = 3;
    
    uint32_t sh_name;
    uint32_t sh_type;
    uint32_t sh_flags;
    uint32_t sh_addr;
    uint32_t sh_offset;
    uint32_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint32_t sh_addralign;
    uint32_t sh_entsize;
});

PACKED(struct ELF32SymbolEntry {
    static constexpr uint32_t STB_GLOBAL = 1;
  
    uint32_t st_name;
    uint32_t st_value;
    uint32_t st_size;
    uint8_t st_info;
    uint8_t st_other;
    uint16_t st_shndx;
});

ELFResult LoadELF32(const uint8_t* iELFData, const char* const* const iSymbolNames, size_t iSymbolNameCount, Executable& oExecutable) {
    const auto file_header = reinterpret_cast<const ELF32FileHeader*>(iELFData);

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

    const auto program_headers = reinterpret_cast<const ELF32ProgramHeader*>(
        iELFData + file_header->e_phoff);

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
                   iELFData + header->p_offset, header->p_filesz);
        }

        // Zero initialize remaining memory according to ELF spec
        memset(&executable_data[header->p_vaddr + header->p_filesz], 0,
               header->p_memsz - header->p_filesz);
    }

    const auto section_headers = reinterpret_cast<const ELF32SectionHeader*>(
        iELFData + file_header->e_shoff);

    const ELF32SectionHeader* symtab_header = nullptr;
    const ELF32SectionHeader* strtab_header = nullptr;

    // Look through sections for symbol table and string table
    for (int i = 0; i < file_header->shnum; ++i) {
        const auto header = &section_headers[i];

	switch(header->sh_type) {
	case ELF32SectionHeader::SHT_SYMTAB:
            symtab_header = header;
	    break;
	case ELF32SectionHeader::SHT_STRTAB:
            strtab_header = header;
	    break;
	} 
    }

    // Return error if symbol or string table is missing
    if (symtab_header == nullptr) return ELFResult::no_symbol_table;
    if (strtab_header == nullptr) return ELFResult::no_string_table;

    const auto strtab =
        reinterpret_cast<const char*>(iELFData + strtab_header->sh_offset);

    std::vector<uint32_t> symbols(iSymbolNameCount, 0);
    size_t found_symbols = 0;
    for (int i = 0; i < symtab_header->sh_size;
         i += symtab_header->sh_entsize) {
        const auto entry = reinterpret_cast<const ELF32SymbolEntry*>(
            iELFData + symtab_header->sh_offset + i);

	// Only look for global symbols
        if ((entry->st_info >> 4) != ELF32SymbolEntry::STB_GLOBAL) continue;

	// Iterate through the requested symbol names
	for(int j = 0; j <= iSymbolNameCount; ++j) {
	  // If symbol was already found look for next symbol
	  if (symbols[j] != 0) continue;

	  // Check for matching symbol name
	  if (std::strcmp(iSymbolNames[j], &strtab[entry->st_name]) == 0) {
	    symbols[j] = entry->st_value;

	    // Exit early if all symbols have been found
	    if(++found_symbols == iSymbolNameCount) break;
	  } 
	}

	// If all symbols have been found, break out of the loop
	if(found_symbols == iSymbolNameCount) break;
    }

    // Return error if not all symbols were found
    if (found_symbols != iSymbolNameCount) return ELFResult::missing_symbols;
 
    oExecutable = Executable{low_addr, file_header->e_entry,
                             Executable::Endianess::little, symbols, executable_data};

    return ELFResult::ok;
}
