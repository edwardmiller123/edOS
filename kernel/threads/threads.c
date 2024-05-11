#include "threads.h"
#include "../consts.h"
#include "../mem.h"
#include "../interrupts/isr.h"
#include "../../stdlib/stdlib.h"

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
    if (activeThreads.size == 0)
    {
        // if list is empty the new thread becomes the head
        activeThreads.head = newThread;
    }
    else
    {
        // iterate through to find the TCB just before the head
        TCB *lastThread = activeThreads.head;
        TCB *thread = activeThreads.head->nextThread;
        while (thread != activeThreads.head)
        {
            // store the current thread so we can insert the new one just before the head.
            lastThread = thread;
            thread = thread->nextThread;
        }
        lastThread->nextThread = newThread;
    }
    // the new thread always links back to the list head.
    // if the list was previously empty then this just links to itself
    newThread->nextThread = activeThreads.head;
    activeThreads.size += 1;
}

// initThreads just sets the TCB for the default thread
void initThreads()
{
    activeThreads.size = 0;
    TCB *defaultThread = kMalloc(sizeof(TCB));
    defaultThread->initTime = 0;
    // starting value of esp for the kernel
    defaultThread->threadStackTop = (void *)DEFAULT_STACK;
    defaultThread->id = "MAIN";
    addToThreadList(defaultThread);
    runningThread = defaultThread;
}

// createThread creates a new TCB and adds it to a linked list of active threads. Takes
// the function to run in the new thread and an ID (just for debugging for now).
// It is left up to the scheduler to actually execute the new thread. New threads are created with
// a max space of 6kb (0x1800) for now.
void createThread(void *threadFunction, char * id)
{
    // create the new TCB
    TCB *newThread = kMalloc(sizeof(TCB));
    // set the thread entry point
    newThread->threadEntry = threadFunction;
    newThread->id = id;

    // Find a new stack position by itterating through the thread list looking for
    // highest address so far;
    int highestStackPosition = DEFAULT_STACK;
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

    // When we switch thread we never return to the interrupt handler since the stack has changed.
    // We must therefore return from the interrupt ourselves and soo set up the new stack 
    // the way the switch thread function expects. The actual values on the stack are mostly unimportant 
    // since this is just for initialisation

    // When we load the new stack top we are actually in the middle of the 
    // stack frame of the threadSwitch function. Hence all the values we expect to pop off
    // will be at higher addresses with the return address at the highest

    // confusing pointer notation but its because we are storing memory addresses
    void ** ediPtr = newThread->threadStackTop ;
    *ediPtr = 0;

    // dont need to add esi as we dont push it during thread switch

    void ** ebxPtr = (newThread->threadStackTop) + 4 ;
    *ebxPtr = 0;

    // ebp should be the position of the stack "before" entering switchThread.
    // i.e an address higher than the positions of all the pushed regsters the function uses
    // but lower than the return address
    void ** ebpPtr = (newThread->threadStackTop) + 8 ;
    *ebpPtr = newThread->threadStackTop + 8;

    // next set up the stack as if returning from an interrupt
    // the order the interrupt is expecting is: 
    // ebx, edi, esi, ebp, (skip esp), ebx (again), 
    // edx, ecx, eax, ss (this should be an extra 8 bytes above to account for "add esp, 8" instruction
    // esp, EFLAGS, cs, EIP (return address) 

    ebxPtr = (newThread->threadStackTop) + 12 ;
    // this contains the gdt code segment as its what we set the segment registers with
    *ebxPtr = KERNEL_DATA_SEG;

    // from here we are effectivly imitating a "popa" instruction
    ediPtr = newThread->threadStackTop + 16 ;
    *ediPtr = 0;

    void ** esiPtr = (newThread->threadStackTop) + 20;
    *esiPtr = 0;

    ebpPtr = (newThread->threadStackTop) + 24 ;
    *ebpPtr = 0;

    // for when we want to skip registers (in this case esp)
    void ** blankPtr = (newThread->threadStackTop) + 28;
    *blankPtr = 0;

    ebxPtr = (newThread->threadStackTop) + 32 ;
    *ebxPtr = 0;

    void ** edxPtr = (newThread->threadStackTop) + 36;
    *edxPtr = 0;

    void ** ecxPtr = (newThread->threadStackTop) + 40;
    *ecxPtr = 0;

    void ** eaxPtr = (newThread->threadStackTop) + 44;
    *eaxPtr = 0;

    // account for the "add esp, 8" instruction hence the gap on the stack

    // next set up the stack how iret expects
    void ** ssPtr = (newThread->threadStackTop) + 56;
    *ssPtr = 0x10; // gdt data segment

    void ** espPtr = (newThread->threadStackTop) + 60;
    *espPtr = newThread->threadStackTop; // the new stack position

    void ** eflagsPtr = (newThread->threadStackTop) + 64;
    *eflagsPtr = 0; // the current status of the cpu. Just set as 0 for now

    void ** csPtr = (newThread->threadStackTop) + 68;
    *csPtr = KERNEL_CODE_SEG; // gdt code segment

    // finally the return address of the thread entry function
    void ** EIP = (newThread->threadStackTop) + 72;
    *EIP = newThread->threadEntry;

    addToThreadList(newThread);
}

// schedule updates the running threads TCB and switches to a new thread based on
// every call (in future it will be based on the cpuTime it has received).
// takes the current system uptime as an arg to avoid circular dependancies.
// If a thread switch occurs schedule never returns as it is expected to be called by an interrupt handler. It proceeds to handle
// the interrupt itself in switchThread when making the switch
void schedule(int currentUptime)
{
    // first update the cpuTime of the currently running thread
    int newCpuTime = currentUptime - (runningThread->initTime);
    runningThread->cpuTime += newCpuTime;
    // for now use a round robin schedule so we switch thread on every tick
    // no point switching threads if there is only 1
    if (activeThreads.size > 1)
    {   
        // create a new pointer as we need to switch the tracked running thread before
        // callimng switchThread
        TCB * lastThread = runningThread;
        // signal that the timer interrupt is finished to allow more to fire. Probably not
        // the best idea to do this before the handler is finished but interrupts are disabled
        // until switchThread completes so we are probably safe
        kPrintString("switch\n");
        // signal the end of the timer interrupt
        PICsendEOI(0);
        // make the switch. We dont return to the scheduler from here
        runningThread = runningThread->nextThread;
        switchThread(lastThread, lastThread->nextThread);
    }
}