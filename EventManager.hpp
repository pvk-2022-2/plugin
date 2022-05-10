#pragma once

#include "EventQueue.hpp"
#include "UserIO.h"

class CEventManager {
    using SEvent = CEventQueue::SEvent;

public:
    CEventManager(uint32_t iQueueCapacity) : fQueue(iQueueCapacity){};

    bool QueueEvent(uint32_t iEventID, uint32_t iA0 = 0, uint32_t iA1 = 0,
                    uint32_t iA2 = 0, uint32_t iA3 = 0) {
        uint32_t eventAddress = GetEventVector(iEventID);
        if (!eventAddress) // Unassigned event id
            return false;

        SEvent event(eventAddress, iA0, iA1, iA2, iA3);
        return fQueue.Push(event);
    }

    bool GetNextEvent(SEvent& oEvent) { return fQueue.Pop(oEvent); }

    bool SetEventVector(uint32_t iEventID, uint32_t iAddress) {
        if (iEventID >= EVENTID_MAX) return false;
        fEventVectorTable[iEventID] = iAddress;

        return true;
    }

    bool HandleMMIOStore(uint32_t iAddress, uint32_t iValue) {
        switch (MMIO_INDEX(iAddress)) {
            case MMIO_INDEX(MMIO_REGISTER_EVENT): {
                const uint32_t eventID = parse_event_id(iValue);
                const uint32_t eventAddress = parse_event_address(iValue);
                SetEventVector(eventID, eventAddress);

                break;
            }
            default: return false;
        }

        return true;
    }

private:
    uint32_t fEventVectorTable[EVENTID_MAX] = {};
    uint32_t GetEventVector(uint32_t iEventID) {
        if (iEventID >= EVENTID_MAX) return 0;
        return fEventVectorTable[iEventID];
    };

    CEventQueue fQueue;
};