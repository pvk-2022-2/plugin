#include "NoteHelper.hpp"
#include "UserIO.h"

void SendNoteOffEvent(uint8_t iNoteNumber, uint16_t iFrameIndex)
{
	if(iNoteNumber & 0x80) 
		return;

	TJBox_NoteEvent event;
	event.fNoteNumber = iNoteNumber;
	event.fVelocity = 0;
	event.fAtFrameIndex = iFrameIndex;
	JBox_OutputNoteEvent(event);
}

void CNoteHelper::SendNoteEvent(uint8_t iNoteNumber, uint8_t iVelocity, uint16_t iFrameIndex)
{
	if(iNoteNumber & 0x80) 
		return;

	TJBox_NoteEvent event;
	event.fNoteNumber = iNoteNumber;
	event.fVelocity = iVelocity;
	event.fAtFrameIndex = iFrameIndex;
	JBox_OutputNoteEvent(event);
}


CNoteHelper::CNoteHelper() {
    fNoteStates = JBox_GetMotherboardObjectRef("/note_states");
}

void CNoteHelper::HandleDiffs(CEventManager& iEventManager, const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount) {
	for (TJBox_UInt32 i = 0; i < iDiffCount; ++i) {
		const TJBox_PropertyDiff& propertyDiff = iPropertyDiffs[i];
		if (propertyDiff.fPropertyRef.fObject == fNoteStates) {
			const TJBox_NoteEvent& noteEvent = JBox_AsNoteEvent(propertyDiff);
			
            // Reason sends off notes to very low notes after pausing and I don't know why
            if(noteEvent.fNoteNumber <= 8) continue;

            uint32_t eventID = noteEvent.fVelocity ?  EVENTID_NOTEON : EVENTID_NOTEOFF;
            iEventManager.QueueEvent(eventID, (uint32_t)noteEvent.fNoteNumber, (uint32_t)noteEvent.fVelocity);
		}
	}
}