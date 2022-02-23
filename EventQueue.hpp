#pragma once
#include <stddef.h>
#include "Events.hpp"

using namespace std;
// Keep track of events and their ordering
// TODO ADD PRIORITIZING WHEN NEEDED
class CEventQueue {
    public:
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