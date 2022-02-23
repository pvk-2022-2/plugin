#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>
#include "mips-emulator/runtime_static_memory.hpp"
#include "mips-emulator/instruction.hpp"

#include "EventQueue.hpp"
#include "EmulatorThread.hpp"
#include "Terminal.h"
#include "MMIO.h"

#include "Jukebox.h"
#include <utility>
#include <vector>

using namespace std;
using namespace mips_emulator;
class CEmulatorHost : std::enable_shared_from_this<CEmulatorHost> {
    public:
        CEmulatorHost(TJBox_Float64 iSampleRate, size_t iMemorySize);
        void InitMemory();
        Terminal& GetTerminal() { return fTerminal; };

        void ProcessBatch(const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount);

        template<typename T> 
        optional<T> read(uint32_t iAddress) {
            if (!IsMMIO(iAddress))  // RETURN ADDIU $a0, $a0, 1 FOR DEBUGGING PURPOSES
                return optional<T>(static_cast<T>(0x24840001));

            uint32_t result;
            const bool success = HandleMMIORead(iAddress, result);

            if(success) return optional(static_cast<T>(result));
            else return optional<T>(); 
        };

        template<>
        optional<Instruction>  read(uint32_t iAddress) {
            return optional<Instruction>();
        }

        template<typename T>
        bool store(uint32_t iAddress, T iValue) {
            if (!IsMMIO(iAddress)) return false;
            const uint32_t u32Value = static_cast<uint32_t>(iValue);

            return HandleMMIOStore(iAddress, u32Value);
        };

    private:
        bool IsMMIO(uint32_t iAddress) { return (iAddress & MMIO_MASK) == MMIO_MASK; };
        bool HandleMMIOStore(uint32_t iAddress, uint32_t iValue);
        bool HandleMMIORead(uint32_t iAddress, uint32_t& oValue);

        uint64_t ExecuteEvents(uint64_t iStepCount);

        RuntimeStaticMemory<CEmulatorHost> fMemory;
        CEmulatorThread fMainThread;  // Registerfile to be used for main
        CEmulatorThread fEventThread; // Registerfile used by the current event

        CEventQueue fEventQueue;
        Terminal fTerminal;

        TJBox_Float64 fSampleRate;
};