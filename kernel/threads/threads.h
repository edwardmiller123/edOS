#ifndef THREADS
#define THREADS

// PCB (thread controller block) holds information about a running
// thread.
typedef struct TCB {
    int ID;
    void * threadStackTop;
    void * nextThread;
    int cpuTime;
} TCB __attribute__((packed));

// switchThread switches the current thread to the one in the given TCB
extern void switchThread(TCB * currentThread, TCB * nextThread);

// initThreads just creates a TCB for the default thread
void initThreads();

#endif