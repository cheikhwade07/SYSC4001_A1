Interrupts Simulator
====================

Overview
--------
This simulator models interrupt handling in an operating system.
It processes trace files containing CPU, SYSCALL, and END_IO events using a vector table and device table.
The program outputs a detailed execution trace showing each step of the interrupt process.

Folder Structure
----------------
.
├── interrupts.cpp          # Main simulation source
├── interrupts.hpp          # Helper and parsing functions
├── build.sh                # Script to compile the simulator
├── run_all.sh              # Script to execute all simulations
├── input_files/            # Contains trace_1.txt, trace_2.txt, etc.
├── vector_table.txt
├── device_table.txt
├── output_files/           # Generated simulation results (from run_all)
│   ├── trace_1_ctx10_isr40.txt
│   ├── trace_2_ctx30_isr200.txt
│   └── ...
├── execution.txt           # Default single-run output file
└── bin/interrupts.exe      # Compiled executable

Compilation
-----------
To compile the simulator and create the executable in /bin:

    bash build.sh

This produces:
    bin/interrupts.exe

Running the Simulator
---------------------
### Option 1 — Default Run (3 arguments)
Uses default values: context = 10 ms, ISR = 40 ms.

    ./bin/interrupts.exe input_files/trace_1.txt vector_table.txt device_table.txt

Output file:
    execution.txt

### Option 2 — Custom Run (specify output file and times)
Provide an output file and custom timing values (context and ISR).

    ./bin/interrupts.exe input_files/trace_1.txt vector_table.txt device_table.txt output_files/run1.txt 20 100

Output file:
    output_files/run1.txt

Running All Simulations
-----------------------
To automatically run all traces with varying context and ISR times:

    ./run_all.sh

This script:
- Creates the output_files/ folder (if not present)
- Runs each trace for multiple parameter combinations
- Saves results as:
      output_files/trace_1_ctx10_isr40.txt
      output_files/trace_2_ctx30_isr200.txt
      ...

Notes
-----
- Default times: context = 10 ms, ISR = 40 ms
- Requires a C++17-compatible compiler (g++ recommended)
- Works in Bash environments (Linux, macOS, or Git Bash on Windows)
