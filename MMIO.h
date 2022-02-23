#pragma once
#include "stdint.h"

#define MMIO_MASK  0xFF000000U

#define MMIO_INDEX(address) (((address) & ~MMIO_MASK) >> 4)

#define MMIO_PUTCHAR    (MMIO_MASK | 0x100)
#define MMIO_PUTS       (MMIO_MASK | 0x110)
#define MMIO_PUTINT     (MMIO_MASK | 0x120)
#define MMIO_PUTHEX     (MMIO_MASK | 0x130)
