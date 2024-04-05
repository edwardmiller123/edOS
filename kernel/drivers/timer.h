#ifndef TIMER
#define TIMER

// timer updates the global runtime using the PIT. It also calls the scheduler
// to switch between threads (TODO)
void timer(struct registers r);

// setTimerFrequency sets how often irq0 fires to the 
// given frequency which must be a 16 bit number
void setTimerFrequency(short targetFreq);

// initTimer initialises the PIT
void initTimer();

// kGetRunTime returns the total system up time
int kGetPITCount();

// kSleep waits until the given time in seconds has passed
void kSleep(int seconds);

#endif