#pragma once

#include "Jukebox.h"
#include "EventManager.hpp"
#include <vector>

class CNoteHelper {
public:
    void HandleDiffs(CEventManager& iEventManager,
                     const TJBox_PropertyDiff iPropertyDiffs[],
                     TJBox_UInt32 iDiffCount);
    void SendNoteEvent(uint8_t iNoteNumber, uint8_t iVelocity,
                       uint16_t iFrameIndex);

    bool HandleMMIOStore(uint32_t iAddress, uint32_t iValue);

    CNoteHelper();

private:
    TJBox_ObjectRef fNoteStates;
    std::vector<bool> fSentNotes;
};