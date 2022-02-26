#include "NoteHelper.hpp"
#include "UserIO.h"

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