#include "EmulatorHost.hpp"
#include "MMIO.h"
#include "Constants.h"

CEmulatorHost* CEmulatorHost::CreateFromProgram(TJBox_Value iProgramString) {
	uint32_t bufsize = JBox_GetStringLength(iProgramString);
	vector<uint8_t> content = DecodeParameterString(iProgramString);
	// TEMPORARY HEX DEOCODER
	
	// TODO Load elf, replace buf with memory content
	uint32_t memsize = 0x10000; 
	auto emulator = new CEmulatorHost(memsize, 0);

	// Put result into memory
	for(int i = 0; i < bufsize; i++)
		emulator->fMemory.get_memory()[i] = (uint8_t)content[i];

	return emulator;
}


CEmulatorHost::CEmulatorHost(TJBox_UInt64 iMemorySize, uint32_t iEntryPoint) :  
	fMemory(iMemorySize, make_shared<CMMIO>(this)), 
	fEventQueue(10)
{
	// Set Property References
	fCustomPropertiesRef = JBox_GetMotherboardObjectRef("/custom_properties");
	fMainThread.Start(iEntryPoint);
}

bool CEmulatorHost::HandleMMIOStore(uint32_t iAddress, uint32_t iValue) {
	// Use index to shorten lookup distance between cases.
	switch (MMIO_INDEX(iAddress))
	{
	
	// TODO IMPLEMENT PUTS WITHOUT MEMORY LEAK
	case MMIO_INDEX(MMIO_PUTCHAR): {
		if(iValue != 0) // AVOID KILLING THE TERMINAL
			fTerminal.Putch(static_cast<char> (iValue)); 

		break;
	}
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

// Starts the next event on the event queue
// Returns false if event queue is empty.
bool CEmulatorHost::StartNextEvent() {
	// Grab next from queue if there is one
	SEvent event = {0, 0 ,0, 0, 0};
	if (!fEventQueue.Pop(event)) 
		return false;

	fEventThread.Start(event);
	return true;
}

uint64_t CEmulatorHost::ExecuteEvents(uint64_t iStepCount) {
	while(fEventThread.IsRunning() || StartNextEvent()) {
		// Finish current Event if there is one
		iStepCount = fEventThread.StepN(iStepCount, fMemory);
		if (iStepCount == 0) break;
		
		if (fEventThread.HasErrored())
			fTerminal.Puts("EVENT CRASHED :((((\n");

		if (fEventThread.HasFinished())
			fTerminal.Puts("EVENT FINISHED :))))\n");
	}

	// Return remaining steps
	return iStepCount;
}

uint64_t CEmulatorHost::ExecuteMain(uint64_t iStepCount) {
	if (!fMainThread.IsRunning()) return iStepCount;
	iStepCount = fMainThread.StepN(iStepCount, fMemory);

	if(fMainThread.HasErrored())
		fTerminal.Puts("MAIN THREAD BROKE\n");

	if(fMainThread.HasFinished())
		fTerminal.Puts("MAIN THREAD FINISHED\n");

	return iStepCount;
}

void CEmulatorHost::ProcessBatch(const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount) {
	if(index++ % 250 != 0) return;

	// FIND AND ADD EVENTS

	// RUN INSTRUCTIONS
	uint64_t steps = 100;

	steps = ExecuteEvents(steps);

	// Run remaining instructions on the main thread
	steps = ExecuteMain(steps);
	
	// POSTPROCESS BATCH
	fTerminal.SendProperties();
}