# Plugin for mips-emulator
-----------

We are using the SimplePlayer as a template.

The plugin is meant to handle:

- Run Emulator Code
- MMIO 
    - Register Event
    - Current Batch Time for tight note timing
    - Dispatch UI commands
    - UI Data like knobs
    - Output Note
- Event dispatching
    - Whole, half, quarter notes
    - Button presses


# Building


## On Windows:
in x64 native Developer Command Prompt:
`python3 build45.py local45 Testing SDK="SDK path" (SDK is optional, default: ../..)`

## On Mac:
???????????????

Must have reason installed.