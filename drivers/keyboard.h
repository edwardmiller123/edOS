#ifndef KEYBOARD
#define KEYBOARD

#include "../kernel/I_O_asm_helpers.h"
#include "screen.h"

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_AND_COMMAND_REGISTER 0x64

void testController() {
  // test ps2 controller
  port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xAA);
  int testResult = port_byte_in(PS2_DATA_PORT);
  switch (testResult)
  {
  case 0x55:
    print_string("Controller Operational.\n");
    break;
  case 0xFC:
    print_string("Controller failed with fault.\n");
    break;
  default:
    print_string("Controller failed with unexpected result.\n");
    printInt(testResult);
    break;
  }
}

void testPort1() {
  // test ps2 port 1
  port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xAB);
  int testResult = port_byte_in(PS2_DATA_PORT);
  switch (testResult)
  {
  case 0x00:
    print_string("Port 1 Operational.\n");
    break;
  default:
    print_string("Port 1 failed with unexpected result.\n");
    printInt(testResult);
    break;
  }
}

void testPort2() {
  // test ps2 port 1
  port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xA9);
  int testResult = port_byte_in(PS2_DATA_PORT);
  switch (testResult)
  {
  case 0x00:
    print_string("Port 2 Operational.\n");
    break;
  default:
    print_string("Port 2 failed with unexpected result.\n");
    printInt(testResult);
    break;
  }
}

// testPS2Controller runs a simple test to see if the ps2 controller is working.
void testPS2Controller()
{
  // test ps2 controller and ports.
  print_string("Running PS/2 Controller tests...\n");
  testController();
  testPort1();
  testPort2();

}

void initPS2Controller()
{

  // Disable second ps/2 port.
  port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xA7);

  // Enable first ps/2 port.
  port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xAE);

  // Select scan code set 2. Command byte sets scan code set and the data byte chooses the set.
  // port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xF0);
  // port_byte_out(PS2_DATA_PORT, 2);

  // Set the keyboard to send scan codes.
  // port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xF4);
}

void updateCommandQueue()
{
}

#endif