#include "threads.h"
#include "../drivers/screen.h"
#include "../consts.h"
#include "../mem.h"
#include "../helpers.h"
#include "../interrupts/tss.h"
#include "../interrupts/isr.h"
#include "../../stdlib/stdlib.h"

#define THREAD_STACK_SIZE (void *)0x1800

// User mode threads implementation
// There will be one scheduler that runs in ring 0 and schedules both kernel and user threads.
// User threads will be identical to kernel threads with the addition of a user mode stack
// and the thread entry func will be in user space. All threads will live in the same list and be switched
// on irq0 with different behaviour based on the thread type (kernel or user). When switching to a user thread,
// we put the new stack to switch to on the old stack and let the iret take care of the switch (rather than doing it manually
// like we do for kernel threads). On every thread switch (no matter which type) we update the current kernel stack in the tss.

// Thread implementation:
// The timer calls the scheduler which updates the running thread TCB with the
// chosen thread. The scheduler will not make the switch as this is instead the
// job of the general irq handler.
// The general irq handler will always put the registers stored in the running thread TCB onto the stack
// and store the old ones. We also update esp0 in the TSS during the switch. Then when any IRQ
// returns it will resume the new thread if a thread switch has taken place.

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
static TCB *runningThread = NULL;

// activeThreads contains all TCBs of currently in progress threads.
static threadList activeThreads;

// A simple way of creating unique thread ids. This number never decreases.
static int idCount = 1;

// newId increments and returns the thread id counter
int newId()
{
    int id = idCount;
    idCount++;
    return id;
}

// add adds a new TCB into the circular thread list
void add(TCB *newThread)
{
    cli();
    TCB *lastThread = activeThreads.head;
    if (activeThreads.size == 0)
    {
        // if list is empty the new thread becomes the head
        activeThreads.head = newThread;
        // when there is just one element the heads next element is itself
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
    activeThreads.head->previousThread = newThread;
    newThread->previousThread = lastThread;
    activeThreads.size += 1;
    sti();
}

// remove removes the given TCB from the list of active threads
// and frees its memory. Ensure to call this with interrupts disabled
// to avoid strange race conditions.
void remove(TCB *threadToRemove)
{
    TCB *threadBehind = threadToRemove->previousThread;
    TCB *threadInfront = threadToRemove->nextThread;
    threadBehind->nextThread = threadInfront;
    threadInfront->previousThread = threadBehind;

    // finally free the memory used for the TCB
    activeThreads.size--;
    kFree(threadToRemove->state);
    kFree(threadToRemove);
}

// initThreads creates a mostly empty TCB for the initial thread. This gets
// populated on the first timer tick after it has been created.
void initThreads()
{
    activeThreads.size = 0;
    // allocate space for the default TCB.
    TCB *defaultThread = kMalloc(sizeof(TCB));

    defaultThread->status = ACTIVE;

    defaultThread->initTime = 0;
    // starting value of esp for the kernel. This gets set on the first thread switch
    defaultThread->kStackPos = NULL;
    defaultThread->id = newId();
    defaultThread->type = UNSET;
    // allocate memory for the registers of the default thread. We set the values
    // to all be zero as they are filled when the first interrupt fires.
    defaultThread->state = (struct registers *)kMalloc(sizeof(struct registers));
    // Only set the segemnt registers as we need to read ds on the first thread switch.
    // All other values are set when the switch takes place.
    struct registers regs = {
        .ds = KERNEL_DATA_SEG,
        .cs = KERNEL_CODE_SEG,
        .useresp = NULL,
    };
    *defaultThread->state = regs;

    add(defaultThread);
    runningThread = defaultThread;
    // the space allocated is never freed as the default thread is always running
}

// exit marks the thread for removal and then spins. All the marked threads are then removed in the irq handler
// after any thread switches have occured. e.g thread is finished -> calls exit() -> exit marks thread as done then hangs
// -> we switch out of the done thread into another -> all marked threads are removed -> new thread resumes.
// exit removes the currently running thread from the list and never returns.
void exit()
{
    runningThread->status = FINISHED;
    while (1)
    {
        // wait until this thread is switched out into the void
        ;
    };
}

// findNewKernelStack finds a new stack position by iterating through the thread list looking for
// the highest kernel stack address so far and incrementing it by the OS stack size;
void *findNewKernelStack()
{
    void *highestStackPosition = (void *)KERNEL_STACK;
    TCB *thread = activeThreads.head->nextThread;
    while (thread != (activeThreads.head))
    {
        if ((thread->kStackPos) > highestStackPosition)
        {
            highestStackPosition = thread->kStackPos;
        }
        thread = thread->nextThread;
    }
    return (void *)((int)highestStackPosition + (int)THREAD_STACK_SIZE);
}

// findNewUserStack finds a new stack position by iterating through the thread list looking for
// the highest user stack address so far;
void *findNewUserStack()
{
    void *highestUStackPosition = (void *)DEFAULT_STACK;
    TCB *thread = activeThreads.head->nextThread;
    while (thread != (activeThreads.head))
    {
        if ((thread->state->useresp) > highestUStackPosition)
        {
            highestUStackPosition = thread->state->useresp;
        }
        thread = thread->nextThread;
    }
    return (void *)((int)highestUStackPosition + (int)THREAD_STACK_SIZE);
}

// createKThread creates a new TCB and adds it to a linked list of active threads. Takes
// the function to run in the new thread.
// It is left up to the scheduler and IRQ handler to actually execute the new thread.
// New threads are created with a max space of 6kb (0x1800) for now.
// The memory allocated for a TCB gets freed when we remove it from the list.
void createKThread(void *threadFunction)
{
    // create the new TCB. This gets freed when the thread gets terminated.
    TCB *newThread = kMalloc(sizeof(TCB));

    // set the thread entry point
    newThread->threadEntry = threadFunction;
    newThread->id = newId();
    newThread->status = ACTIVE;
    newThread->type = KERNEL;

    // set new thread stack position
    newThread->kStackPos = findNewKernelStack();

    // set our exit function as the return address for the thread.
    setAtAddress(&exit, newThread->kStackPos);

    // now set up the initial register values for the new thread
    newThread->state = (struct registers *)kMalloc(sizeof(struct registers));
    struct registers regs = {
        .stubesp = 0,
        .ds = KERNEL_DATA_SEG,
        .edi = 0,
        .esi = 0,
        // set ebp to the same value as esp when the thread starts since this is what
        // would happen if we called a c func normally
        .ebp = newThread->kStackPos,
        .unused = 0,
        // This first esp value "shouldn't" matter as it gets set from the TSS anyway by the IRET.
        .ebx = 0,
        .edx = 0,
        .edx = 0,
        .ecx = 0,
        .eax = 0,
        // can ignore the intNumber and errCode as the general irq handler calls the EOI
        // using the original register values anyway
        // set eip to our new threads entry point
        .eip = newThread->threadEntry,
        .cs = KERNEL_CODE_SEG,
        .eflags = 514, // this has the IF and reserved bits set
        .useresp = NULL,
        .ss = 0,
    };

    *newThread->state = regs;

    add(newThread);
}

// createUThread creates a new TCB for a usermode thread and adds it to a linked list of active threads. Takes
// the function to run in the new thread.
// It is left up to the scheduler and IRQ handler to actually execute the new thread.
// New threads are created with a max space of 6kb (0x1800) for now.
// The memory allocated for a TCB gets freed when we remove it from the list.
void createUThread(void *threadFunction)
{
    // create the new TCB. This gets freed when the thread gets terminated.
    TCB *newThread = kMalloc(sizeof(TCB));
    // set the thread entry point
    newThread->threadEntry = threadFunction;
    newThread->id = newId();
    newThread->status = ACTIVE;
    newThread->type = USER;

    // set new kernel thread stack position
    newThread->kStackPos = findNewKernelStack();

    // set new user thread stack position
    void *uStackPos = findNewUserStack();

    // set our exit function as the return address for the thread.
    setAtAddress(&exit, uStackPos);

    // now set up the initial register values for the new thread
    // this differs from the kThreads as we need to set up to return to usermode
    newThread->state = (struct registers *)kMalloc(sizeof(struct registers));
    struct registers regs = {
        .stubesp = 0,
        .ds = USER_DATA_SEG_RPL3,
        .edi = 0,
        .esi = 0,
        // set ebp to the same value as esp when the thread starts since this is what
        // would happen if we called a c function normally
        .ebp = uStackPos,
        .unused = 0,
        // This first esp value "shouldn't" matter as it gets set from the TSS anyway by the IRET.
        .ebx = 0,
        .edx = 0,
        .edx = 0,
        .ecx = 0,
        .eax = 0,
        // can ignore the intNumber and errCode as the general irq handler calls the EOI
        // using the original register values anyway
        // set eip to our new threads entry point
        .eip = newThread->threadEntry,
        .cs = USER_CODE_SEG_RPL3,
        .eflags = 514, // this has the IF and reserved bits set
        .useresp = uStackPos,
        .ss = USER_DATA_SEG_RPL3,
    };

    *newThread->state = regs;

    add(newThread);
}

// cleanUpFinished iterates through the thread list and removes any with the
// "FINISHED" status
void cleanUpFinished()
{
    TCB *thread = activeThreads.head->nextThread;
    TCB *currentThread = NULL;
    while (thread != (activeThreads.head))
    {
        currentThread = thread;
        thread = thread->nextThread;
        if (currentThread->status == FINISHED)
        {
            remove(currentThread);
        }
    }
}

// threadSwitch initiates a thread switch by saving and loading the state of the cpu from
// the runningThread TCB global. The values of all the registers and the stack of the interrupted
// function are saved either to runningThread or to the previous TCB in the thread list if we are switching
// to a new thread.
// For thread switches that were initiated in ring 0, we push the new threads stack value
// onto the old stack so it can be popped off into esp by the irq handler and hence switched to as the first
// action when threadSwitch returns. The register values for the new thread are then pushed onto the new stack so
// they can also be restored by the irq handler after esp has been switched.
// For ring 3 thread switches we also push the new user stack and the user segment registers so they can be
// be popped into the correct registers for us by the iret.
void threadSwitch(struct registers r)
{
    // The TCB created for the default thread wont have a type so
    // we infer the type for the value of ds on the stack
    if (runningThread->type == UNSET)
    {
        if (r.ds == KERNEL_DATA_SEG) {
            runningThread->type = KERNEL;
        } else if (r.ds == USER_DATA_SEG_RPL3) {
            runningThread->type = USER;
        } else {
            kPrintString("unknown ds value, halting\n");
            hlt();
        }
    }
    // This is the saved stack frame position of the irq stub. We add 36 to the stubEsp
    // as this is the sum of all registers plus a value for ds pushed onto the stack.
    void *oldIrqStackFrame = (void *)(r.stubesp + 36);

    // this is the value of esp before the interrupt fired i.e the previously running threads stack.
    // it is 20 bytes above the irq stubs stack frame as 8 bytes are taken by the error code and int number
    // and then the extra 12 (20 for user threads) by the various values pushed on the stack during the interrupt.

    void *callerEsp = (void *)(oldIrqStackFrame + 20);
    // For user threads we also have to account for the user esp and ss values on the stack
    // The value of ds will determine which ring the thread we are switching from is in.
    if (r.ds == (unsigned int)USER_DATA_SEG_RPL3)
    {
        callerEsp = (void *)(oldIrqStackFrame + 28);
    }
    // Save the old thread's registers and stack.
    // If there are multiple threads then the state of the old thread gets
    // stored in the previous threads TCB since the runningThread global indicates
    // either the currently running thread or the next thread to be executed.
    // We do not need to make any changes for user mode threads here as the user stack gets saved
    // along with the rest of the registers.
    if (activeThreads.size > 1)
    {
        // NOTE: this only works for pre-emptive scheduling. In reality a thread may need more time
        // so would need to switch back to itself
        *runningThread->previousThread->state = r;
        runningThread->previousThread->kStackPos = callerEsp;
    }
    else
    {
        *runningThread->state = r;
        runningThread->kStackPos = callerEsp;
        // If we are switching back to the same thread then all the correct values are already on the stack
        // so we can just return.
        return;
    }

    // update esp0 in the TSS
    updateRing0Stack((void *)runningThread->kStackPos);

    // this is the new value of the stack which we switch the stack pointer to when the irq stub resumes.
    // This could either be the same value as before or a new one if a thread switch has occurred
    void *newStubEsp = (void *)(runningThread->kStackPos - 56);
    if (runningThread->type == USER)
    {
        newStubEsp = (void *)(runningThread->kStackPos - 64);
    }

    // we put the new stack value to switch to on the old stack so it can be popped off into esp
    // when the irq stub resumes.
    setAtAddress(newStubEsp, (void *)(r.stubesp - 4));

    // From here we proceed to restore the register values by either putitng them back on the old stack
    // or by putting new values on the new stack.

    // the irq stubs stack frame in the new thread.
    void *newIrqStackFrame = (void *)(runningThread->kStackPos - 20);
    if (runningThread->type == USER)
    {
        newIrqStackFrame = (void *)(runningThread->kStackPos - 28);
    }

    // Get the registers from runningThread and push them onto the stack.
    // These will then be popped off by the irq stub.

    // next set up the stack that the irq stub expects

    // First we add what iret expects

    // Set the return address for the irq accordingly.
    // The return address is at +8 above the stack frame as it is the first thing iret
    // expects to pop off the stack after we have cleaned up the pushed error code and
    // pushed IRQ number.

    // set EIP value
    setAtAddress(runningThread->state->eip, newIrqStackFrame + 8);

    // set cs value
    setAtAddress(runningThread->state->cs, newIrqStackFrame + 12);

    // set EFLAGS value.
    setAtAddress(runningThread->state->eflags, newIrqStackFrame + 16);

    // when switching to a user mode thread we need to set up the stack to allow us to
    // return from ring 0 to ring 3
    if (runningThread->type == USER)
    {
        // set the useresp value (value of esp pushed on the stack before the irq fired)
        setAtAddress(runningThread->state->useresp, newIrqStackFrame + 20);

        // set ss value (stack segment)
        setAtAddress(runningThread->state->ss, newIrqStackFrame + 24);
    }

    // Next implement a pusha instruction. These values are below the stack pointer
    // we set as they are in the stack frame of our general irq handler

    // set eax value
    setAtAddress(runningThread->state->eax, newIrqStackFrame - 4);

    // set ecx value
    setAtAddress(runningThread->state->ecx, newIrqStackFrame - 8);

    // set edx value
    setAtAddress(runningThread->state->edx, newIrqStackFrame - 12);

    // set ebx value
    setAtAddress(runningThread->state->ebx, newIrqStackFrame - 16);

    // move an extra 4 bytes as the irq handler skips popping esp
    // e.g skip setAtAddress(something, irqStackFrame - 24);

    // set ebp value
    setAtAddress(runningThread->state->ebp, newIrqStackFrame - 24);

    // set esi value
    setAtAddress(runningThread->state->esi, newIrqStackFrame - 28);

    // set edi value
    setAtAddress(runningThread->state->edi, newIrqStackFrame - 32);

    // finally the irq handler expects a value to load the data segment
    // register with (and others) which we are using ebx to do.
    // set ebx value
    setAtAddress(runningThread->state->ds, newIrqStackFrame - 36);

    // after every switch we need to remove finished threads. This must be done after a
    // switch has taken place so we dont accidently over write a threads state.
    cleanUpFinished();
}

// schedule updates the active TCB with the next thread to schedule. It does not
// make the switch itself as this is done by the general IRQ handler.
void schedule(int currentUptime)
{
    // first update the cpuTime of the currently running thread
    int newCpuTime = currentUptime - (runningThread->initTime);
    runningThread->cpuTime += newCpuTime;
    // for now use a round robin schedule so we switch thread on every tick
    // dont switch if we havent initialised the threads yet
    if (activeThreads.size > 0)
    {
        // make the switch.
        runningThread = runningThread->nextThread;
    }
}