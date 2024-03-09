#ifndef SYSCALL
#define SYSCALL

// timerHandler updates the global count using the PIT
// TODO: actually implement this
void timerHandler(struct registers r);

// initTimer initialises the PIT
void initTimer();

#endif