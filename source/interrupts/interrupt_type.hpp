#pragma once

#include "types/types.hpp"

enum class InterruptType : uint8
{
    // CPU Exceptions (0-31)
    DivideByZero = 0,
    Debug = 1,
    NonMaskableInterrupt = 2,
    Breakpoint = 3,
    Overflow = 4,
    BoundRangeExceeded = 5,
    InvalidOpcode = 6,
    DeviceNotAvailable = 7,
    DoubleFault = 8,
    CoprocessorSegmentOverrun = 9,
    InvalidTSS = 10,
    SegmentNotPresent = 11,
    StackSegmentFault = 12,
    GeneralProtectionFault = 13,
    PageFault = 14,
    Reserved15 = 15,
    x87FPUError = 16,
    AlignmentCheck = 17,
    MachineCheck = 18,
    SIMDException = 19,
    VirtualizationException = 20,
    ControlProtectionException = 21,

    // IRQs (mapped to vectors 32-47)
    Timer = 32,        // IRQ0
    Keyboard,          // IRQ1
    Cascade,           // IRQ2
    SerialPort2,       // IRQ3
    SerialPort1,       // IRQ4
    ParallelPort2,     // IRQ5
    FloppyDisk,        // IRQ6
    ParallelPort1,     // IRQ7
    RealTimeClock,     // IRQ8
    Reserved41,         // IRQ9
    Reserved42,        // IRQ10
    Reserved43,        // IRQ11
    PS2Mouse,          // IRQ12
    FPUCoprocessor,     // IRQ13
    ATAPrimary,          // IRQ14
    ATASecondary         // IRQ15
};
