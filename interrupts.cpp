/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 */

#include "interrupts.hpp"

int main(int argc, char **argv)
{

    // vectors is a C++ std::vector of strings that contain the address of the ISR
    // delays  is a C++ std::vector of ints that contain the delays of each device
    // the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;     //!< string to store single line of trace file
    std::string execution; //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/
    long long current_time = 0; // current_time variable to track simulation time
    enum EventType
    {
        CPU_BURST,
        SYSCALL,
        END_IO
    };
    struct Event
    {
        EventType type;
        int start_time;   // time when event starts (ms)
        int duration;     // how long it takes (ms)
        std::string desc; // text label (e.g. "CPU burst", "SYSCALL 7")
    };
    struct CPUEvent : public Event
    {
        int burst_time; // duration from trace file
        CPUEvent(int duration_ms, long long &start_time)
        {
            type = CPU_BURST;
            burst_time = duration_ms;
            duration = duration_ms;
            desc = std::to_string(start_time) + ", " + std::to_string(burst_time) + ", CPU burst\n";
            start_time += burst_time;
        }
    };
    struct SyscallEvent : public Event
    {
        int device_id;           // e.g., 7
        std::string ISR_address; // from vector_table.txt (e.g. 0x0E)
        int io_delay;            // from device_table.txt (e.g. 110 ms)
        int isr_time;            // ISR execution time (variable, default 40 ms)
        int context_time;        // context save/restore time (10, 20, 30 ms)

        SyscallEvent(int dev, std::string ISR_Addr, int delay, long long &start_time, int isr_time = 40, int context_time = 10)
        {
            device_id = dev;
            ISR_address = ISR_Addr;
            io_delay = delay;
            type = SYSCALL; // default value (can vary)
            long long event_start = start_time;
            desc = "";
            // 1. switch to kernel mode
            desc += std::to_string(start_time) + ", 1, switch to kernel mode\n";
            start_time += 1;

            // 2. save context
            desc += std::to_string(start_time) + ", " + std::to_string(context_time) + ", save context\n";
            start_time += context_time;

            // 3. find vector entry
            desc += std::to_string(start_time) + ", 1, find vector " + std::to_string(device_id) + " in memory\n";
            start_time += 1;

            // 4. load ISR address into PC

            desc += std::to_string(start_time) + ", 1, load ISR address " + ISR_Addr + " into PC\n";
            start_time += 1;

            // 5. run ISR (device driver setup)
            desc += std::to_string(start_time) + ", " + std::to_string(isr_time) + ", SYSCALL: run the ISR (device driver for device " + std::to_string(device_id) + ")\n";
            start_time += isr_time;

            //  6. device performing I/O
            desc += std::to_string(start_time) + ", " + std::to_string(io_delay) + ", device " + std::to_string(device_id) + " performing I/O operation (device busy)\n";
            start_time += io_delay;

            // 7. return from interrupt
            desc += std::to_string(start_time) + ", 1, IRET (return from interrupt)\n";
            start_time += 1;
            duration = start_time - event_start;
        };
    };
    struct EndIOEvent : public Event
    {
        int device_id;           // e.g., 7 (used as the vector index)
        std::string ISR_address; // ISR address value from vector_table.txt (e.g., 0x001C)
        int isr_time;            // ISR execution time (default 40 ms)
        int context_time;        // context save/restore time (default 10 ms)

        EndIOEvent(int dev, std::string isrAddr, long long &start_time, int isr_time = 40, int context_time = 10)
        {
            device_id = dev;
            ISR_address = isrAddr;
            type = END_IO;
            long long event_start = start_time;

            desc = "";

            // 1. switch to kernel mode
            desc += std::to_string(start_time) + ", 1, switch to kernel mode\n";
            start_time += 1;

            // 2. save context
            desc += std::to_string(start_time) + ", " + std::to_string(context_time) + ", save context\n";
            start_time += context_time;

            // 3. find vector entry in memory (index = device_id)
            desc += std::to_string(start_time) + ", 1, find vector " + std::to_string(device_id) + " in memory position 0x" + std::to_string(device_id * 2) + "\n";
            start_time += 1;

            // 4. load ISR address from vector table into PC
            desc += std::to_string(start_time) + ", 1, load ISR address " +
                    ISR_address + " into the PC\n";
            start_time += 1;

            // 5. run ISR (device driver for end of I/O)
            desc += std::to_string(start_time) + ", " +
                    std::to_string(isr_time) + ", ENDIO: run the ISR (device driver for device " +
                    std::to_string(device_id) + ")\n";
            start_time += isr_time;

            // 6. check device status or mark I/O complete (added this because shown in TA example)
            desc += std::to_string(start_time) + ", 416, check device status and complete operation\n";
            start_time += 416;

            // 7. return from interrupt
            desc += std::to_string(start_time) + ", 1, IRET (return from interrupt)\n";
            start_time += 1;

            // total elapsed time
            duration = start_time - event_start;
        }
    };
    /******************************************************************/

    // parse each line of the input trace file
    while (std::getline(input_file, trace))
    {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/
        /***********SIMULATION CODE 1***********/
        if (activity == "CPU")
        {
            CPUEvent cpu(duration_intr, current_time);
            execution += cpu.desc;
        }
        else if (activity == "SYSCALL")
        {
            SyscallEvent syscall(duration_intr, vectors[duration_intr], delays[duration_intr], current_time);
            execution += syscall.desc;
        }
        else if (activity == "END_IO")
        {
            EndIOEvent endio(duration_intr, vectors[duration_intr], current_time);
            execution += endio.desc;
        }
        /***********SIMULATION CODE 1***********/

        /******************************************************************/
    }
    input_file.close();
    write_output(execution);
    return 0;
}
