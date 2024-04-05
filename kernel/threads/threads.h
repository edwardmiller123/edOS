#ifndef THREADS
#define THREADS

// TCB (thread controller block) holds information about a running
// thread.
typedef struct TCB {
    void * threadStackTop;
    struct TCB * nextThread;
    void * threadEntry;
    int initTime; // the time stamp when the thread was created
    int cpuTime; // amount of processing time spent on this thread
} __attribute__((packed)) TCB;

// switchThread switches the current thread to the one in the given TCB
extern void switchThread(TCB * currentThread, TCB * nextThread);

// addToThreadList adds a new TCB into the circular thread list;
void addToThreadList(TCB * newThread);

// initThreads just creates a TCB for the default thread
void initThreads();

// createThread creates a new thread TCB and add its to a list of active threads. 
// It is left up to the scheduler to actually execute the new thread. New threads are created with
// a max of 6kb (0x1800) for now.
int createThread(void * threadFunction);

#endif