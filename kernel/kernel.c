#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../interrupts//isr.h"
#include "../shell/shell.h"
#include "../drivers/syscall.h"
#include "utils.h"

void main()
{
  // set up interrupt handling
  initPIC();
  clear_screen();
  kPrintStringAt("================================================================================\n", 0, 0);
  kPrintStringAt("=============================| Welcome to edOS! |===============================\n", 0, 1);
  kPrintStringAt("================================================================================\n", 0, 2);
  initPS2Keyboard(0);

  runShell();
}
