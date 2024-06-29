#include "threads.h"
#include "../drivers/screen.h"
#include "../consts.h"
#include "../mem.h"
#include "../helpers.h"
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
void removeThreadFromList(TCB *threadToRemove)
{
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
    // allocate memory for the registers of the default thread. We set the values
    // to all be zero as they are filled when the first interrupt fires.
    defaultThread->state = (struct registers *)kMalloc(sizeof(struct registers));
    struct registers regs = {0};
    *defaultThread->state = regs;

    addToThreadList(defaultThread);
    runningThread = defaultThread;
}

// threadWrapper calls the threads entry function and removes it from the list
// of active threads once it completes
void threadWrapper()
{
    kPrintString("called thread\n");
    // cast the thread entry address to a pointer
    void (*entryFuncPtr)(void) = runningThread->threadEntry;
    // call the thread entry function
    (*entryFuncPtr)();
    kPrintString("thread finished. removing\n");
    // thread has finished so remove this thread from the list
    removeThreadFromList(runningThread);
}

// createKThread creates a new TCB and adds it to a linked list of active kernel threads. Takes
// the function to run in the new thread and an ID (just for debugging for now).
// It is left up to the scheduler and IRQ handler to actually execute the new thread.
// New threads are created with a max space of 6kb (0x1800) for now.
// The memory alloctaed for a thread gets freed when we remove it from the list.
void createKThread(void *threadFunction, char *id)
{
    // create the new TCB. This gets freed when the thread gets terminated.
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

    // now set up the initial register values for the new thread
    newThread->state = (struct registers *)kMalloc(sizeof(struct registers));
    struct registers regs = {
        // calleresp is only used by the irq handler and is set byu the calling routine anyway so
        // just set it to 0 as the initial value doesnt matter
        .calleresp = 0,
        .ds = KERNEL_DATA_SEG,
        .edi = 0,
        .esi = 0,
        .ebp = 0,
        .esp = 0,
        // This first esp value "shouldn't" matter as it gets set from the TSS anyway by the IRET.
        .ebx = 0,
        .edx = 0,
        .edx = 0,
        .ecx = 0,
        .eax = 0,
        // can ignore the intNumber and errCode as the general irq handler calls the EOI
        // using the original regiuster values anyway
        .eip = &threadWrapper,
        .cs = KERNEL_CODE_SEG,
        .eflags = 513, // this has the CF and IF bits set
        .useresp = newThread->threadStackTop,
        .ss = KERNEL_DATA_SEG,
    };

    *newThread->state = regs;

    addToThreadList(newThread);
}

// threadSwitch initiates a thread switch by setting up the stack to allow
// the interrupt handler to actually make the switch when it returns.
// This is done by getting the values for the registers from the global runningThread
// TCB and putting them on the stack. These are then popped off the stack during the
// interrupt into the corresponding registers. Finally when the interrupt returns the new
// value for the stack along with the return address are loaded by the iret
// instruction and voila. This function also updates the esp0 value in the TSS.
void threadSwitch(struct registers r)
{
    // Save the old threads registers
    // If there are multiple threads then the state of the old thread gets
    // stored in the previous threads TCB since runningThread global indicates
    // either the currently running thread or the next thread to be executed.
    if (activeThreads.size > 1)
    {
        *runningThread->previousThread->state = r;
    }
    else
    {
        *runningThread->state = r;
    }

    // This is the stack frame position of the irq stub. We add 36 to the callerEsp
    // as this is the sum of all registers plus a value for ds pushed onto the stack.
    void *irqStackFrame = (void *)(runningThread->state->calleresp + 36);
    // update esp0 in the TSS
    // TODO: fix this when doing user mode threads
    updateRing0Stack((void *)runningThread->threadStackTop);

    // Get the registers from runningThread and push them onto the stack.
    // These will then be popped off by
    // the interrupt handler and the thread switch will be taken care off
    // by the processor.

    // next set up the stack that the irq handler expects

    // First we add what iret expects

    // Set the return address for the irq accordingly.
    // The return address is at +8 above the stack frame as it is the first thing iret
    // expects to pop off the stack after we have cleaned up the pushed error code and
    // pushed IRQ number.
    void *EIP = irqStackFrame + 8;
    if (activeThreads.size > 1)
    {
        setAtAddress(&threadWrapper, EIP);
    }
    else
    {
        // if only one thread then there is no switch hence just
        // pick up where we left off.
        setAtAddress(runningThread->state->eip, EIP);
    }

    // set cs value
    setAtAddress(runningThread->state->cs, irqStackFrame + 12);

    // set EFLAGS value. We must manually flip the IF (Interrupt Enable Flag) bit
    // as normally sti would flip it.
    setAtAddress(runningThread->state->eflags, irqStackFrame + 16);

    // set the useresp value (value of esp pushed on the stack before the irq fired)
    setAtAddress(runningThread->state->useresp, irqStackFrame + 20);

    // set ss value (stack segment)
    setAtAddress(runningThread->state->ss, irqStackFrame + 24);


    // Next implement a pusha instruction. These values are below the stack pointer
    // we set as they are in the stack frame of our general irq handler

    // set eax value
    setAtAddress(runningThread->state->eax, irqStackFrame - 8);

    // set ecx value
    setAtAddress(runningThread->state->ecx, irqStackFrame - 12);

    // set edx value
    setAtAddress(runningThread->state->edx, irqStackFrame - 16);

    // set ebx value
    setAtAddress(runningThread->state->ebx, irqStackFrame - 20);

    // move an extra 4 bytes as the irq handler skips popping esp
    // e.g skip setAtAddress(something, irqStackFrame - 24);

    // set ebp value
    setAtAddress(runningThread->state->ebp, irqStackFrame - 28);

    // set esi value
    setAtAddress(runningThread->state->esi, irqStackFrame - 32);

    // set edi value
    setAtAddress(runningThread->state->edi, irqStackFrame - 36);

    // finally the irq handler expects a value to load the data segment
    // register (and others) with which we are using ebx to do.
    // set ebx value
    setAtAddress(runningThread->state->ds, irqStackFrame - 40);
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
        // make the switch.
        runningThread = runningThread->nextThread;
    }
}