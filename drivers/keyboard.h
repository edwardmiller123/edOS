#ifndef KEYBOARD
#define KEYBOARD

#include "../kernel/I_O_asm_helpers.h"
#include "screen.h"

// To talk to the keyboard encoder directly
// we read data and write command codes both to 0x60.
#define PS2_DATA_PORT 0x60
#define PS2_STATUS_AND_COMMAND_REGISTER 0x64
#define QUEUE_SIZE 100

void testController();

void testPort1();

void testPort2();

// testPS2Controller runs a simple test to see if the ps2 controller is working.
void testPS2Controller();

void initPS2Keyboard();

// addToQueue adds the provided key code to the end of command queue.
void addToQueue(int keyCode, int *front, int *rear, int commandQueue[]);

// removeFromQueue removes the front element by decreasing the accesible size of the queue.
void removeFromQueue(int *front);

// pollKeyboard checks the data byte from the keyboard controller and if its not
// empty adds it to the command queue.
void pollKeyboard(int *front, int *rear, int commandQueue[]);

// applyKeyPress prints the corresponding character for a given key code.
void applyKeyPress(int keyCode);

// handleKeyPress applys the keyCode at the front of the queue then removes it from
// the queue.
void handleKeyPress(int *front, int *rear, int commandQueue[]);

// resetCommandQueue fills the command queue with zeros and sets the start and end
// indices back to -1.
void resetCommandQueue(int *front, int *rear, int commandQueue[]);

// handleKeyboardInput polls the keyboard to create a queue of keyboard commands
// which are then translated to a character to print.
void handleKeyboardInput();

#endif