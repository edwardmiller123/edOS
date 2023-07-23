#include "kernel_checks.h"

void main()
{
  clear_screen();
  print_string_at("edOS.v1.0\n", 0, 1);
  testControllers();
  // int ps2response = initPS2Controller();
  // print_string(intToString(ps2response));
  print_string(intToString(16));
}
