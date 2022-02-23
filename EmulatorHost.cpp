#include "EmulatorHost.hpp"
#include "MMIO.h"

CEmulatorHost::CEmulatorHost(TJBox_Float64 iSampleRate, TJBox_UInt64 iMemorySize) : 
	fSampleRate(iSampleRate), 
	fMemory(iMemorySize, make_shared<CMMIO>(this)), 
	fEventQueue(10)
{
	fMainThread.Start(0x100);

	uint32_t program[] = {0x00842026, 0x248400ff, 0x00042600, 0x24050041, 0xac850100, 0x2405006d, 0xac850100, 0x2405006f, 0xac850100, 0x2405006e, 0xac850100, 0x24050067, 0xac850100, 0x2405000a, 0xac850100, 0x08000000 | (0x00F00000 >> 2)};
	for (int i = 0; i < sizeof(program)/4; i++)
		fMemory.store(i * 4, program[i]);

	uint32_t fib[] = {0x00842026, 0x248400ff, 0x00042600, 0x2405002c, 0x24060020, 0x24110001, 0x2409000f, 0xac910120, 0x02309021, 0x00118021, 0x00128821, 0xac850100, 0xac860100, 0x25080001, 0x1509fff8, 0x00421025, 0x03e00008};
	for (int i = 0; i < sizeof(fib)/4; i++)
		fMemory.store((i * 4) + 0x100, fib[i]);
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
	// FIND AND ADD EVENTS

	// RUN INSTRUCTIONS
	uint64_t steps = 100;
	steps = ExecuteEvents(steps);

	// Run remaining instructions on the main thread
	steps = ExecuteMain(steps);

	// POSTPROCESS BATCH
	fTerminal.SendProperties();
}