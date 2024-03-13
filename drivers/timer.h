#ifndef TIMER
#define TIMER

// timerHandler updates the global count using the PIT

void timerHandler(struct registers r);

// setTimerFrequency sets how often irq0 fires to the 
// given frequency which must be a 16 bit number
void setTimerFrequency(short targetFreq);

// initTimer initialises the PIT
void initTimer();

// kGetRunTime returns the total system up time
int kGetSystemUptime();

#endif