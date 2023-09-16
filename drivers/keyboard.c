#include "../kernel/I_O_asm_helpers.h"
#include "screen.h"
#include "keyboard.h"
#include "../cpu/isr.h"

int heldKey, queuePosition, waitingForKeyCode;
int keyCodeQueue[6];

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
    case 0x02:
      return '!';
    case 0x03:
      return '"';
    case 0x04:
      return '£';
    case 0x05:
      return '$';
    case 0x06:
      return '%';
    case 0x07:
      return '^';
    case 0x08:
      return '&';
    case 0x09:
      return '*';
    case 0x0A:
      return '(';
    case 0x0B:
      return ')';
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
    case 0x02:
      return '1';
    case 0x03:
      return '2';
    case 0x04:
      return '3';
    case 0x05:
      return '4';
    case 0x06:
      return '5';
    case 0x07:
      return '6';
    case 0x08:
      return '7';
    case 0x09:
      return '8';
    case 0x0A:
      return '9';
    case 0x0B:
      return '0';
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

// singleKeyCodeHandler converts single keycode keyboard commands into the correspinding action.
void singleKeyCodeHandler(int keyCode)
{
  int character = 0;
  character = keyCodeToAscii(keyCode, heldKey);
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
  }
  if (character != 0)
  {
    print_char(character, 0);
  }
}

// trimQueue takes the keyCode queue and returns a new array with only the non 0 values. If there
// are none it just returns the queue unchanged.
int *trimQueue(int keyCodes[6])
{
  if (keyCodes[5] == 0)
  {
    return keyCodes;
  }

  int *trimmedQueue;
  for (int i = 0; i < 6; i++)
  {
    if (keyCodes[i] == 0)
    {
      break;
    }
    trimmedQueue[i] = keyCodes[i];
  }
  return trimmedQueue;
}

// multipleKeyCodeHandler converts keyboard commands consistsing of more than one keycode into
// corresponding actions.
void multipleKeyCodeHandler()
{
  int *trimmedQueue = trimQueue(keyCodeQueue);
  if (compareIntArrays(trimmedQueue, (int[]){0xE0, 0x4D}, 2))
  {
    // cursor left
    moveCursor(0);
  }
  else if (compareIntArrays(trimmedQueue, (int[]){0xE0, 0x4B}, 2))
  {
    // cursor right
    moveCursor(1);
  }
}

// keycodesToActions translates a given set of keycodes to a command for the screen.
void keycodesToActions()
{

  if (keyCodeQueue[1] == 0)
  {
    singleKeyCodeHandler(keyCodeQueue[0]);
  }
  else
  {
    multipleKeyCodeHandler();
  }
}

// addToQueue adds a new keyCode to the front of the queue
void addToQueue(int keyCode)
{
  if (queuePosition <= 6)
  {
    keyCodeQueue[queuePosition] = keyCode;
    queuePosition++;
  }
  else
  {
    // Not strictly nessecary but catches index out of bounds errors.
    printString("Queue Full\n");
    resetQueue();
  }
}

// resetQueue resets all values in the keycode queue to 0;
void resetQueue()
{
  for (int i = 0; i < 6; i++)
  {
    keyCodeQueue[i] = 0;
  }
  queuePosition = 0;
  waitingForKeyCode = 0;
}

// setDriverState tells the driver whether to expect more keycodes based on the last keycode sent
// by the keyboard.
void setDriverState(int keyCode)
{
  switch (keyCode)
  {
  case 0xE0:
    waitingForKeyCode = 1;
    break;
  case 0x4B:
    waitingForKeyCode = 0;
    break;
  case 0x4D:
    waitingForKeyCode = 0;
    break;
  case 0xCB:
    waitingForKeyCode = 0;
    break;
  case 0xCD:
    waitingForKeyCode = 0;
    break;
  }
}

// handleKeyboardInput reads the keyboard data port and applys the corresponding action.
void handleKeyboardInput(struct registers r)
{
  int keyCode = port_byte_in(PS2_DATA_PORT);
  if (keyCode != 0)
  {
    addToQueue(keyCode);
  }

  setDriverState(keyCode);

  keycodesToActions(keyCodeQueue);
  if (waitingForKeyCode == 0)
  {
    resetQueue();
  }
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
  // TODO: Figure out why this is failing.
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

  resetQueue();
}
