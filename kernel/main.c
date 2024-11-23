#include "drivers/screen.h"
#include "log.h"
#include "drivers/keyboard.h"
#include "threads.h"
#include "drivers/timer.h"
#include "interrupts/isr.h"
#include "interrupts/tss.h"
#include "drivers/syscall.h"
#include "../userspace/userspace.h"
#include "../stdlib/stdlib.h"

// enterUserMode is our wrapper assembly function that jumps to user mode and then
// starts the shell
extern int enterUserMode();

void main()
{
  // briefly enable interrupts to catch any errors before we start 
  // initialising other things
  sti();
  // clear boot sector messages
  clear_screen();
  kPrintStringAt("================================================================================\n", 0, 0);
  kPrintStringAt("=============================| Welcome to edOS! |===============================\n", 0, 1);
  kPrintStringAt("================================================================================\n", 0, 2);

  setLogLevel(DEBUG);

  cli();
  // initial kernel setup
  initPIC();
  initTSS();
  initThreads();
  initPS2Keyboard();
  initTimer();
  sti();

  // a little dirty but this ensures the timer has a chance to tick and initialise the threads
  // correctly before we enter usermode.
  kSleep(10);

  // TODO: There is still stack corruption happening as we sometimes get UB so still need
  // to do some investigating

  // make the jump to user mode
  kLogInfo("Entering user mode");
  createUThread(&initUserSpace);
}
