#include "screen.h"
#include "../interrupts/isr.h"
#include "keyboard.h"
#include "syscall.h"
#include "../kernel/mem.h"
#include "timer.h"
#include "../stdlib/stdlib.h"

// syscallScreenHandler handles calls to print strings to the screen
void screenHandler(int input, int functionCode)
{
    // little empty at the moment but a switch statemement if we want
    // to add functionality for the "read" scenario
    switch (functionCode)
    {
    case 2:
        kPrintString((char*) input);
        break;
    }
}

// keyboardHandler handles calls to read the keyboard input from the keybuffer
// reads return the contents of the buffer and writes clear it
void *keyboardHandler(int input, int functionCode)
{
    char *output = 0;
    switch (functionCode)
    {
    case 1:
        output = readKeyBuffer();
        break;
    case 2:
        resetKeyBuffer();
        break;
    }
    return output;
}

// memoryHandler handles calls to allocate and free heap memory.
void *memoryHandler(int input, int functionCode)
{
    void *output = 0;
    switch (functionCode)
    {
    case 1:
        output = kMalloc(input);
        break;
    case 2:
        kFree(input);
        break;
    }
    return output;
}

// timerHandler handles calls to the system timer
void *timerHandler(int functionCode)
{
    void *output = 0;
    switch (functionCode)
    {
    case 1:
        output = kGetPITCount();
        break;
    }
    return output;
}

// syscallHandler calls the corresponding handler for the givem driver code (DC).
// The syscall handlers are called based of what DC the interrupt receives.
// They each receive a function code (FC) which specifies a read or write instruction
// screen: DC = 1
// keyboard: DC = 2
// memory: DC = 3
// FC = 1 : read
// FC = 2 : write
// The handler returns a void ptr so as to use the most generic type possible and to is up to the
// higher level handlers to cast the type accordingly.
void *syscallHandler(struct registers r)
{
    int input = r.eax;
    int DC = r.ebx;
    int FC = r.ecx;
    void *output = 0;
    switch (DC)
    {
    case 1:
        screenHandler(input, FC);
        break;
    case 2:
        output = keyboardHandler(input, FC);
        break;
    case 3:
        output = memoryHandler(input, FC);
        break;
    case 4:
        output = timerHandler(FC);
    }
    PICsendEOI(r.intNumber);
    return output;
}