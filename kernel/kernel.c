#include "kernel_checks.h"

void main()
{
  clear_screen();
  print_string_at("edOS.v1.0\n", 0, 1);
  testControllers();
}
