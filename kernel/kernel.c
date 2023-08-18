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
  
  // enable interrupts
  __asm__ volatile("sti");
  handleKeyboardInput();
}
