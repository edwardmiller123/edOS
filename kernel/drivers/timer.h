#ifndef TIMER
#define TIMER

// timer updates the global runtime using the PIT. It also calls the scheduler
// to switch between threads
int timer(struct registers r);

// initTimer initialises the PIT
void initTimer();

// kGetPITCount returns the total system up time
int kGetPITCount();

// kSleep waits until the given time in milliseconds has passed
void kSleep(int ms);

#endif