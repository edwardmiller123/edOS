#include "threads.h"
#include "../drivers/timer.h"
#include "../mem.h"

#define DEFAULT_STACK 0x9000 // 36kb
#define MAX_THREADS 10

// A linked list containing the active threads
typedef struct threadList
{
    int size;
    TCB *head;
} __attribute__((packed)) threadList;

// switchThread is only defined in the header since we link against the assembly
// file for the function body

// runningThread is the TCB of the thread currently executing
static TCB *runningThread;

// activeThreads contains all TCBs of currently in progress threads.
static threadList activeThreads;

// addToThreadList adds a new TCB into the circular thread list;
void addToThreadList(TCB *newThread)
{
    activeThreads.size += 1;
    if (activeThreads.size == 0)
    {
        // if list is empty the new thread becomes the head
        activeThreads.head = newThread;
    }
    else
    {
        TCB *lastThread = 0;
        TCB *thread = activeThreads.head->nextThread;
        while (thread != activeThreads.head)
        {
            // store the current thread so we can insert the new one just before the head.
            TCB *lastThread = thread;
            thread = thread->nextThread;
        }
        lastThread->nextThread = newThread;
    }
    // the new thread always links back to the list head
    newThread->nextThread = activeThreads.head;
}

// initThreads just sets the TCB for the default thread
void initThreads()
{
    activeThreads.size = 0;
    TCB *defaultThread = kMalloc(sizeof(TCB));
    defaultThread->initTime = kGetPITCount();
    // starting value of esp for the kernel
    defaultThread->threadStackTop = (void *)DEFAULT_STACK;
    addToThreadList(defaultThread);
    runningThread = defaultThread;
}

// createThread creates a new thread TCB and adds it to a linked list of active threads.
// It is left up to the scheduler to actually execute the new thread. New threads are created with
// a max space of 6kb (0x1800) for now.
void createThread(void *threadFunction)
{
    // create the new TCB
    TCB *newThread = kMalloc(sizeof(TCB));
    // set the thread entry point
    newThread->threadEntry = threadFunction;

    // Find a new stack position by itterating through thread list looking for
    // highest address so far;
    int highestStackPosition = 0;
    TCB *thread = activeThreads.head->nextThread;
    highestStackPosition = activeThreads.head->threadStackTop;
    while (thread != (activeThreads.head))
    {
        if ((thread->threadStackTop) > highestStackPosition)
        {
            highestStackPosition = thread->threadStackTop;
        }
        thread = thread->nextThread;
    }
    // set new thread stack position
    newThread->threadStackTop = (void *)(highestStackPosition + 0x1800);
    addToThreadList(newThread);
}

// schedule updates the running threads TCB and switches to a new thread based on
// the cpuTime it has received. This should be called as part of the timer interrupt.
// takes the current system uptime as an arg to avoid circular dependancies.
void schedule(int currentUptime)
{
    // first update the cpuTime of the currently running thread
    int newCpuTime = currentUptime - (runningThread->initTime);
    runningThread->cpuTime += newCpuTime;
}
