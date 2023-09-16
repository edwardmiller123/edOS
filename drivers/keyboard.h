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

void singleKeyCodeActions(int keyCode);

void multipleKeyCodeActions();

void keycodesToActions();

// addToQueue adds a new keyCode to the front of the queue
void addToQueue(int keyCode);

// resetQueue resets all values in the keycode queue to 0;
void resetQueue();

// setDriverState tells the driver whether to expect more keycodes based on the last keycode sent
// by the keyboard.
void setDriverState(int keyCode);

void handleKeyboardInput(struct registers r);

void initPS2Keyboard();

#endif