#pragma once
#include <stddef.h>
#include "UserIO.h"

// Keep track of events and their ordering
// TODO ADD PRIORITIZING WHEN NEEDED
class CEventQueue {
    public:
        struct SEvent
        {
            uint32_t fAddress;
            uint32_t fA0 = 0;
            uint32_t fA1 = 0;
            uint32_t fA2 = 0;
            uint32_t fA3 = 0;

            SEvent(uint32_t iAddress, uint32_t iA0, uint32_t iA1, uint32_t iA2, uint32_t iA3 ) : fAddress(iAddress), fA0(iA0), fA1(iA1), fA2(iA2), fA3(iA3) {};
            SEvent() {};
        };

        CEventQueue(size_t iCapacity);

        bool Push(SEvent iEvent);
        bool Pop(SEvent& oEvent);

        bool HasEvent() { return fHeadIndex > 0; };

    private:
        // Currently too scared to use vector in case of unknown allocations
        size_t fCapacity;

        SEvent* fEventBank;
        size_t fHeadIndex;
};