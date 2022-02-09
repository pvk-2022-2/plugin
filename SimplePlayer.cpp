#include "StdInclude.h"

#include "SimplePlayer.h"
#include "Constants.h"

#include <cmath>
#include <vector>

namespace {

void SendNoteOffEvent(std::uint8_t iNoteNumber, std::uint16_t iFrameIndex)
{
	JBOX_ASSERT(iNoteNumber < 128);
	TJBox_NoteEvent event;
	event.fNoteNumber = iNoteNumber;
	event.fVelocity = 0;
	event.fAtFrameIndex = iFrameIndex;
	JBox_OutputNoteEvent(event);
}

void SendNoteOnEvent(std::uint8_t iNoteNumber, std::uint8_t iVelocity, std::uint16_t iFrameIndex)
{
	JBOX_ASSERT(iNoteNumber < 128);
	TJBox_NoteEvent event;
	event.fNoteNumber = iNoteNumber;
	event.fVelocity = iVelocity;
	event.fAtFrameIndex = iFrameIndex;
	JBox_OutputNoteEvent(event);
}


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


std::uint8_t ConvertCVToNote(TJBox_Value iCVValue)
{
	const TJBox_Float64 value = JBox_GetNumber(iCVValue) * 127.f + 0.1f;
	if (value < 0) {
		return 0;
	}
	else if (value > 127) {
		return 127;
	}
	return static_cast<std::uint8_t>(value);
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

CSimplePlayer::CSimplePlayer(TJBox_Float64 iSampleRate) :
	fIsPlaying(false),
	fSampleRate(iSampleRate),
	fLastNumeratorNote(kInvalidNoteNumber),
	fLastDenominatorNote(kInvalidNoteNumber),
	fLastPosition(0)
{

	fCustomPropertiesRef = JBox_GetMotherboardObjectRef("/custom_properties");
	fTransportRef = JBox_GetMotherboardObjectRef("/transport");
	fNoteStates = JBox_GetMotherboardObjectRef("/note_states");
	fEnvironmenRef = JBox_GetMotherboardObjectRef("/environment");

	// CV inputs
	TJBox_ObjectRef numeratorNoteCVRef = JBox_GetMotherboardObjectRef("/cv_inputs/numerator_note_cv");
	fNumeratorNoteCVInputRef = JBox_MakePropertyRef(numeratorNoteCVRef, "value");
	fNumeratorNoteCVConnectedRef = JBox_MakePropertyRef(numeratorNoteCVRef, "connected");
	TJBox_ObjectRef denominatorNoteCVRef = JBox_GetMotherboardObjectRef("/cv_inputs/denominator_note_cv");
	fDenominatorNoteCVInputRef = JBox_MakePropertyRef(denominatorNoteCVRef, "value");
	fDenominatorNoteCVConnectedRef = JBox_MakePropertyRef(denominatorNoteCVRef, "connected");
}

void CSimplePlayer::HandleDiffs(const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount)
{
	for (TJBox_UInt32 i = 0; i < iDiffCount; ++i) {
		const TJBox_PropertyDiff& diff = iPropertyDiffs[i];

		if (diff.fPropertyRef.fObject == fTransportRef && diff.fPropertyTag == kJBox_TransportPlaying)
		{
			fIsPlaying = JBox_GetBoolean(diff.fCurrentValue);
		}
		else if (diff.fPropertyRef.fObject == fTransportRef && diff.fPropertyTag == kJBox_TransportLoopEnabled)
		{
			fIsLooping = JBox_GetBoolean(diff.fCurrentValue);
		}
		else if (diff.fPropertyRef.fObject == fTransportRef && diff.fPropertyTag == kJBox_TransportTempo)
		{
			fTempo = JBox_GetNumber(diff.fCurrentValue);
		}
		else if (diff.fPropertyRef.fObject == fEnvironmenRef && diff.fPropertyTag == kJBox_EnvironmentPlayerBypassed)
		{
			fIsBypassedByHost = JBox_GetBoolean(diff.fCurrentValue);
		}
		else if (diff.fPropertyRef.fObject == fCustomPropertiesRef && diff.fPropertyTag == kOnOffTag)
		{
			fIsEnabled = JBox_GetBoolean(diff.fCurrentValue);
		}
	}
}

void CSimplePlayer::ForwardNoteEvents(const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount)
{
	for (TJBox_UInt32 i = 0; i < iDiffCount; ++i) {
		const TJBox_PropertyDiff& propertyDiff = iPropertyDiffs[i];
		if (propertyDiff.fPropertyRef.fObject == fNoteStates) {
			const TJBox_NoteEvent& noteEvent = JBox_AsNoteEvent(propertyDiff);
			JBox_OutputNoteEvent(noteEvent);
		}
	}
}

std::uint8_t CSimplePlayer::NoteFromCV(std::uint16_t iDefaultNote,TJBox_PropertyRef iNoteCVConnectedRef, TJBox_PropertyRef iNoteCVInputRef)
{
	if (JBox_GetBoolean(JBox_LoadMOMProperty(iNoteCVConnectedRef))) {
		const TJBox_Value noteCV = JBox_LoadMOMProperty(iNoteCVInputRef);
		std::uint8_t newNote = ConvertCVToNote(noteCV);
		return newNote;
	}
	return iDefaultNote;
}

TJBox_Float64 CSimplePlayer::ComputeBatchLengthPPQ() const {
	const TJBox_Float64 batchLengthPPQ = (kBatchSize / fSampleRate) * ((fTempo / 60) * kPPQResolution);
	return batchLengthPPQ;
}


// Returns two ranges to support looping
std::pair<TPPQRange, TPPQRange> CSimplePlayer::ComputePPQRangesForCurrentBatch(TJBox_Float64 iBatchLengthPPQ) const
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

void CSimplePlayer::PlayRange(const TPPQRange& iRange) {

	// Output the numerator beat if it happens in the current batch
	{
		const TJBox_Float64 numerator = JBox_LoadMOMPropertyAsNumber(fCustomPropertiesRef, kNumeratorTag) + 1;
		const TJBox_Float64 denominator = JBox_LoadMOMPropertyAsNumber(fCustomPropertiesRef, kDenominatorTag) + 1;
		const std::int64_t beatLength = static_cast<std::int64_t>(kPPQResolution * (denominator / numerator));
		const std::int64_t nextBeat = FindNextBeat(beatLength, iRange.second);
		if (IsInRange(nextBeat, iRange)) {
			const std::int64_t offsetPPQ = nextBeat - iRange.first;
			const std::uint16_t frameIndex = PPQToFrameIndex(offsetPPQ, fTempo, fSampleRate);

			const std::uint8_t note = NoteFromCV(kDefaultNumeratorNote, fNumeratorNoteCVConnectedRef, fNumeratorNoteCVInputRef);
			// Send a note off for the previously played note.
			// This is a naive way of handling note off events to keep this example minimal.
			if (fLastNumeratorNote != kInvalidNoteNumber) {
				SendNoteOffEvent(fLastNumeratorNote, 0);
			}
			fLastNumeratorNote = note;
			SendNoteOnEvent(note, kNoteVelocity, frameIndex);
		}
	}

	// Output the denominator beat if it happens in the current batch
	{
		const std::int64_t beatLength = kPPQResolution; // Denominator is always on quarter note beats
		const std::int64_t nextBeat = FindNextBeat(beatLength, iRange.second);
		if (IsInRange(nextBeat, iRange)) {
			const std::int64_t offsetPPQ = nextBeat - iRange.first;
			const std::uint16_t frameIndex = PPQToFrameIndex(offsetPPQ, fTempo, fSampleRate);
			const std::uint8_t note = NoteFromCV(kDefaultDenominatorNote, fDenominatorNoteCVConnectedRef, fDenominatorNoteCVInputRef);
			if (fLastDenominatorNote != kInvalidNoteNumber) {
				SendNoteOffEvent(fLastDenominatorNote, 0);
			}
			fLastDenominatorNote = note;
			SendNoteOnEvent(note, kNoteVelocity, frameIndex);
		}
	}
	fLastPosition = iRange.second;
}


void CSimplePlayer::RenderBatch(const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount)
{
	HandleDiffs(iPropertyDiffs, iDiffCount);

	if (fIsBypassedByHost) {
		// We don't have to do anything if the player is bypassed by the host
		return;
	}

	if (!fIsEnabled || !fIsPlaying) {
		if (fLastNumeratorNote != kInvalidNoteNumber) {
			SendNoteOffEvent(fLastNumeratorNote, 0);
			fLastNumeratorNote = kInvalidNoteNumber;
		}
		if (fLastDenominatorNote != kInvalidNoteNumber) {
			SendNoteOffEvent(fLastDenominatorNote, 0);
			fLastDenominatorNote = kInvalidNoteNumber;
		}
		// Forward all note events if the player is turned off
		// or if sequencer is not playing
		ForwardNoteEvents(iPropertyDiffs, iDiffCount);
		return;
	}

	// This player only adds note events, so we simply
	// forward all incoming events
	ForwardNoteEvents(iPropertyDiffs, iDiffCount);

	const TJBox_Float64 batchLength = ComputeBatchLengthPPQ();
	const std::pair<TPPQRange, TPPQRange> ranges = ComputePPQRangesForCurrentBatch(batchLength);
	PlayRange(ranges.first);
	if (!IsEmptyRange(ranges.second)) {
		PlayRange(ranges.second);
	}
}
