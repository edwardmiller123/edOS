#ifndef KEYBOARD
#define KEYBOARD

#include "../kernel/I_O_asm_helpers.h"
#include "screen.h"

// To talk to the keyboard encoder directly
// we read data and write command codes both to 0x60.
#define PS2_DATA_PORT 0x60
#define PS2_STATUS_AND_COMMAND_REGISTER 0x64
#define QUEUE_SIZE 10

void testController()
{
  // test ps2 controller
  int testResult = sendCommand(0xAA, PS2_DATA_PORT, PS2_STATUS_AND_COMMAND_REGISTER);
  switch (testResult)
  {
  case 0x55:
    print_string("Controller Operational\n");
    break;
  case 0xFC:
    print_string("Controller failed with fault\n");
    break;
  default:
    print_string("Controller failed with unexpected result\n");
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
    print_string("Port 1 Operational\n");
    break;
  default:
    print_string("Port 1 failed with unexpected result\n");
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
    print_string("Port 2 Operational\n");
    break;
  default:
    print_string("Port 2 failed with unexpected result\n");
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

void initPS2Keyboard()
{
  // Test the controller and ports are working.
  testPS2Controller();

  // Disable second ps/2 port.
  port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xA7);

  // Enable first ps/2 port.
  // port_byte_out(PS2_STATUS_AND_COMMAND_REGISTER, 0xAE);

  // Disable keyboard scanning.
  int disableScanningResponse = sendCommand(0xF5, PS2_DATA_PORT, PS2_DATA_PORT);
  switch (disableScanningResponse)
  {
  case 0xFA:
    print_string("Keyboard scanning disabled\n");
    break;
  case 0:
    print_string("Keyboard not found (empty response)\n");
    break;
  default:
    print_string("Error disabling scan codes\n");
  }

  // Reset keyboard
  int resetResponse = sendCommand(0xFF, PS2_DATA_PORT, PS2_DATA_PORT);
  switch (resetResponse)
  {
  case 0xFA:
    print_string("Keyboard reset\n");
    break;
  case 0:
    print_string("Keyboard not found (empty response)\n");
    break;
  default:
    print_string("Error resetting keyboard\n");
  }

  // Select scan code set 2. Command byte sets scan code set and the data byte chooses the set.
  int selectScanCodeResponse = sendCommandWithData(0xF0, 2, PS2_DATA_PORT, PS2_DATA_PORT);
  switch (resetResponse)
  {
  case 0xFA:
    print_string("Scan code set\n");
    break;
  case 0:
    print_string("Keyboard not found (empty response)\n");
    break;
  default:
    print_string("Error setting scan code\n");
  }

  // Set the keyboard to send scan codes.
  int enableScanningResponse = sendCommand(0xF4, PS2_DATA_PORT, PS2_DATA_PORT);
  switch (enableScanningResponse)
  {
  case 0xFA:
    print_string("Scanning enabled\n");
    break;
  case 0:
    print_string("Keyboard not found (empty response)\n");
    break;
  default:
    print_string("Error enabling scanning\n");
  }
}

void addToQueue(int keyCommand, int front, int rear, int commandQueue[])
{
  if (rear < QUEUE_SIZE)
  {
    print_char('A', 0);
    if (front == -1)
    {
      front = 0;
    }
    rear++;
    commandQueue[rear] = keyCommand;
  }
}

void removeFromQueue(int front)
{
  front++;
}

void pollKeyboard(int front, int rear, int commandQueue[])
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
  }
}

// handleKeyPress applys the keyCode at the front of the queue then removes it from
// the queue.
void handleKeyPress(int front, int commandQueue[])
{
  applyKeyPress(commandQueue[front]);
  removeFromQueue(front);
}

// handleKeyboardInput polls the keyboard to create a queue of keyboard commands
// which are then translated to a character to print. 
void handleKeyboardInput()
{
  int commandQueue[QUEUE_SIZE];
  int queueFront = -1;
  int queueRear = -1;
  

  while (1)
  {
    pollKeyboard(queueFront, queueRear, commandQueue);
    handleKeyPress(queueFront, commandQueue);
  }
}

#endif