#include "../kernel/I_O_asm_helpers.h"
#include "screen.h"
#include "keyboard.h"
#include "../cpu/isr.h"

// TODO: This should be a global command queue of keycodes that other things can
// read from but for now is just for shift key.
int heldKey;

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

unsigned char keyCodeToAscii(int keyCode, int heldKey)
{
  // Note: this is scan code set 1
  if (heldKey == 0x2A)
  {
    switch (keyCode)
    {
    case 0x10:
      return 'Q';
    case 0x11:
      return 'W';
    case 0x12:
      return 'E';
    case 0x13:
      return 'R';
    case 0x14:
      return 'T';
    case 0x15:
      return 'Y';
    case 0x16:
      return 'U';
    case 0x17:
      return 'I';
    case 0x18:
      return 'O';
    case 0x19:
      return 'P';
    case 0x1A:
      return '{';
    case 0x1B:
      return '}';
    case 0x1C:
      return '\n';
    case 0x1E:
      return 'A';
    case 0x1F:
      return 'S';
    case 0x20:
      return 'D';
    case 0x21:
      return 'F';
    case 0x22:
      return 'G';
    case 0x23:
      return 'H';
    case 0x24:
      return 'J';
    case 0x25:
      return 'K';
    case 0x26:
      return 'L';
    case 0x2C:
      return 'Z';
    case 0x2D:
      return 'X';
    case 0x2E:
      return 'C';
    case 0x2F:
      return 'V';
    case 0x30:
      return 'B';
    case 0x31:
      return 'N';
    case 0x32:
      return 'M';
    case 0x39:
      return ' ';
    }
  }
  else
  {
    switch (keyCode)
    {
    case 0x10:
      return 'q';
    case 0x11:
      return 'w';
    case 0x12:
      return 'e';
    case 0x13:
      return 'r';
    case 0x14:
      return 't';
    case 0x15:
      return 'y';
    case 0x16:
      return 'u';
    case 0x17:
      return 'i';
    case 0x18:
      return 'o';
    case 0x19:
      return 'p';
    case 0x1A:
      return '[';
    case 0x1B:
      return ']';
    case 0x1C:
      return '\n';
    case 0x1E:
      return 'a';
    case 0x1F:
      return 's';
    case 0x20:
      return 'd';
    case 0x21:
      return 'f';
    case 0x22:
      return 'g';
    case 0x23:
      return 'h';
    case 0x24:
      return 'j';
    case 0x25:
      return 'k';
    case 0x26:
      return 'l';
    case 0x2C:
      return 'z';
    case 0x2D:
      return 'x';
    case 0x2E:
      return 'c';
    case 0x2F:
      return 'v';
    case 0x30:
      return 'b';
    case 0x31:
      return 'n';
    case 0x32:
      return 'm';
    case 0x39:
      return ' ';
    }
  }
  return 0;
}

// printKeyToScreen prints the corresponding character for a given key code.
void printKeyToScreen(int keyCode)
{
  int character;
  switch (keyCode)
  {
  case 0x0E:
    // backspace
    character = 0x0E;
    break;
  case 0x2A:
    // shiftKey
    heldKey = 0x2A;
    break;
  case 0xAA:
    // shiftKey released
    heldKey = 0xAA;
    break;
  default:
    character = keyCodeToAscii(keyCode, heldKey);
  }
  if (character != 0)
  {
    print_char(character, 0);
  }
}

// handleKeyboardInput reads the keyboard data port and prints the corresponding character
// to the screen.
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

  // Test the controller and ports are working.
  testPS2Controller();

  // Enable first ps/2 port.
  port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xAE);

  // flush output buffer
  port_byte_in(PS2_DATA_PORT);

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
}
