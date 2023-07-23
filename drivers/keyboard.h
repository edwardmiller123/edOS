#include "../kernel/I_O_asm_helpers.h"

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_AND_COMMAND_REGISTER 0x64

// testPS2Controller runs a simple test to see if the ps2 controller is working.
int testPS2Controller()
{
  // test ps2 controller
  port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xAA);
  int testResult = port_byte_in(PS2_DATA_PORT);
  switch (testResult)
  {
  case 0x55:
    return 1;
  case 0xFC:
    return 0;
  default:
    return 2;
  }
}

void updateCommandQueue()
{
}