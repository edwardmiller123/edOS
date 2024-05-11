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

void thread1() {
  for (int i = 0; i < 200; i++) {
    kPrintString("T1: ");
    printInt(i);
    kPrintString(" | ");
  }
}

void thread2() {
  for (int i = 0; i < 200; i++) {
    kPrintString("T2: ");
    printInt(i);
    kPrintString(" | ");
  }
}

void main()
{
  // set up interrupt handling
  initPIC();
  // clear boot sector messages
  clear_screen();
  kPrintStringAt("================================================================================\n", 0, 0);
  kPrintStringAt("=============================| Welcome to edOS! |===============================\n", 0, 1);
  kPrintStringAt("================================================================================\n", 0, 2);
  initPS2Keyboard(0);
  initThreads();
  initTimer();
  initTSS();

  createThread(&thread1, "THREAD1");
  createThread(&thread2, "THREAD2");

  for (int i = 0; i < 200; i++) {
    kPrintString("main: ");
    printInt(i);
    kPrintString(" | ");
  }

  // make the jump to user mode
  // enterUserMode();


}
