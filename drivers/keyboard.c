#include "../kernel/I_O_asm_helpers.h"
#include "screen.h"
#include "keyboard.h"

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
    printInt(testResult);
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

void initPS2Keyboard()
{
  // Test the controller and ports are working.
  testPS2Controller();

  // Disable second ps/2 port.
  port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xA7);

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
}

// addToQueue adds the provided key code to the end of command queue.
void addToQueue(int keyCode, int *front, int *rear, int commandQueue[])
{
  if (*rear < QUEUE_SIZE)
  {
    if (*front == -1)
    {
      *front = 0;
    }
    *rear = *rear + 1;
    commandQueue[*rear] = keyCode;
  }
}

// removeFromQueue removes the front element by decreasing the accesible size of the queue.
void removeFromQueue(int *front)
{
  *front = *front + 1;
}

// pollKeyboard checks the data byte from the keyboard controller and if its not
// empty adds it to the command queue.
void pollKeyboard(int *front, int *rear, int commandQueue[])
{
  int keyCode = port_byte_in(PS2_DATA_PORT);
  if (keyCode != 0)
  {
    addToQueue(keyCode, front, rear, commandQueue);
  }
}

// applyKeyPress prints the corresponding character for a given key code.
void applyKeyPress(int keyCode)
{
  switch (keyCode)
  {
  case 0x12:
    print_char('E', 0);
    break;
  case 0x20:
    print_char('D', 0);
    break;
  case 0x10:
    print_char('Q', 0);
    break;
  case 0x11:
    print_char('W', 0);
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
  default:
    break;
  }
}

// handleKeyPress applys the keyCode at the front of the queue then removes it from
// the queue.
void handleKeyPress(int *front, int *rear, int commandQueue[])
{
  if (*front != -1 && *rear >= *front)
  {
    applyKeyPress(commandQueue[*front]);
    if (commandQueue[*front] != 0)
    {
      removeFromQueue(front);
    }
  }
}

// resetCommandQueue fills the command queue with zeros and sets the start and end
// indices back to -1.
void resetCommandQueue(int *front, int *rear, int commandQueue[])
{
  for (int i = 0; i < QUEUE_SIZE; i++)
  {
    commandQueue[i] = 0;
  }
  *front = -1;
  *rear = -1;
}

// handleKeyboardInput polls the keyboard to create a queue of keyboard commands
// which are then translated to a character to print.
void handleKeyboardInput()
{
  int commandQueue[QUEUE_SIZE] = {0};
  int queueFront = -1;
  int queueRear = -1;

  while (1)
  {
    pollKeyboard(&queueFront, &queueRear, commandQueue);
    handleKeyPress(&queueFront, &queueRear, commandQueue);
    if (queueRear == QUEUE_SIZE)
    {
      resetCommandQueue(&queueFront, &queueRear, commandQueue);
    }
  }
}
