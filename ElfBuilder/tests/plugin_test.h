#pragma once
#define MMIO_MASK  0xFF000000U

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef int int32_t;

volatile uint8_t* putch = (uint8_t*)(MMIO_MASK | 0x100);
volatile uint32_t* putint= (uint32_t*)(MMIO_MASK | 0x120);
volatile uint32_t* puthex= (uint32_t*)(MMIO_MASK | 0x130);

#define EVENTID_BEAT 1
#define EVENTID_NOTEON 2
#define EVENTID_NOTEOFF 3

void play_note(const uint32_t frame, const uint8_t velocity, const uint8_t note_number) {
    const uint32_t note_data = (frame << 16) | (velocity << 8) | (note_number);
    volatile uint32_t* play_note_mmio = (uint32_t*)(MMIO_MASK | 0x200);

    *play_note_mmio = note_data;
}

void register_event(const uint32_t event_id, const uint32_t address) {
    const uint32_t event_reference = (event_id << 24) | (address & 0xFFFFFF);
    volatile uint32_t* register_event_mmio = (uint32_t*)(MMIO_MASK | 0x010);

    *register_event_mmio = event_reference;
}

void puts(char* str) {
    while(*str)
        *putch = *str++;
}

void* memcpy(void* dest, const void* src, uint32_t count) {
    char* dst8 = (char*)dest;
    char* src8 = (char*)src;

    while (count--)
        *dst8++ = *src8++;

    return dest;
}
