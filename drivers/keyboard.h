#ifndef KEYBOARD
#define KEYBOARD

#include "../kernel/I_O_asm_helpers.h"
#include "screen.h"

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_AND_COMMAND_REGISTER 0x64

// testPS2Controller runs a simple test to see if the ps2 controller is working.
void testPS2Controller()
{
  // test ps2 controller
  port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xAA);
  int testResult = port_byte_in(PS2_DATA_PORT);
  switch (testResult)
  {
  case 0x55:
    print_string("PS/2 Controller Operational.\n");
    break;
  case 0xFC:
    print_string("PS/2 failed with fault.\n");
    break;
  default:
    print_string("PS/2 failed with unexpected result.\n");
    break;
  }
  printInt(testResult);
}

int checkResponse(int expectedResonse)
{
  int response = port_byte_in(PS2_DATA_PORT);
  if (response == expectedResonse)
  {
    return 1;
  }
  return response;
}

int initPS2Controller()
{

  // Enable first ps/2 port.
  port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xAE);

  // Select scan code set 2. Command byte sets scan code set and the data byte chooses the set.
  // port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xF0);
  // port_byte_out(PS2_DATA_PORT, 2);

  // Set the keyboard to send scan codes.
  // port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xF4);
  return checkResponse(0);
}

void updateCommandQueue()
{
}

#endif