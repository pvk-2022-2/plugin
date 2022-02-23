#include "EmulatorHost.hpp"
#include "MMIO.h"

CEmulatorHost::CEmulatorHost(TJBox_Float64 iSampleRate, TJBox_UInt64 iMemorySize) : 
	fSampleRate(iSampleRate), 
	fMemory(iMemorySize, make_shared<CMMIO>(this)), 
	fEventQueue(10)
{

	fMainThread.Start(0);
}

void CEmulatorHost::InitMemory() {
	
}

bool CEmulatorHost::HandleMMIOStore(uint32_t iAddress, uint32_t iValue) {
	// Use index to shorten lookup distance between cases.
	switch (MMIO_INDEX(iAddress))
	{
	
	// TODO IMPLEMENT PUTS WITHOUT MEMORY LEAK
	case MMIO_INDEX(MMIO_PUTCHAR): fTerminal.Putch(static_cast<char> (iValue)); break;
	case MMIO_INDEX(MMIO_PUTINT): fTerminal.PutInt(static_cast<int32_t>(iValue)); break;
	case MMIO_INDEX(MMIO_PUTHEX): fTerminal.PutHex(iValue); break;

	default: return false;
	}

	return true;
}

bool CEmulatorHost::HandleMMIORead(uint32_t iAddress, uint32_t& oValue) {
	
	switch (MMIO_INDEX(iAddress))
	{
	
	default: 
		return false;
	}

	return true;
}


uint64_t CEmulatorHost::ExecuteEvents(uint64_t iStepCount) {
	// Finish current Event if there is one
	iStepCount = fEventThread.StepN(iStepCount, fMemory);
	

	// Run next events in queue
	while(iStepCount > 0 && fEventQueue.HasEvent()) {
		SEvent event = {0, 0 ,0, 0, 0};
		fEventQueue.Pop(event);

		fEventThread.Start(event);
		iStepCount = fEventThread.StepN(iStepCount, fMemory);

		// TODO DISPLAY ERROR STUFF
	}

	// Return remaining steps
	return iStepCount;
}

void CEmulatorHost::ProcessBatch(const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount) {
	// FIND ADD EVENTS
	fTerminal.Puts("Among ");

	// RUN INSTRUCTIONS
	uint64_t steps = 1000;
	steps = ExecuteEvents(steps);

	// Run remaining instructions on the main thread
	if(steps > 0)
		steps = fMainThread.StepN(steps, fMemory);

	fMemory.store(MMIO_PUTINT, (int32_t)steps);
	fTerminal.Putch('\n');

	// POSTPROCESS BATCH
	fTerminal.SendProperties();
}