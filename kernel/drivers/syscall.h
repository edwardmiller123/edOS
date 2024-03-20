#ifndef SYSCALL
#define SYSCALL

#include "../interrupts/isr.h"

// syscallScreenHandler handles calls to print strings to the screen
void screenHandler(int input, int functionCode);

// keyboardHandler handles calls to read the keyboard input from the keybuffer
// reads return the contents of the buffer and writes clear it
void *keyboardHandler(int input, int functionCode);

// memoryHandler handles calls to allocate and free heap memory.
void *memoryHandler(int input, int functionCode);

// timerHandler handles calls to the system timer.
// Read: gets the timer count
void *timerHandler(int functionCode);

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
void *syscallHandler(struct registers r);

#endif