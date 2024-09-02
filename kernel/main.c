#include "drivers/screen.h"
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
    kPrintString("T2: ");
    printInt(i);
    kSleep(1);
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
  cli();
  // initial kernel setup
  initPIC();
  initTSS();
  initPS2Keyboard(0);
  initThreads();
  initTimer();
  sti();

  // make the jump to user mode
  enterUserMode();

}
