#include "EmulatorThread.hpp"

void CEmulatorThread::Start(uint32_t iAddress) {
    fRegisterFile.zero_all();
    fRegisterFile.set_pc(iAddress);

    // Set reconizable return address
    fRegisterFile.set_unsigned(RegisterName::e_ra, kFinishedExecMask);
    fRegisterFile.set_unsigned(RegisterName::e_sp, fStackAddress);
    fState = EThreadState::eRunning;
}

void CEmulatorThread::Start(const CEventQueue::SEvent& iEvent) {
    Start(iEvent.fAddress);

    fRegisterFile.set_unsigned(RegisterName::e_a0, iEvent.fA0);
    fRegisterFile.set_unsigned(RegisterName::e_a1, iEvent.fA1);
    fRegisterFile.set_unsigned(RegisterName::e_a2, iEvent.fA2);
    fRegisterFile.set_unsigned(RegisterName::e_a3, iEvent.fA3);
}
