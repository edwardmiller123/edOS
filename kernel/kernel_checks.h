#ifndef KERNEL_CHECKS
#define KERNEL_CHECKS

#include "../drivers/screen.h"
#include "../drivers/keyboard.h"

void testControllers() {
  int test = testPS2Controller();
  if (test == 1)
  {
    print_string("PS/2 Controller Operational.\n");
  }
  else if (test == 0)
  {
    print_string("PS/2 failed with fault.\n");
  }
  else if (test == 2)
  {
    print_string("PS/2 failed with unexpected result.\n");
  }
}

#endif