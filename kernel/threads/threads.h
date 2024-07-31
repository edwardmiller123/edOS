#ifndef THREADS
#define THREADS

// a way of storing the values of the registers when 
// calling the interrupt handler from assembly.
struct registers {
  // just before the call instruction we push esp on the stack. This value will actually
  // be 4 bytes higher than the actual value since the push modifes esp in the process.
  unsigned int stubesp;
  // the data segment register
  unsigned int ds;
  // general purpose registers we will push onto the stack
  unsigned int edi;
  unsigned int esi;
  unsigned int ebp;
  unsigned int unused; // this is always empty as the pusha instruction skips esp
  unsigned int ebx;
  unsigned int edx;
  unsigned int ecx;
  unsigned int eax;
  unsigned int intNumber;
  unsigned int errCode;

  // registers automattically pushed onto the stack by the processor
  // These are what iret expects to be on the stack
  unsigned int eip;
  unsigned int cs;
  unsigned int eflags;
  // useresp and ss are only pushed if a priviledge change occurs
  unsigned int useresp; 
  unsigned int ss;
}__attribute__((packed));

enum Status {
    ACTIVE,
    FINISHED,
};

// TCB (thread controller block) holds information about a running
// thread.
typedef struct TCB {
    void * threadStackPos;

    // the saved values of the registers at the time the thread was interrupted
    struct registers * state;
    struct TCB * nextThread;
    struct TCB * previousThread;

    // address of the threads entry function
    void * threadEntry;

    // the time stamp when the thread was created
    int initTime;

    // amount of processing time spent on this thread
    int cpuTime;
    int id;
    enum Status status;

} __attribute__((packed)) TCB;

// newId increments and returns the thread id counter
int newId();

// addThread adds a new TCB into the circular thread list;
void add(TCB * newThread);

// removeThread removes the given TCB from the list of active threads 
// and frees its memory
void remove(TCB *threadToRemove);

// initThreads just creates a TCB for the default thread
void initThreads();

// exit removes the currently running thread from the list and never returns.
void exit();

// createKThread creates a new TCB and adds it to a linked list of active kernel threads. Takes
// the function to run in the new thread and an ID (just for debugging for now).
// It is left up to the scheduler and IRQ handler to actually execute the new thread. 
// New threads are created with a max space of 6kb (0x1800) for now.
void createKThread(void *threadFunction);

// cleanUpFinished iterates through the thread list and removes any with the
// "DONE" status
void cleanUpFinished();

void threadSwitch(struct registers r);

// schedule updates the active TCB with the next thread to schedule. It does not 
// make the switch itself as this is done by the general IRQ handler.
void schedule(int currentUptime);

#endif