#include "../kernel/I_O_asm_helpers.h"
#include "screen.h"
#include "keyboard.h"
#include "../cpu/isr.h"

void testController()
{
  // test ps2 controller
  int testResult = sendCommand(0xAA, PS2_DATA_PORT, PS2_STATUS_AND_COMMAND_REGISTER);
  switch (testResult)
  {
  case 0x55:
    printString("Controller Operational\n");
    break;
  case 0xFC:
    printString("Controller failed with fault\n");
    break;
  default:
    printString("Controller failed with unexpected result\n");
    break;
  }
}

void testPort1()
{
  // test ps2 port 1
  port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xAB);
  int testResult = port_byte_in(PS2_DATA_PORT);
  switch (testResult)
  {
  case 0x00:
    printString("Port 1 Operational\n");
    break;
  default:
    printString("Port 1 failed with unexpected result\n");
    printInt(testResult);
    break;
  }
}

void testPort2()
{
  // test ps2 port 1
  port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xA9);
  int testResult = port_byte_in(PS2_DATA_PORT);
  switch (testResult)
  {
  case 0x00:
    printString("Port 2 Operational\n");
    break;
  default:
    printString("Port 2 failed with unexpected result\n");
    printInt(testResult);
    break;
  }
}

// testPS2Controller runs a simple test to see if the ps2 controller is working.
void testPS2Controller()
{
  // test ps2 controller and ports.
  printString("Running PS/2 Controller tests...\n");
  testController();
  testPort1();
  testPort2();
}

// printKeyToScreen prints the corresponding character for a given key code.
void printKeyToScreen(int keyCode)
{
  switch (keyCode)
  {
  case 0x10:
    print_char('Q', 1);
    break;
  case 0x11:
    print_char('W', 0);
    break;
  case 0x12:
    print_char('E', 0);
    break;
  case 0x13:
    print_char('R', 0);
    break;
  case 0x14:
    print_char('T', 0);
    break;
  case 0x15:
    print_char('Y', 0);
    break;
  case 0x16:
    print_char('U', 0);
    break;
  case 0x17:
    print_char('I', 0);
    break;
  case 0x18:
    print_char('O', 0);
    break;
  case 0x1A:
    print_char('[', 0);
    break;
  case 0x1B:
    print_char(']', 0);
    break;
  case 0x1C:
    print_char('\n', 0);
    break;
  case 0x1E:
    print_char('A', 0);
    break;
  case 0x1F:
    print_char('S', 0);
    break;
  case 0x20:
    print_char('D', 0);
    break;
  case 0x21:
    print_char('F', 0);
    break;
  case 0x22:
    print_char('G', 0);
    break;
  case 0x23:
    print_char('H', 0);
    break;
  case 0x24:
    print_char('J', 0);
    break;
  case 0x25:
    print_char('K', 0);
    break;
  case 0x26:
    print_char('L', 0);
    break;
  case 0x2C:
    print_char('Z', 0);
    break;
  case 0x2D:
    print_char('X', 0);
    break;
  case 0x2E:
    print_char('C', 0);
    break;
  case 0x2F:
    print_char('V', 0);
    break;
  case 0x30:
    print_char('B', 0);
    break;
  case 0x31:
    print_char('N', 0);
    break;
  case 0x32:
    print_char('M', 0);
    break;
  case 0x39:
    print_char(' ', 0);
    break;
  default:
    break;
  }
}

void handleKeyboardInput(struct registers r)
{
  int keyCode = port_byte_in(PS2_DATA_PORT);
  printKeyToScreen(keyCode);
}

void initPS2Keyboard()
{
  registerInterruptHandler(33, handleKeyboardInput);

  // Disable first ps/2 port.
  port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xAD);

  // Disable second ps/2 port.
  port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xA7);

  // flush output buffer
  port_byte_in(PS2_DATA_PORT);

  // Test the controller and ports are working.
  testPS2Controller();

  // Enable first ps/2 port.
  port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xAE);

  // Disable keyboard scanning.
  int disableScanningResponse = sendCommand(0xF5, PS2_DATA_PORT, PS2_DATA_PORT);
  switch (disableScanningResponse)
  {
  case 0xFA:
    printString("Keyboard scanning disabled\n");
    break;
  case 0:
    printString("Keyboard not found (empty response)\n");
    break;
  default:
    printString("Error disabling scan codes\n");
  }

  // Reset keyboard
  int resetResponse = sendCommand(0xFF, PS2_DATA_PORT, PS2_DATA_PORT);
  switch (resetResponse)
  {
  case 0xFA:
    printString("Keyboard reset\n");
    break;
  case 0:
    printString("Keyboard not found (empty response)\n");
    break;
  default:
    printString("Error resetting keyboard\n");
  }

  int enableScanningResponse = sendCommand(0xF4, PS2_DATA_PORT, PS2_DATA_PORT);
  switch (enableScanningResponse)
  {
  case 0xFA:
    printString("Keyboard scanning enabled\n");
    break;
  case 0:
    printString("Keyboard not found (empty response)\n");
    break;
  default:
    printString("Error enabling keyboard scanning\n");
  }
}
