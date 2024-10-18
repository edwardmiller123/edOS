#include "drivers/screen.h"
#include "log.h"
#include "drivers/keyboard.h"
#include "threads/threads.h"
#include "drivers/timer.h"
#include "interrupts/isr.h"
#include "interrupts/tss.h"
#include "drivers/syscall.h"
#include "../stdlib/stdlib.h"

// enterUserMode is our wrapper assembly function that jumps to user mode and then
// starts the shell
extern int enterUserMode();

void thread2() {
  for (int i = 1; i <= 5; i++) {
    kPrintString("KT2: ");
    printInt(i);
    kSleep(1);
  }
  return;
}

void thread3() {
  for (int i = 1; i <= 5; i++) {
    kPrintString("KT3: ");
    printInt(i);
    kSleep(2);
  }
  return;
}

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

  setLogLevel(INFO);

  cli();
  // initial kernel setup
  initPIC();
  initTSS();
  initThreads();
  initPS2Keyboard(0);
  initTimer();
  sti();

  // a little dirty but this ensures the timer has a chance to tick and initialise the threads
  // correctly before we enter usermode.
  kSleep(10);

  // make the jump to user mode
  kLogInfo("Entering user mode");
  enterUserMode();

}
