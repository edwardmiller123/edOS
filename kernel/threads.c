#include "threads.h"
#include "log.h"
#include "drivers/screen.h"
#include "drivers/keyboard.h"
#include "consts.h"
#include "mem.h"
#include "interrupts/tss.h"
#include "interrupts/isr.h"
#include "../stdlib/stdlib.h"

// Thread Implementation:
// A thread is the current state of a CPU core at any one time e.g the values stored in its registers.
// This state (along with other info) is stored in a Thread Control Block (TCB) struct. Threads are scheduled using a
// pre-emptive system. When the timer interrupt fires, the scheduler chooses the next TCB from the activeThreads linked list 
// and sets it to be the global runningThread variable. The switchThread function is then called which stores the state of the CPU
// in the old threads TCB and (if the thread has changed) loads the new desired state from the runningThread TCB into the CPU.
// At the end of every thread switch we run a clean up function to remove any finished threads that are left spinning in the FINISHED state.
// The scheduling of both kernel and user threads are handled by the same scheduler which runs in the kernel

// NOTE: This usage of the global runningThread will have to change if we support multi-core in the future since the scheduler will have to handle
// multiple threads on different cores. A solution could be to run only run the timer (and hence the scheduler) on a single core and simply have a global
// list/struct of "running threads" with a map of each core to a specific thread.

// A linked list containing the active threads
typedef struct threadList
{
    int size;
    TCB *head;
} __attribute__((packed)) threadList;

// runningThread is the TCB of the thread currently executing (or just about to be executed)
// The IRQ handler always loads its registers from whatever is stored in this variable.
static TCB *runningThread = NULL;

// activeThreads contains all TCBs of currently in progress threads.
static threadList activeThreads;

// A simple way of creating unique thread ids. This number never decreases.
static int idCount = 1;

// makeInFocus sets the currently running thread to store keyboard input in its stdin buffer
void makeInFocus()
{
    runningThread->inFocus = 1;
    switchActiveKeyBuffer(runningThread->stdin);
    int args[] = {runningThread->id};
    kLogf(INFO, "Thread $ has the focus", args, 1);
}

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
    int args[] = {
        newThread->id,
        newThread->state->ds,
        newThread->state->cs,
        newThread->kStackPos,
        newThread->state->useresp};
    kLogf(INFO, "Added thread $, ds: $, cs: $, K Stack: $, U Stack: $", args, 5);
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

    int args[] = {
        threadToRemove->id,
        threadToRemove->state->ds,
        threadToRemove->state->cs,
        threadToRemove->kStackPos,
        threadToRemove->state->useresp};

    activeThreads.size--;
    // finally free the memory used for the TCB
    kFree(threadToRemove->stdin);
    kFree(threadToRemove->state);
    kFree(threadToRemove);
    kLogf(INFO, "Removed thread $, ds: $, cs: $, K Stack: $, U Stack: $", args, 5);
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
    // allocate memory for the registers of the default thread. We set the values
    // to all be zero as they are filled when the first interrupt fires.
    defaultThread->state = (struct registers *)kMalloc(sizeof(struct registers));
    // Only set the segment registers as we need to read ds on the first thread switch.
    // All other values are set when the switch takes place.
    struct registers regs = {
        .ds = KERNEL_DATA_SEG,
        .cs = KERNEL_CODE_SEG,
        .useresp = NULL,
    };
    *defaultThread->state = regs;

    KeyboardInput *stdin = newKeyboardInput();
    defaultThread->stdin = stdin;

    // the default thread has both canaries as NULL since it doesnt really have a stack limit for now.
    // In future this should just be the standard size since there is always the danger of colliding with the heap with the 
    // current memory layout.
    defaultThread->kCanaryAddress = NULL;
    defaultThread->uCanaryAddress = NULL;

    add(defaultThread);
    runningThread = defaultThread;
    kLogInfo("Initialised default thread");
    // the space allocated is never freed as the default thread is always running
    makeInFocus();
}

// exit marks the thread for removal and then spins. All the marked threads are then removed in the irq handler
// after any thread switches have occurred. e.g thread is finished -> calls exit() -> exit marks thread as done then hangs
// -> we switch out of the done thread into another -> all marked threads are removed -> new thread resumes.
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
    void *highestStackPosition = KERNEL_STACK;
    TCB *thread = activeThreads.head->nextThread;
    while (thread != (activeThreads.head))
    {
        if ((int)thread->kStackPos > (int)highestStackPosition)
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
    void *highestUStackPosition = USER_STACK;
    TCB *thread = activeThreads.head->nextThread;
    while (thread != (activeThreads.head))
    {
        if ((thread->state->useresp) > (int)highestUStackPosition)
        {
            highestUStackPosition = thread->state->useresp;
        }
        thread = thread->nextThread;
    }
    return (void *)((int)highestUStackPosition + (int)THREAD_STACK_SIZE);
}

// createKThread creates a new TCB and adds it to a linked list of active threads
// waiting to be scheduled. Takes the function to run in the new thread.
// New threads are created with a max space of 0x3000 (12 kib) for now.
// The memory allocated for a TCB gets freed when we remove it from the list.
void createKThread(void *threadFunction)
{
    // create the new TCB. This gets freed when the thread gets terminated.
    TCB *newThread = kMalloc(sizeof(TCB));

    // set the thread entry point
    newThread->threadEntry = threadFunction;
    newThread->id = newId();
    newThread->status = ACTIVE;

    // set new thread stack position
    void * kStackPos = findNewKernelStack();
    newThread->kStackPos = kStackPos;

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

    // dont need to allocate a stdin buffer for kernel threads since they wont
    // take user input directly
    newThread->stdin = NULL;

    add(newThread);

    // Set the Stack canary
    int kernelCanaryPosition = (int)kStackPos - (int)THREAD_STACK_SIZE;
    setAtAddress((int)STACK_CANARY, kernelCanaryPosition);

    newThread->kCanaryAddress = kernelCanaryPosition;
    newThread->uCanaryAddress = NULL;
}

// createUThread creates a new TCB for a usermode thread and adds it to a list of active threads
// waiting to be scheduled. Takes the function to run in the new thread.
// New threads are created with a max space of 12kib (0x3000) for now.
// The memory allocated for a TCB gets freed when we remove it from the list.
void createUThread(void *threadFunction)
{
    // create the new TCB. This gets freed when the thread gets terminated.
    TCB *newThread = kMalloc(sizeof(TCB));
    // set the thread entry point
    newThread->threadEntry = threadFunction;
    newThread->id = newId();
    newThread->status = ACTIVE;

    // set new kernel thread stack position
    void * kStackPos = findNewKernelStack();
    newThread->kStackPos = kStackPos;

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

    KeyboardInput *stdin = newKeyboardInput();
    newThread->stdin = stdin;

    // Set the Stack canaries
    void * kernelCanaryPosition = (void*)((int)kStackPos - (int)THREAD_STACK_SIZE);
    setAtAddress((int)STACK_CANARY, kernelCanaryPosition);

    newThread->kCanaryAddress = kernelCanaryPosition;

    int userCanaryPosition = (int)uStackPos - (int)THREAD_STACK_SIZE;
    setAtAddress((int)STACK_CANARY, userCanaryPosition);

    newThread->uCanaryAddress = userCanaryPosition;

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

// threadType determines the thread type based off the passed in value
// of ds.
ThreadType getThreadType(unsigned int ds)
{
    if (ds == (unsigned int)USER_DATA_SEG_RPL3)
    {
        return USER;
    }
    if (ds == (unsigned int)KERNEL_DATA_SEG)
    {
        return KERNEL;
    }

    return UNKNOWN;
}

// threadSwitch initiates a thread switch by saving and loading the state of the CPU from
// the runningThread TCB global. The irq handler pushes the register values on to the stack, executes the handling
// code then pops the values back of the stack into the registers to complete the interrupt. The flow for the thread switch
// therefor consists of the following steps:
// 1. Check the thread type from ds (e.g user or kernel)
// 2. Save the old register values in the last threads TCB (this is just the runningThread if there are no others in the active list)
// 3. Update the TSS with the new kernel stack value
// 4. Put the value of the new threads kernel stack at the correct place on the old threads kernel stack. The IRQ handler will pop this 
// into esp once the handling code returns.
// 5. Put the new register values at the correct places on the new kernel stack. These will then be popped off into the registers
// by the IRQ handler.
// 6. Check for and clean up any threads with the FINISHED status.
// For user threads the only difference is that the TCB contains a kernel and a user stack. If a thread is identified
// as a user thread then we put some extra values onto the new stack, namely the user stack value and the value of ss.
// The value of the user stack is loaded into esp by the iret instruction when the IRQ handler returns.
void threadSwitch(struct registers r)
{
    // The value of ds will determine which ring the threads involved in the switch are in.
    ThreadType oldThreadType = getThreadType(r.ds);
    ThreadType newThreadType = getThreadType(runningThread->state->ds);

    if (oldThreadType == UNKNOWN)
    {
        int args[] = {r.ds};
        kLogf(FATAL, "Last thread has unknown type. ds: $", args, 1);
        return;
    }
    if (newThreadType == UNKNOWN)
    {
        int args[] = {runningThread->state->ds, runningThread->id};
        kLogf(FATAL, "New thread has unknown type. ds: $, Thread: $", args, 2);
        return;
    }

    // Check the stack canaries
    int kCanary = (int)NULL;
    if (runningThread->kCanaryAddress != NULL) {
        kCanary = (int)getAtAddress(runningThread->kCanaryAddress);
    }
    if ((void *)kCanary != NULL && kCanary != STACK_CANARY) {
        int args[] = {runningThread->id};
        kLogf(ERROR, "Thread $ kernel stack limit reached. Exiting", args, 1);
        return;
    }

    int uCanary = (int)NULL;
    if (runningThread->uCanaryAddress != NULL) {
        uCanary = (int)getAtAddress(runningThread->uCanaryAddress);
    }
    if ((void *)uCanary != NULL && uCanary != STACK_CANARY) {
        int args[] = {runningThread->id};
        kLogf(ERROR, "Thread $ user stack limit reached. Exiting", args, 1);
        exit();
        return;
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
    if (oldThreadType == USER)
    {
        callerEsp = (void *)(oldIrqStackFrame + 28);
    }

    int args[] = {
        runningThread->id,
        (int)callerEsp,
        runningThread->kStackPos,
        r.useresp,
        runningThread->state->useresp,
    };
    kLogf(DEBUG, "Switching to thread $, kStack: $ -> $, uStack: $ -> $", args, 5);

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

    // TODO: in user mode this is being set to a huge number
    // update esp0 in the TSS
    updateRing0Stack(runningThread->kStackPos);

    // this is the new value of the stack which we switch the stack pointer to when the irq stub resumes.
    // This could either be the same value as before or a new one if a thread switch has occurred
    void *newStubEsp = (void *)(runningThread->kStackPos - 56);
    if (newThreadType == USER)
    {
        newStubEsp = (void *)(runningThread->kStackPos - 64);
    }

    // we put the new stack value to switch to on the old stack so it can be popped off into esp
    // when the irq stub resumes.
    setAtAddress(newStubEsp, (void *)(r.stubesp - 4));

    // From here we proceed to restore the register values by either putting them back on the old stack
    // or by putting new values on the new stack.

    // the irq stubs stack frame in the new thread.
    void *newIrqStackFrame = (void *)(runningThread->kStackPos - 20);
    if (newThreadType == USER)
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
    if (newThreadType == USER)
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
    // for now use a round robin schedule so we switch thread on every tick
    // dont switch if we havent initialised the threads yet

    if (activeThreads.size > 0)
    {
        // make the switch.
        runningThread = runningThread->nextThread;
        int args[] = {runningThread->id};
        kLogf(DEBUG, "Thread switch scheduled. Next will be $", args, 1);
    }
}