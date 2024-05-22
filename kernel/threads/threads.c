#include "threads.h"
#include "../consts.h"
#include "../mem.h"
#include "../interrupts/tss.h"
#include "../interrupts/isr.h"
#include "../../stdlib/stdlib.h"

// TODO: add user mode threads after significantly more thought into
// the subject

// Thread implementation:
// The timer calls the scheduler which updates the running thread TCB with the
// chosen thread. The scheduler will not make the switch as this is instead the 
// job of the general irq handler.
// The general irq handler will always put the registers stored in the running thread TCB onto the stack
// and store the old ones. There wil also be a function to update esp0 in the TSS. Then when any IRQ
// returns it will (hopefully) resume the new thread if a thread switch has taken place.
// We do not need to switch the stack pointer ourselves (apart from in the tss) as this will
// be taken care of by the interrupt.

// A linked list containing the active threads
typedef struct threadList
{
    int size;
    TCB *head;
} __attribute__((packed)) threadList;

// switchThread is only defined in the header since we link against the assembly
// file for the function body

// runningThread is the TCB of the thread currently executing (or just about to be executed)
// The IRQ handler always loads its registers from whatever is stored in this variable.
static TCB *runningThread;

// activeThreads contains all TCBs of currently in progress threads.
static threadList activeThreads;

// addToThreadList adds a new TCB into the circular thread list;
void addToThreadList(TCB *newThread)
{ 
    TCB *lastThread = activeThreads.head;
    if (activeThreads.size == 0)
    {
        // if list is empty the new thread becomes the head
        activeThreads.head = newThread;
    }
    else
    {
        // iterate through to find the TCB just before the head
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
    newThread->previousThread = lastThread;
    activeThreads.size += 1;
}

// removeThreadFromList removes the given TCB from the list of active threads 
// and frees its memory
void removeThreadFromList(TCB *threadToRemove) {
    if (activeThreads.size == 0)
    {
        return;
    }
    // iterate through to find the TCB just before the head
        TCB *lastThread = activeThreads.head;
        TCB *thread = activeThreads.head->nextThread;
        while (thread != activeThreads.head)
        {
            // store the current thread so we can insert the new one just before the head.
            lastThread = thread;
            thread = thread->nextThread;
        }
        // remove the thread by joining the two either side
        lastThread->nextThread = threadToRemove->nextThread;
        // finally free the memory used for the TCB
        kFree(threadToRemove->state);
        kFree(threadToRemove);
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

// threadWrapper calls the threads entry function and removes it from the list
// of active threads once it completes
void threadWrapper() {
    // cast the thread entry address to a pointer
    void ( *entryFuncPtr)(void) = runningThread->threadEntry;
    // call the thread entry function
    ( *entryFuncPtr)();
    kPrintString("thread finished. removing\n");
    // thread has finished so remove this thread from the list
    removeThreadFromList(runningThread);
}

// createKThread creates a new TCB and adds it to a linked list of active kernel threads. Takes
// the function to run in the new thread and an ID (just for debugging for now).
// It is left up to the scheduler and IRQ handler to actually execute the new thread. 
// New threads are created with a max space of 6kb (0x1800) for now.
void createKThread(void *threadFunction, char * id)
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

    // TODO: Setup fake stack for new thread

    addToThreadList(newThread);
}

// threadSwitch initiates a thread switch by setting up the stack to allow the interrupt
// the interrupt handler to actually make the switch when it returns.
// This is done by getting the values for the registers from the global runningThread 
// TCB and putting them on the stack. These are then popped off the stack during the 
// interrupt into the corresponding registers. Finally when the interrupt returns the new
// value for the stack along with the retrun address are loaded by the iret 
// instruction and and voila. This function all updates the esp0 value in the TSS.
void threadSwitch(struct registers r) {
    // Save the old threads registers
    struct registers * regs = kMalloc(sizeof(struct registers));
    memoryCopy(&r, regs, sizeof(struct registers));
    // If there are multiple threads then the state of the old thread gets
    // stored in the previous threads TCB since runningThread global indicates 
    // either the currently running thread or the next thread to be executed.
    if (activeThreads.size > 1) {
        runningThread->previousThread->state = regs;
    } else {
        runningThread->state = regs;
    }

    // update esp0 in the TSS
    updateRing0Stack((void *)runningThread->state->useresp);

    // Get the registers from runningThread and push them onto the stack.
    // These will then be popped off by
    // the interrupt handler and the thread switch will be taken care off
    // by the processor.

    // next set up the stack that the irq handler expects

    // First we add what iret expects

    // set the return address for the irq accordingly
    void ** EIP = runningThread->state->useresp + 4;
    *EIP = &threadWrapper;
    if (activeThreads.size > 1) {
        *EIP = &threadWrapper;
    } else {
        // if only one thread then there is no switch hence just
        // pick up where we left off.
        *EIP = runningThread->state->eip;
    }

    void ** cs = runningThread->state->useresp + 8;
    *cs = runningThread->state->cs;

    void ** EFLAGS = runningThread->state->useresp + 12;
    *EFLAGS = runningThread->state->eflags; 

    // useresp is the stack we are currently on (old thread) 
    // so we change it to the new address to switch to when the interrupt returns
    void ** useresp = runningThread->state->useresp + 16;
    *useresp = runningThread->threadStackTop;

    void ** ss = runningThread->state->useresp + 20;
    *ss = runningThread->state->ss;

    // Next implement a pusha instruction. These values are below the stack pointer
    // we set as they are "inside" our general irq handler

    void ** eaxPtr = runningThread->state->useresp - 8;
    *eaxPtr = runningThread->state->eax;

    void ** ecxPtr = runningThread->state->useresp - 12;
    *ecxPtr = runningThread->state->ecx;

    void ** edxPtr = runningThread->state->useresp - 16;
    *edxPtr = runningThread->state->edx;

    void ** ebxPtr = runningThread->state->useresp - 20;
    *ebxPtr = runningThread->state->ebx;

    // move an extra 4 bytes as the irq handler skips popping esp

    void ** ebpPtr = runningThread->state->useresp - 28;
    *ebpPtr = runningThread->state->ebp;

    void ** esiPtr = runningThread->state->useresp - 32;
    *esiPtr = runningThread->state->esi;

    void ** ediPtr = runningThread->state->useresp - 36;
    *ediPtr = runningThread->state->edi;

    // finally the irq handler expects a value to load the segment registers with
    ebxPtr = runningThread->state->useresp - 40;
    *ebxPtr = KERNEL_DATA_SEG;
}

// schedule updates the active TCB with the next thread to schedule. It does not 
// make the switch itself as this is done by the general IRQ handler.
void schedule(int currentUptime)
{
    // first update the cpuTime of the currently running thread
    int newCpuTime = currentUptime - (runningThread->initTime);
    runningThread->cpuTime += newCpuTime;
    // for now use a round robin schedule so we switch thread on every tick
    // no point switching threads if there is only 1
    if (activeThreads.size > 1)
    {   
        kPrintString("switch\n");
        // make the switch.
        runningThread = runningThread->nextThread;
    }
}