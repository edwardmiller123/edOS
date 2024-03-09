#include "../interrupts/isr.h"
#include "screen.h"

// timerHandler updates the global count using the PIT
// TODO: actually implement this
void timerHandler(struct registers r) {
    kPrintString("tick");
}

// initTimer initialises the PIT
void initTimer() {
    registerInterruptHandler(32, timerHandler);
}