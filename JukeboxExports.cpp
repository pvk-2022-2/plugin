#include "StdInclude.h"
#include "Jukebox.h"
#include <cstring>
#include "SimplePlayer.h"

/**
 * @brief	This is our entry point when we're calling jbox.make_native_object_rw from realtime_controller.lua.
 * @details	Note that we pass a parameter - the instance id. The code below demonstrates how to it up even though we 
 *			currently don't use it.
 */
void* JBox_Export_CreateNativeObject(const char iOperation[], const TJBox_Value iParams[], TJBox_UInt32 iCount) {

	JBOX_ASSERT(std::strcmp(iOperation, "Instance") == 0);
	JBOX_TRACE("CreateNativeObject");
	JBOX_TRACE(iOperation);

	JBOX_ASSERT(iCount == 2);
	TJBox_Value instanceIDValue = JBox_MakeNumber(JBox_GetNumber(iParams[0]));
	TJBox_Value array[1];
	array[0] = instanceIDValue;
	JBOX_TRACEVALUES("instance ID = ^0", array, 1);

	const TJBox_Float64 sampleRate = JBox_GetNumber(iParams[0]);

	return new CSimplePlayer(sampleRate);
}

/** 
 * @brief	This is the callback we get from Reason/Recon to do our audio rendering
 */
void JBox_Export_RenderRealtime(void* privateState, const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount) {

	JBOX_ASSERT(privateState != nullptr);

	CSimplePlayer* pi = static_cast<CSimplePlayer*>(privateState);
	pi->RenderBatch(iPropertyDiffs, iDiffCount);
}
