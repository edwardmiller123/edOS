#include "../interrupts/isr.h"
#include "../io.h"
#include "../threads.h"
#include "screen.h"

// Channel 0 generates an interrupt (IRQ0) at a set frequency.
// The channel stores the frequency divider (16 bit value). The default frequency is
// 1193180Hz and the default divider value is 65535.
#define CHAN_0_DATA 0x40

// Sending a byte to the command register sets the timer options
// command register bit map:
// 6 and 7      Select channel :
//                 0 0 = Channel 0
//                 0 1 = Channel 1
//                 1 0 = Channel 2
//                 1 1 = Read-back command (8254 only)
// 4 and 5      Access mode :
//                 0 0 = Latch count value command
//                 0 1 = Access mode: lobyte only
//                 1 0 = Access mode: hibyte only
//                 1 1 = Access mode: lobyte/hibyte
// 1 to 3       Operating mode :
//                 0 0 0 = Mode 0 (interrupt on terminal count)
//                 0 0 1 = Mode 1 (hardware re-triggerable one-shot)
//                 0 1 0 = Mode 2 (rate generator)
//                 0 1 1 = Mode 3 (square wave generator)
//                 1 0 0 = Mode 4 (software triggered strobe)
//                 1 0 1 = Mode 5 (hardware triggered strobe)
//                 1 1 0 = Mode 2 (rate generator, same as 010b)
//                 1 1 1 = Mode 3 (square wave generator, same as 011b)
// 0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
#define COMMAND 0x43

// The default clock frequency before being divided
#define DEFAULT_FREQ 1193180

// These are the settings for the command register: 01111100
// = 0(binary mode) 111(square wave) 11(lobyte/hibyte) 00 (channel 0)
// With these settings the PIT expects to receive two writes to the
// data register to set the divider.
#define PIT_SETTINGS 0x7C

// The total time the system has been running for in ms
static int systemUptime = 0;

// timer updates the global run time using the PIT. It also calls the scheduler
// to switch between threads
int timer(struct registers r)
{
    // run the scheduler on every tick
    systemUptime += 1;
    schedule(systemUptime);
    return 0;
}

// setTimerFrequency sets how often irq0 fires to the
// given frequency which must be a 16 bit number
void setTimerFrequency(short targetFreq)
{
    short divider = DEFAULT_FREQ / targetFreq;
    // set the command register byte
    writeByte(COMMAND, PIT_SETTINGS);
    // write the high and low bytes of the divider to the data register

    // get the first 8 bits
    char lowByte = divider & 0xFF;
    writeByte(CHAN_0_DATA, lowByte);

    // get the last 8 bits
    char highByte = divider >> 8;
    writeByte(CHAN_0_DATA, highByte);
}

// initTimer initialises the PIT
void initTimer()
{
    systemUptime = 0;
    registerInterruptHandler(32, timer);
    // set frequency to 1000 to tick every ms
    setTimerFrequency(1000);
}

// kGetPITCount returns the total system up time in ms
int kGetPITCount()
{
    return systemUptime;
}

// kSleep waits until the given time in milliseconds has passed
void kSleep(int ms)
{
    int currentTime = kGetPITCount();
    int startTime = currentTime;
    int timeEnd = startTime + ms;
    while (currentTime < timeEnd) {
        currentTime = kGetPITCount();
    };
    return;
}