#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../cpu/isr.h"
#include "utils.h"

void main()
{
  // set up interrupt handling
  initPIC();
  clear_screen();
  print_string_at("edOS.v0.4\n", 0, 0);
  initPS2Keyboard();

  // Mock shell
  char *stdInBuffer;
  int shellRunning = 1;
  while (shellRunning = 1)
  {
    stdInBuffer = readKeyBuffer();
    for (int i = 0; i < strLen(stdInBuffer); i++)
    {
      if (stdInBuffer[i] == 'z')
      {
        shellRunning = 0;
      }
    }
  }
  printString(stdInBuffer);
}
