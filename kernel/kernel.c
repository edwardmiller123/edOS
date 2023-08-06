#include "../drivers/screen.h"
#include "../drivers/keyboard.h"

void main()
{
  clear_screen();
  print_string_at("edOS.v0.1\n", 0, 0);
  initPS2Keyboard();
  handleKeyboardInput();
}
