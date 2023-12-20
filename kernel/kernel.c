#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../cpu/isr.h"
#include "../shell/shell.h"
#include "../drivers/syscall.h"
#include "utils.h"

void main()
{
  // set up interrupt handling
  initPIC();
  clear_screen();
  print_string_at("================================================================================\n", 0, 0);
  print_string_at("=============================| Welcome to edOS! |===============================\n", 0, 1);
  print_string_at("================================================================================\n", 0, 2);
  initPS2Keyboard(0);
  initSyscalls();

  runShell();
}
