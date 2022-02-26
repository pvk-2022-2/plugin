#include "StdInclude.h"

#include "TimeHelper.hpp"
#include "Constants.h"
#include "UserIO.h"

#include <cmath>
#include <vector>

namespace {

std::uint16_t PPQToFrameIndex(TJBox_Float64 iPPQ, TJBox_Float64 iTempo, TJBox_Float64 iSampleRate)
{
	std::uint8_t frameIndex = 0;
	const TJBox_Float64 beatsPerSecond = iTempo / 60;
	const TJBox_Float64 numPPQsPerSecond = beatsPerSecond * kPPQResolution;
	const TJBox_Float64 seconds =  iPPQ / numPPQsPerSecond;
	const TJBox_Float64 frames = seconds * iSampleRate;
	frameIndex = static_cast<std::uint16_t>(std::max<TJBox_Float64>(0, std::min<TJBox_Float64>(frames, 63)));
	JBOX_ASSERT(frameIndex >= 0 && frameIndex < kBatchSize);
	return frameIndex;
}

std::int64_t FindNextBeat(std::int64_t iBeatLength, std::int64_t iMaxBeatPosition)
{
	JBOX_ASSERT(iBeatLength > 0);
	const std::int64_t absolutePPQ = static_cast<std::int64_t>((iMaxBeatPosition) / iBeatLength) * iBeatLength;
	return absolutePPQ;
}

bool IsInRange(std::int64_t iPosition, const TPPQRange& iRange) {
	if (iPosition >= iRange.first && iPosition < iRange.second) {
		return true;
	}
	return false;
}

bool IsEmptyRange(const TPPQRange& iRange) {
	return iRange.first == iRange.second;
}

#if DEBUG
bool IsValidRange(const TPPQRange& iRange) {
	return iRange.first <= iRange.second;
}
#endif // DEBUG
}

CTimeHelper::CTimeHelper(TJBox_Float64 iSampleRate) :
	fIsPlaying(false),
	fSampleRate(iSampleRate),
	fLastPosition(0)
{

	fCustomPropertiesRef = JBox_GetMotherboardObjectRef("/custom_properties");
	fTransportRef = JBox_GetMotherboardObjectRef("/transport");
	fEnvironmenRef = JBox_GetMotherboardObjectRef("/environment");
}

TJBox_Float64 CTimeHelper::ComputeBatchLengthPPQ() const {
	const TJBox_Float64 batchLengthPPQ = (kBatchSize / fSampleRate) * ((fTempo / 60) * kPPQResolution);
	return batchLengthPPQ;
}

// Returns two ranges to support looping
std::pair<TPPQRange, TPPQRange> CTimeHelper::ComputePPQRangesForCurrentBatch(TJBox_Float64 iBatchLengthPPQ) const
{
	TPPQRange firstRange;
	TPPQRange secondRange; // Only used if the main seequencer has looped this batch

	TJBox_Float64 batchBeginPPQ = JBox_LoadMOMPropertyAsNumber(fTransportRef, kJBox_TransportPlayPos);
	const TJBox_Float64 batchEndPPQ = batchBeginPPQ + iBatchLengthPPQ;
	firstRange.first = static_cast<std::int64_t>(batchBeginPPQ);
	firstRange.second = static_cast<std::int64_t>(batchEndPPQ);

	const TJBox_Float64 absDiff = std::abs(batchBeginPPQ - fLastPosition);
	if (absDiff <= (iBatchLengthPPQ * 4)) {
		// Sequencer pos has (probably) not jumped,
		// adjust start of current batch to the end of previous batch
		firstRange.first = fLastPosition;
	}

	// Handle main sequencer looping
	if (fIsLooping) {
		// Did the sequencer loop this batch?
		const std::int64_t loopEndPPQ = static_cast<std::int64_t>(JBox_LoadMOMPropertyAsNumber(fTransportRef, kJBox_TransportLoopEndPos));

		if (IsInRange(loopEndPPQ, firstRange)) {
			const std::int64_t loopBeginPPQ = static_cast<std::int64_t>(JBox_LoadMOMPropertyAsNumber(fTransportRef, kJBox_TransportLoopStartPos));

			firstRange.second = loopEndPPQ;
			secondRange.first = loopBeginPPQ;
			const TJBox_Float64 secondRangeLength = iBatchLengthPPQ - (loopEndPPQ - batchBeginPPQ);
			JBOX_ASSERT(secondRangeLength >= 0);
			secondRange.second = loopBeginPPQ + secondRangeLength;

			if (IsEmptyRange(firstRange)) {
				// Edge case, the sequencer looped at the end of the batch,
				// just swap so that the first range is never empty
				std::swap(firstRange, secondRange);
			}
		}
	}

	JBOX_ASSERT(IsValidRange(firstRange));
	JBOX_ASSERT(!IsEmptyRange(firstRange) || (IsEmptyRange(firstRange) && IsEmptyRange(secondRange)));
	JBOX_ASSERT(IsValidRange(secondRange));
	return std::make_pair(firstRange, secondRange);
}

void CTimeHelper::ProcessRange(const TPPQRange& iRange, CEventManager& iEventManager) {

	// Output the denominator beat if it happens in the current batch
	{
		const std::int64_t beatLength = kPPQResolution; // Denominator is always on quarter note beats
		const std::int64_t nextBeat = FindNextBeat(beatLength, iRange.second);
		if (IsInRange(nextBeat, iRange)) {
			const std::int64_t offsetPPQ = nextBeat - iRange.first;
			const std::uint16_t frameIndex = PPQToFrameIndex(offsetPPQ, fTempo, fSampleRate);
			
			iEventManager.QueueEvent(EVENTID_BEAT, frameIndex);
		}
	}

	fLastPosition = iRange.second;
}


void CTimeHelper::HandleBatch(CEventManager& iEventManager, const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount)
{
	// Update player state
	for (TJBox_UInt32 i = 0; i < iDiffCount; ++i) {
		const TJBox_PropertyDiff& diff = iPropertyDiffs[i];

		if (diff.fPropertyRef.fObject == fTransportRef && diff.fPropertyTag == kJBox_TransportPlaying)
			fIsPlaying = JBox_GetBoolean(diff.fCurrentValue);

		else if (diff.fPropertyRef.fObject == fTransportRef && diff.fPropertyTag == kJBox_TransportLoopEnabled)
			fIsLooping = JBox_GetBoolean(diff.fCurrentValue);

		else if (diff.fPropertyRef.fObject == fTransportRef && diff.fPropertyTag == kJBox_TransportTempo)
			fTempo = JBox_GetNumber(diff.fCurrentValue);

		else if (diff.fPropertyRef.fObject == fEnvironmenRef && diff.fPropertyTag == kJBox_EnvironmentPlayerBypassed)
			fIsBypassedByHost = JBox_GetBoolean(diff.fCurrentValue);

		else if (diff.fPropertyRef.fObject == fCustomPropertiesRef && diff.fPropertyTag == kOnOffTag)
			fIsEnabled = JBox_GetBoolean(diff.fCurrentValue);
	}

	const TJBox_Float64 batchLength = ComputeBatchLengthPPQ();
	const std::pair<TPPQRange, TPPQRange> ranges = ComputePPQRangesForCurrentBatch(batchLength);

	ProcessRange(ranges.first, iEventManager);
	if (!IsEmptyRange(ranges.second))
		ProcessRange(ranges.second, iEventManager);
}
