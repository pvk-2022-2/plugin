#pragma once
#include <stdint.h>

#define MMIO_MASK 0xFF000000U
#define MMIO_DEVICE(address) (((address) & ~MMIO_MASK) >> 8)
#define MMIO_INDEX(address) (((address) & ~MMIO_MASK) >> 4)

// Write only MMIO
#define MMIO_REGISTER_EVENT (MMIO_MASK | 0x010)
#define MMIO_PUTCHAR (MMIO_MASK | 0x100)
#define MMIO_PUTS (MMIO_MASK | 0x110)
#define MMIO_PUTINT (MMIO_MASK | 0x120)
#define MMIO_PUTHEX (MMIO_MASK | 0x130)

#define MMIO_OUTNOTE (MMIO_MASK | 0x200)

#define MMIO_MOVEGRIDCURSOR (MMIO_MASK | 0x300)
#define MMIO_SETGRIDTILE (MMIO_MASK | 0x310)

#define MMIO_ISPLAYING (MMIO_MASK | 0x400)
#define MMIO_ISENABLED (MMIO_MASK | 0x410)

// Event constants so that they can be used in C as well
inline uint32_t pack_event_reference(const uint32_t event_id,
                                     const uint32_t address) {
    return (event_id << 24) | (address & 0xFFFFFF);
}

inline uint32_t parse_event_address(const uint32_t event_reference) {
    return event_reference & 0xFFFFFF;
}

inline uint32_t parse_event_id(const uint32_t event_reference) {
    return event_reference >> 24;
}

#define EVENTID_BEAT 1
#define EVENTID_NOTEON 2
#define EVENTID_NOTEOFF 3

#define EVENTID_ONPLAY 6
#define EVENTID_ONPAUSE 7

#define EVENTID_GRIDPRESS 8
#define EVENTID_GRIDRELEASE 9

#define EVENTID_MAX 16

// Note/uint32_t packing/unpacking
typedef struct {
    uint8_t note_number;
    uint8_t velocity;
    uint16_t frame;
} NoteStruct;

inline uint32_t pack_note(NoteStruct& note) {
    return (note.frame << 16) | (note.velocity << 8) | (note.note_number);
}

inline void unpack_note(const uint32_t packed, NoteStruct& note) {
    note.frame = (uint16_t)(packed >> 16);
    note.velocity = (uint8_t)(packed >> 8);
    note.note_number = (uint8_t)(packed);
}