#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../cpu/isr.h"
#include "../shell/shell.h"
#include "utils.h"

void main()
{
  // set up interrupt handling
  initPIC();
  clear_screen();
  print_string_at("Welcome to edOS!\n", 0, 0);
  initPS2Keyboard();

  runShell();
}
