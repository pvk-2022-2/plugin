#pragma once

#include "EventManager.hpp"
#include "Jukebox.h"

#include <memory>
#include <utility>

typedef std::pair<int64_t, int64_t> TPPQRange;

// MODIFIED CSimplePlayer, most code stolen.
class CTimeHelper {
public:
    CTimeHelper(TJBox_Float64 iSampleRate);
    void HandleBatch(CEventManager& iEventManager,
                     const TJBox_PropertyDiff iPropertyDiffs[],
                     TJBox_UInt32 iDiffCount);

private:
    TJBox_Float64 ComputeBatchLengthPPQ() const;
    std::pair<TPPQRange, TPPQRange>
    ComputePPQRangesForCurrentBatch(TJBox_Float64 iBatchLengthPPQ) const;
    bool IsPositionInCurrentBatch(std::int64_t iPosition,
                                  TPPQRange iCurrentBatchRange) const;
    void ProcessRange(const TPPQRange& iRange, CEventManager& iEventManager);

    TJBox_ObjectRef fTransportRef;
    TJBox_ObjectRef fCustomPropertiesRef;
    TJBox_ObjectRef fEnvironmenRef;

    bool fIsPlaying;
    bool fIsLooping;
    bool fIsBypassedByHost;
    bool fIsEnabled;
    TJBox_Float64 fSampleRate;
    TJBox_Float64 fTempo;
    int64_t fLastPosition;
};
