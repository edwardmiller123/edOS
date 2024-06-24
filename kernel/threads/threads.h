#ifndef THREADS
#define THREADS

// a way of storing the values of assembly registers in c.
struct registers {
  // the data segment register
  unsigned int ds;
  // general purpose registers we will push onto the stack
  unsigned int edi;
  unsigned int esi;
  unsigned int ebp;
  unsigned int esp;
  unsigned int ebx;
  unsigned int edx;
  unsigned int ecx;
  unsigned int eax;
  unsigned int errCode;
  unsigned int intNumber;

  // registers automattically pushed onto the stack by the processor
  // These are what iret expects to be on the stack
  unsigned int eflags;
  unsigned int eip;
  unsigned int cs;
  // useresp and ss are only pushed if a priviledge change occurs
  unsigned int useresp; 
  unsigned int ss;
}__attribute__((packed));

// TCB (thread controller block) holds information about a running
// thread.
typedef struct TCB {
    void * threadStackTop;
    struct registers * state;
    struct TCB * nextThread;
    struct TCB * previousThread;
    void * threadEntry;
    int initTime; // the time stamp when the thread was created
    int cpuTime; // amount of processing time spent on this thread
    char * id;
} __attribute__((packed)) TCB;

// switchThread switches the current thread to the one in the given TCB
// extern void switchThread(TCB * currentThread, TCB * nextThread);

// addToThreadList adds a new TCB into the circular thread list;
void addToThreadList(TCB * newThread);

// removeThreadFromList removes the given TCB from the list of active threads 
// and frees its memory
void removeThreadFromList(TCB *threadToRemove);

// initThreads just creates a TCB for the default thread
void initThreads();

// threadWrapper calls the threads entry function and removes it from the list
// of active threads once it completes
void threadWrapper();

// createKThread creates a new TCB and adds it to a linked list of active kernel threads. Takes
// the function to run in the new thread and an ID (just for debugging for now).
// It is left up to the scheduler and IRQ handler to actually execute the new thread. 
// New threads are created with a max space of 6kb (0x1800) for now.
void createKThread(void *threadFunction, char * id);

void threadSwitch(struct registers r);

// schedule updates the active TCB with the next thread to schedule. It does not 
// make the switch itself as this is done by the general IRQ handler.
void schedule(int currentUptime);

#endif