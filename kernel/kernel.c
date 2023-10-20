#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../cpu/isr.h"

void main()
{
  // set up interrupt handling
  initPIC();
  clear_screen();
  print_string_at("edOS.v0.4\n", 0, 0);
  initPS2Keyboard();

  int shellRunning = 1;
  while (shellRunning = 1)
  {
    char *keyBuffer = readKeyBuffer();
    printString(keyBuffer);
  }
}
