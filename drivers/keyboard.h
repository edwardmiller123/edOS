#ifndef KEYBOARD
#define KEYBOARD

#include "../kernel/I_O_asm_helpers.h"
#include "screen.h"
#include "../cpu/isr.h"

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

unsigned char keyCodeToAscii(int keyCode, int heldKey);

void keycodesToActions(int keyCodes[6]);

// applyKeyPress prints the corresponding character for a given key code.
void printKeyToScreen(int keyCode);

// addToQueue adds a new keyCode to the front of the queue
void addToQueue(int keyCode);

// resetQueue resets all values in the keycode queue to 0;
void resetQueue();

void handleKeyboardInput(struct registers r);

void initPS2Keyboard();

#endif