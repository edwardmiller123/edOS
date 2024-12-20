#include "syscall.h"
#include "screen.h"
#include "timer.h"
#include "keyboard.h"
#include "../log.h"
#include "../threads.h"
#include "../mem.h"
#include "../../stdlib/stdlib.h"

// syscallScreenHandler handles calls to print strings to the screen
void screenHandler(int input, int functionCode)
{
    // little empty at the moment but a switch statemement if we want
    // to add functionality for the "read" scenario
    switch (functionCode)
    {
    case 2:
        kPrintString((char *)input);
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
    // TODO: the buffer isnt getting cleared correctly from system calls
        resetKeyBuffer();
        break;
    }
    return output;
}

// memoryHandler handles calls to allocate and free heap memory.
// Read: mallocs
// Write: frees
void *memoryHandler(int input, int functionCode)
{
    void *output = 0;
    switch (functionCode)
    {
    case 1:
        output = kMalloc(input);
        break;
    case 2:
        kFree((void *)input);
        break;
    }
    return output;
}

// threadHandler handles calls to create user threads.
// Read: Set the current thread as in focus
// Write: create user thread
void threadHandler(int input, int functionCode)
{
    switch (functionCode)
    {
    case 1:
        makeInFocus();
        break;
    case 2:
        createUThread((void *)input);
        break;
    }
}

// timerHandler handles calls to the system timer.
// Read: gets the timer count
void *timerHandler(int functionCode)
{
    void *output = 0;
    switch (functionCode)
    {
    case 1:
        // cast to a pointer just so the compiler dopesnt complain
        output = (void *)kGetPITCount();
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
// The handler returns a void ptr so as to use the most generic type possible and it is up to the
// higher level handlers to cast the type accordingly.
void *syscallHandler(struct registers r)
{
    int input = r.eax;
    int DC = r.ebx;
    int FC = r.ecx;
    void *output = NULL;
    int args[] = {DC, FC};
    kLogf(DEBUG, "Syscall received. DC: $, FC: $", args, 2);
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
        break;
    case 5:
        threadHandler(input, FC);
        break;
    }
    PICsendEOI(r.intNumber);
    return output;
}