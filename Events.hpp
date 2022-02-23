#pragma once
#include <stddef.h>
#include <cstdint>

using namespace std;

enum class EEventID : uint8_t {
    eOnBeat,
};

struct SEvent
{
    uint32_t fAddress;
    uint32_t fA0;
    uint32_t fA1;
    uint32_t fA2;
    uint32_t fA3;

    SEvent(uint32_t iAddress, uint32_t iA0, uint32_t iA1, uint32_t iA2, uint32_t iA3 ) : fAddress(iAddress), fA0(iA0), fA1(iA1), fA2(iA2), fA3(iA3) {};
    SEvent(uint32_t iAddress) : SEvent(iAddress, 0, 0, 0, 0) {};
};
