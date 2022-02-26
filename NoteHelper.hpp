#pragma once

#include "Jukebox.h"
#include "EventManager.hpp"

class CNoteHelper {
    public:
        void HandleDiffs(CEventManager& iEventManager, const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount);
        CNoteHelper();

    private:
        TJBox_ObjectRef fNoteStates;
};