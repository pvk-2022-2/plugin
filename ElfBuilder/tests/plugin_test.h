#pragma once
#define MMIO_MASK  0xFF000000U

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef int int32_t;

volatile uint8_t* putch = (uint8_t*)(MMIO_MASK | 0x100);
volatile uint32_t* putint= (uint32_t*)(MMIO_MASK | 0x120);
volatile uint32_t* puthex= (uint32_t*)(MMIO_MASK | 0x130);

volatile uint32_t* is_playing = (uint32_t*)(MMIO_MASK | 0x400);
volatile uint32_t* is_enabled = (uint32_t*)(MMIO_MASK | 0x410);

#define EVENTID_BEAT 1
#define EVENTID_NOTEON 2
#define EVENTID_NOTEOFF 3

#define EVENTID_ONPLAY 6
#define EVENTID_ONPAUSE 7

#define EVENTID_GRIDPRESS 8
#define EVENTID_GRIDRELEASE 9

#define GRID_WIDTH 8
#define GRID_HEIGHT 8

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

void move_grid_cursor(const uint32_t x, const uint32_t y) {
    volatile uint32_t* move_cursor_mmio = (uint32_t*)(MMIO_MASK | 0x300);
    
    const uint32_t value = x | (y << 16);
    *move_cursor_mmio = value;
}

void set_grid_tile(const uint32_t color, const char ch) {
    volatile uint32_t* set_tile_mmio = (uint32_t*)(MMIO_MASK | 0x310);
    
    const uint32_t value = (uint32_t)(ch) | (color << 8);
    *set_tile_mmio = value;
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
