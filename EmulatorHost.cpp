#include "EmulatorHost.hpp"
#include "UserIO.h"
#include "Constants.h"

CEmulatorHost* CEmulatorHost::CreateFromProgram(TJBox_Float64 iSampleRate,
                                                TJBox_Value iProgramString) {
    std::vector<uint8_t> content = DecodeParameterString(iProgramString);

    // Create empty emulator if no program was provided
    if (content.size() == 0) return nullptr;

    const char* symbol_names[2] = {"s_main_stack_top", "s_event_stack_top"};
    Executable exec;
    auto res = LoadELF32(content.data(), symbol_names,
                         sizeof(symbol_names) / sizeof(char*), exec);

    switch (res) {
        case ELFResult::ok:
            return new CEmulatorHost(iSampleRate, exec.executable, exec.offset,
                                     exec.entry, exec.symbols[0] - 4,
                                     exec.symbols[1] - 4);

        case ELFResult::invalid_magic:
            JBOX_TRACE("ELF Error: Invalid magic in file");
            break;

        case ELFResult::not_32_bits:
            JBOX_TRACE("ELF Error: Not a 32-bit executable");
            break;

        case ELFResult::big_endian:
            JBOX_TRACE(
                "ELF Error: Executable is in big and not little endian format");
            break;

        case ELFResult::no_segments:
            JBOX_TRACE("ELF Error: No segments were found");
            break;

        case ELFResult::no_symbol_table:
            JBOX_TRACE("ELF Error: No symbol table was found");
            break;

        case ELFResult::no_string_table:
            JBOX_TRACE("ELF Error: No string table was found");
            break;

        case ELFResult::missing_symbols:
            JBOX_TRACE("ELF Error: Not all requested symbols were found");
            break;

        default: break;
    }

    JBOX_TRACE("COULD NOT LOAD ELF");

    // If elf could not be loaded, assume it's a pure .text segment for asm
    content.resize(0x10000, 0);
    return new CEmulatorHost(iSampleRate, content, 0, 0, 0x4000 - 4,
                             0x2000 - 4);
}

CEmulatorHost::CEmulatorHost(TJBox_Float64 iSampleRate,
                             std::vector<uint8_t> iExecutable, uint32_t iOffset,
                             uint32_t iEntry, uint32_t iMainStack,
                             uint32_t iEventStack)
    : fMemory(iExecutable, iOffset, make_shared<CMMIO>(this)),
      fEventManager(10), fTimeHelper(iSampleRate), fMainThread(iMainStack),
      fEventThread(iEventStack) {
    // Set Property References
    fCustomPropertiesRef = JBox_GetMotherboardObjectRef("/custom_properties");
    fMainThread.Start(iEntry);
}

bool CEmulatorHost::HandleMMIOStore(uint32_t iAddress, uint32_t iValue) {
    // Use index to shorten lookup distance between cases.
    switch (MMIO_INDEX(iAddress)) {
        case MMIO_INDEX(MMIO_REGISTER_EVENT): {
            const uint32_t eventID = parse_event_id(iValue);
            const uint32_t eventAddress = parse_event_address(iValue);

            fEventManager.SetEventVector(eventID, eventAddress);
            break;
        }

        // TODO IMPLEMENT PUTS WITHOUT MEMORY LEAK
        case MMIO_INDEX(MMIO_PUTCHAR): {
            if (iValue != 0) // AVOID KILLING THE TERMINAL
                fTerminal.Putch(iValue);

            break;
        }
        case MMIO_INDEX(MMIO_PUTINT):
            fTerminal.PutInt(static_cast<int32_t>(iValue));
            break;
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
    switch (MMIO_INDEX(iAddress)) {

        default: return false;
    }

    return true;
}

// Starts the next event on the event queue
// Returns false if event queue is empty.
bool CEmulatorHost::StartNextEvent() {
    // Grab next from queue if there is one
    CEventQueue::SEvent event;
    if (!fEventManager.GetNextEvent(event)) return false;

    fEventThread.Start(event);
    return true;
}

uint64_t CEmulatorHost::ExecuteEvents(uint64_t iStepCount) {
    while (fEventThread.IsRunning() || StartNextEvent()) {
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

    // Store pc and instruction for debugging purposes (assumes step = 1)
    const uint32_t pc = fMainThread.GetRegisterFile().get_pc();
    auto res_instr = fMemory.read_no_mmio<uint32_t>(pc);

    uint32_t instr = res_instr.is_error() ? 0x696969 : res_instr.get_value();

    iStepCount = fMainThread.StepN(iStepCount, fMemory);

    if (fMainThread.HasErrored()) {
        fTerminal.Puts("MAIN THREAD BROKE\nINSTR: ");

        // Print instruction info
        fTerminal.PutHex(instr);
        fTerminal.Puts(" at ");
        fTerminal.PutHex(pc);
        fTerminal.Puts("\n");
    }

    if (fMainThread.HasFinished()) fTerminal.Puts("MAIN THREAD FINISHED\n");

    return iStepCount;
}

void CEmulatorHost::ProcessBatch(const TJBox_PropertyDiff iPropertyDiffs[],
                                 TJBox_UInt32 iDiffCount) {
    // FIND AND ADD EVENTS
    fNoteHelper.HandleDiffs(fEventManager, iPropertyDiffs, iDiffCount);
    fTimeHelper.HandleBatch(fEventManager, iPropertyDiffs, iDiffCount);

    // RUN INSTRUCTIONS
    uint64_t steps = 1; // Use 1 for debugging
    steps = ExecuteEvents(steps);

    // Run remaining instructions on the main thread
    ExecuteMain(steps);

    // POSTPROCESS BATCH
    fTerminal.SendProperties();
}
