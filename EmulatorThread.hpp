#pragma once
#include "mips-emulator/register_file.hpp"
#include "mips-emulator/runtime_static_memory.hpp"
#include "mips-emulator/executor.hpp"

#include "Events.hpp"

using namespace mips_emulator;
using namespace std;

enum class EThreadState {
    eRunning,
    eError,
    eFinished,
};

class CEmulatorThread {
    public:
        // PC mask for successful execution
        const uint32_t kFinishedExecMask = 0x0F000000;

        CEmulatorThread(uint32_t iStackAddress) : fStackAddress(iStackAddress), fState(EThreadState::eError) {};
        CEmulatorThread() : CEmulatorThread(0) {};

        void Start(uint32_t iAddress);
        void Start(const SEvent& iEvent);
        void Kill();

        bool IsRunning() { return fState == EThreadState::eRunning; }
        bool HasErrored() {return fState == EThreadState::eError; }

        const RegisterFile& GetRegisterFile() { return fRegisterFile; }

        template<typename Memory>
        uint64_t StepN(uint64_t iStepCount, Memory& iMemory) {
            if(!IsRunning()) return iStepCount;

            while (iStepCount > 0 && Executor::step(fRegisterFile, iMemory))
                iStepCount--;

            // Determine cause of preliminary stop
            if (iStepCount > 0) {
                const bool finished = (fRegisterFile.get_pc() & kFinishedExecMask) == kFinishedExecMask;
                fState = finished ? EThreadState::eFinished : EThreadState::eError;
            }

            return iStepCount;
        }

    private:
        uint32_t fStackAddress;
        EThreadState fState;
        RegisterFile fRegisterFile;
};