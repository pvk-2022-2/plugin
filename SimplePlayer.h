#pragma once

#include "EmulatorHost.hpp"
#include "Jukebox.h"

#include <memory>
#include <utility>

typedef std::pair<std::int64_t, std::int64_t> TPPQRange;

class CSimplePlayer
{
	public: CSimplePlayer(TJBox_Float64 iSampleRate);

    private: void HandleDiffs(const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount);
    private: std::uint8_t NoteFromCV(std::uint16_t iDefaultNote,TJBox_PropertyRef iNoteCVConnectedRef, TJBox_PropertyRef iNoteCVInputRef);
	private: TJBox_Float64 ComputeBatchLengthPPQ() const;
	private: std::pair<TPPQRange, TPPQRange> ComputePPQRangesForCurrentBatch(TJBox_Float64 iBatchLengthPPQ) const;
	private: void ForwardNoteEvents(const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount);
	private: bool IsPositionInCurrentBatch(std::int64_t iPosition, TPPQRange iCurrentBatchRange) const;
	private: void PlayRange(const TPPQRange& iRange);

	
	public: void RenderBatch(const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount);

    private: TJBox_ObjectRef fTransportRef;
    private: TJBox_ObjectRef fCustomPropertiesRef;
    private: TJBox_ObjectRef fNoteStates;
    private: TJBox_ObjectRef fEnvironmenRef;

    private: TJBox_PropertyRef fNumeratorNoteCVInputRef;
    private: TJBox_PropertyRef fNumeratorNoteCVConnectedRef;
    private: TJBox_PropertyRef fDenominatorNoteCVInputRef;
    private: TJBox_PropertyRef fDenominatorNoteCVConnectedRef;

    private: bool fIsPlaying;
    private: bool fIsLooping;
    private: bool fIsBypassedByHost;
    private: bool fIsEnabled;
    private: TJBox_Float64 fSampleRate;
    private: TJBox_Float64 fTempo;
    private: std::uint8_t fNumeratorNote;
    private: std::uint8_t fDenominatorNote;
    private: std::uint8_t fLastNumeratorNote;
    private: std::uint8_t fLastDenominatorNote;
	private: std::int64_t fLastPosition;

    private: TJBox_UInt64 index = 0;
    private: CEmulatorHost fHost;
};

