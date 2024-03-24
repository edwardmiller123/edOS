#include "threads.h"
#include "../drivers/timer.h"
#include "../../stdlib/stdlib.h"

// switchThread is only defined in the header since we link against the assembly 
// file for the function body

// threads is an array of the running threads (pointers to TCBs)
TCB * threads[10];

// initThreads just sets the TCB for the default thread
void initThreads() {
    TCB * defaultThread = malloc(sizeof(TCB));
    defaultThread -> ID = 1;
    // starting value of esp for the kernel
    defaultThread -> threadStackTop = (void *)0x9000;
    defaultThread -> cpuTime = kGetPITCount();
    
}

