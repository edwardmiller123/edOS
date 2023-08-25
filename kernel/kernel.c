#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../cpu/isr.h"

void main()
{
  // set up the idt
  isrInstall();
  clear_screen();
  print_string_at("edOS.v0.1\n", 0, 0);
  initPS2Keyboard();
  
  // test an interrupt
  // __asm__ volatile("int $2");
  handleKeyboardInput();
}
