#include "threads.h"
#include "../drivers/timer.h"
#include "../../stdlib/stdlib.h"

// switchThread is only defined in the header since we link against the assembly 
// file for the function body

// initThreads just sets the TCB for the default thread
void initThreads() {
    TCB * defaultThread = malloc(sizeof(TCB));
    defaultThread -> ID = 1;
    // starting value of esp for the kernel
    defaultThread -> stackTop = (void *)0x9000;
    defaultThread -> cpuTime = kGetPITCount();
    
}

