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

# Loading Programs

Programs are loaded from the "program" string property. Since strings are null terminated, they are currently ecoded using hex. Note that the Hex in the Values field in each repatch file, is also in hex, so there are two layers of hex. Anyhow, here's a script that converts a MARS Hex output into a .repatch string.

```py
import sys

file = open(sys.argv[1])
lines = file.readlines()

lilend = ""
for l in lines:
    lilend += bytes.fromhex(l)[::-1].hex()

def hexify(s):
    return "".join('{:02x}'.format(ord(c)) for c in s)

print(hexify(lilend.upper()))
```

# Building


## On Windows:
You must first build the UI using RE2DRender, which converts GUI2D to be compatible with Reason. 

`RE2DRender <path to plugin\GUI2D\> <path to plugin\GUI\>`

Then build the 45 in x64 native Developer Command Prompt:

`python3 build45.py local45 Testing SDK="SDK path" (SDK is optional, default: ../..)`

## On Mac:
???????????????

Must have reason installed.