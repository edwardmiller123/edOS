#include "../drivers/screen.h"
#include "../drivers/keyboard.h"

void main()
{
  clear_screen();
  print_string_at("edOS.v0.1\n", 0, 0);
  testPS2Controller();
  initPS2Controller();
  print_string("\n");

  // for (int i = 0; i < 100000; i++) {
  //   printInt(i);
  //   print_string("\n");
  // }
  printInt(0x00);

}
