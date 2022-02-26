#include "EmulatorHost.hpp"
#include "UserIO.h"
#include "Constants.h"

CEmulatorHost* CEmulatorHost::CreateFromProgram(TJBox_Float64 iSampleRate, TJBox_Value iProgramString) {
	uint32_t bufsize = JBox_GetStringLength(iProgramString);
	vector<uint8_t> content = DecodeParameterString(iProgramString);
	// TEMPORARY HEX DEOCODER
	
	// TODO Load elf, replace buf with memory content
	uint32_t memsize = 0x10000; 
	auto emulator = new CEmulatorHost(iSampleRate, memsize, 0);

	// Put result into memory
	for(int i = 0; i < bufsize; i++)
		emulator->fMemory.get_memory()[i] = (uint8_t)content[i];

	return emulator;
}


CEmulatorHost::CEmulatorHost(TJBox_Float64 iSampleRate, TJBox_UInt64 iMemorySize, uint32_t iEntryPoint) :  
	fMemory(iMemorySize, make_shared<CMMIO>(this)), 
	fEventManager(10),
	fTimeHelper(iSampleRate)
{
	// Set Property References
	fCustomPropertiesRef = JBox_GetMotherboardObjectRef("/custom_properties");
	fMainThread.Start(iEntryPoint);
}

bool CEmulatorHost::HandleMMIOStore(uint32_t iAddress, uint32_t iValue) {
	// Use index to shorten lookup distance between cases.
	switch (MMIO_INDEX(iAddress))
	{
	
	case MMIO_INDEX(MMIO_REGISTER_EVENT): {
		const uint32_t eventID = parse_event_id(iValue);
		const uint32_t eventAddress = parse_event_address(iValue);

		fEventManager.SetEventVector(eventID, eventAddress);
		break;
	}

	// TODO IMPLEMENT PUTS WITHOUT MEMORY LEAK
	case MMIO_INDEX(MMIO_PUTCHAR): {
		if(iValue != 0) // AVOID KILLING THE TERMINAL
			fTerminal.Putch(static_cast<char> (iValue)); 

		break;
	}
	case MMIO_INDEX(MMIO_PUTINT): fTerminal.PutInt(static_cast<int32_t>(iValue)); break;
	case MMIO_INDEX(MMIO_PUTHEX): fTerminal.PutHex(iValue); break;

	case MMIO_INDEX(MMIO_OUTNOTE): {
		NoteStruct ns;
		unpack_note(iValue, ns);

		fNoteHelper.SendNoteEvent(ns.note_number, ns.velocity, ns.frame);
		break;
	}

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
	CEventQueue::SEvent event;
	if (!fEventManager.GetNextEvent(event)) 
		return false;


	fEventThread.Start(event);
	return true;
}

uint64_t CEmulatorHost::ExecuteEvents(uint64_t iStepCount) {
	while(fEventThread.IsRunning() || StartNextEvent()) {
		// Finish current Event if there is one
		iStepCount = fEventThread.StepN(iStepCount, fMemory);
		if (iStepCount == 0) break;
		
		if (fEventThread.HasErrored()) {
			fTerminal.Puts("EVENT CRASHED :((((\n");
		}

		/*
		if (fEventThread.HasFinished())
			fTerminal.Puts("EVENT FINISHED :))))\n");
		*/
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
	fNoteHelper.HandleDiffs(fEventManager, iPropertyDiffs, iDiffCount);
	fTimeHelper.HandleBatch(fEventManager, iPropertyDiffs, iDiffCount);

	// RUN INSTRUCTIONS
	uint64_t steps = 1000;

	steps = ExecuteEvents(steps);

	// Run remaining instructions on the main thread
	ExecuteMain(steps);
	
	// POSTPROCESS BATCH
	fTerminal.SendProperties();
}