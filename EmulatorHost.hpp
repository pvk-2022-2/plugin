#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>
#include "mips-emulator/runtime_static_memory.hpp"
#include "mips-emulator/instruction.hpp"

#include "EventManager.hpp"
#include "EmulatorThread.hpp"

#include "Terminal.h"
#include "NoteHelper.hpp"
#include "TimeHelper.hpp"
#include "ELFLoader.hpp"

#include "ProgramDecoder.hpp"
#include "EmulatorMMIO.hpp"

#include "Jukebox.h"
#include <utility>
#include <vector>

using namespace std;
using namespace mips_emulator;

class CEmulatorHost {
    using CMMIO = CEmulatorMMIO<CEmulatorHost>;
    public:
        CEmulatorHost(TJBox_Float64 iSampleRate, std::vector<uint8_t> iExecutable, uint32_t iOffset, uint32_t iEntry, uint32_t iMain_Stack, uint32_t iEventStack);
        [[nodiscard]] static CEmulatorHost* CreateFromProgram(TJBox_Float64 iSampleRate, TJBox_Value iProgramString);

        CTerminal& GetTerminal() { return fTerminal; };

        void ProcessBatch(const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount);
        bool HandleMMIOStore(uint32_t iAddress, uint32_t iValue);
        bool HandleMMIORead(uint32_t iAddress, uint32_t& oValue);

    private:        
        uint64_t ExecuteEvents(uint64_t iStepCount);
        uint64_t ExecuteMain(uint64_t iStepCount);
        bool MustLoadProgram(); 
        bool StartNextEvent();

        RuntimeStaticMemory<CMMIO> fMemory;
        CEventManager fEventManager;
        CEmulatorThread fMainThread;  // Registerfile to be used for main
        CEmulatorThread fEventThread; // Registerfile used by the current event
        
        CTerminal fTerminal;
        CNoteHelper fNoteHelper;
        CTimeHelper fTimeHelper;

        // JBox property references
        TJBox_ObjectRef fCustomPropertiesRef;
};
