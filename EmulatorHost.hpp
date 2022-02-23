#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>
#include "mips-emulator/runtime_static_memory.hpp"
#include "mips-emulator/instruction.hpp"

#include "EventQueue.hpp"
#include "EmulatorThread.hpp"
#include "Terminal.h"

#include "EmulatorMMIO.hpp"

#include "Jukebox.h"
#include <utility>
#include <vector>

using namespace std;
using namespace mips_emulator;

class CEmulatorHost {
    using CMMIO = CEmulatorMMIO<CEmulatorHost>;
    public:
        CEmulatorHost(TJBox_Float64 iSampleRate, size_t iMemorySize);
        void InitMemory();

        Terminal& GetTerminal() { return fTerminal; };

        void ProcessBatch(const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount);
        bool HandleMMIOStore(uint32_t iAddress, uint32_t iValue);
        bool HandleMMIORead(uint32_t iAddress, uint32_t& oValue);

    private:        
        uint64_t ExecuteEvents(uint64_t iStepCount);

        RuntimeStaticMemory<CMMIO> fMemory;
        CEmulatorThread fMainThread;  // Registerfile to be used for main
        CEmulatorThread fEventThread; // Registerfile used by the current event

        CEventQueue fEventQueue;
        Terminal fTerminal;

        TJBox_Float64 fSampleRate;
};