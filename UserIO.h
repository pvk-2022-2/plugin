#pragma once
#include <stdint.h>

#define MMIO_MASK  0xFF000000U
#define MMIO_INDEX(address) (((address) & ~MMIO_MASK) >> 4)

// Write only MMIO
#define MMIO_REGISTER_EVENT (MMIO_MASK | 0x010)
#define MMIO_PUTCHAR        (MMIO_MASK | 0x100)
#define MMIO_PUTS           (MMIO_MASK | 0x110)
#define MMIO_PUTINT         (MMIO_MASK | 0x120)
#define MMIO_PUTHEX         (MMIO_MASK | 0x130)


// Event constants so that they can be used in C as well
#define EVENT_CREATE_REFERENCE(event_id, address) ((uint32_t)(((uint32_t)(event_id) << 24) | ((uint32_t)(address) & 0xFFFFFF)))
#define EVENT_PARSE_ADDRESS(event_ref) ((uint32_t)(event_ref) & 0xFFFFFF)
#define EVENT_PARSE_ID(event_ref) (((uint32_t)(event_ref) >> 24) & 0xFF)

#define EVENTID_BEAT 1
#define EVENTID_NOTEON 2
#define EVENTID_NOTEOFF 3


#define EVENTID_MAX 16