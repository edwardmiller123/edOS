#ifndef TIMER
#define TIMER

// initTimer initialises the PIT
void initTimer();

// kGetPITCount returns the total system up time
int kGetPITCount();

// kSleep waits until the given time in milliseconds has passed
void kSleep(int ms);

#endif