#ifndef THREADS
#define THREADS

#include "types.h"
#include "drivers/keyboard.h"

// Use the stack canary to tell if a threads stack has grown to large. We set this at the end of the
// allocated stack space for every thread. When switching to a new thread if this value is missing 
// then we know the stack has grown to large so we exit.
#define STACK_CANARY 0xbeef

typedef enum Status
{
    ACTIVE,
    FINISHED,
} Status;

typedef enum ThreadType
{
    UNKNOWN,
    KERNEL,
    USER,
} ThreadType;

// TCB (thread control block) holds information about a running
// thread.
typedef struct TCB
{
    void *kStackPos;

    // the saved values of the registers at the time the thread was interrupted
    struct registers *state;
    struct TCB *nextThread;
    struct TCB *previousThread;

    // address of the threads entry function
    void *threadEntry;

    // the time stamp when the thread was created
    int initTime;

    // amount of processing time spent on this thread
    int cpuTime;
    int id;
    Status status;
    // Location of the stack canaries at the end of the allocated stack spaces;
    // This is effectivly the threads stack limit
    void * kCanaryAddress;
    void * uCanaryAddress;

    // if a thread is in focus then it is actively being used by the user and so
    // input should go to the associated stdin buffer
    KeyboardInput *stdin;
    int inFocus;

} __attribute__((packed)) TCB;

// makeInFocus sets the currently running thread to store keyboard input in its stdin buffer
void makeInFocus();

// initThreads just creates a TCB for the default thread
void initThreads();

// exit marks the thread for removal and then spins. All the marked threads are then removed in the irq handler
// after any thread switches have occurred. e.g thread is finished -> calls exit() -> exit marks thread as done then hangs
// -> we switch out of the done thread into another -> all marked threads are removed -> new thread resumes.
void exit();

// createKThread creates a new TCB and adds it to a linked list of active kernel threads. Takes
// the function to run in the new thread and an ID (just for debugging for now).
// It is left up to the scheduler and IRQ handler to actually execute the new thread.
// New threads are created with a max space of 6kb (0x1800) for now.
void createKThread(void *threadFunction);

// createUThread creates a new TCB for a usermode thread and adds it to a linked list of active threads. Takes
// the function to run in the new thread.
// It is left up to the scheduler and IRQ handler to actually execute the new thread.
// New threads are created with a max space of 6kb (0x1800) for now.
// The memory allocated for a TCB gets freed when we remove it from the list.
void createUThread(void *threadFunction);

void threadSwitch(struct registers r);

// schedule updates the active TCB with the next thread to schedule. It does not
// make the switch itself as this is done by the general IRQ handler.
void schedule(int currentUptime);

#endif