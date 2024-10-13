#ifndef KEYBOARD
#define KEYBOARD

#include "../types.h"

// To talk to the keyboard encoder directly
// we read data and write command codes both to 0x60.
#define PS2_DATA_PORT 0x60
#define PS2_STATUS_AND_COMMAND_REGISTER 0x64
#define QUEUE_SIZE 6
#define KEY_BUFFER_SIZE 256

typedef struct KeyboardInput
{
    char *buffer;
    int bufferFront;
    int bufferRear;
} KeyboardInput;

// newKeyboardInput initialises a keyboard input structure. Remember to free it once finished with
KeyboardInput *newKeyboardInput();

// switchActiveKeyQueue updates the currently active key buffer to the given one.
void switchActiveKeyQueue(KeyboardInput *newInputDst);

void testController();

void testPort1();

void testPort2();

// testPS2Controller runs a simple test to see if the ps2 controller is working.
void testPS2Controller(int debug);

unsigned char keyCodeToAscii(int keyCode, int heldKey);

char singleKeyCodeHandler(int keyCode);

int *trimQueue(int keyCodes[6]);

void multipleKeyCodeHandler();

char keycodesToActions();

// addToQueue adds a new keyCode to the front of the queue
void addToQueue(int keyCode);

// resetQueue resets all values in the keycode queue to 0;
void resetQueue();

// setDriverState tells the driver whether to expect more keycodes based on the last keycode sent
// by the keyboard.
void setDriverState(int keyCode);

// resetKeyBuffer resets all values in the key buffer to 0
void resetKeyBuffer();

// addToBuffer adds a character to the key buffer
void addToBuffer(char character);

// isBufferEmpty checks whether or not the key buffer is empty
int isBufferEmpty();

// isBufferFull checks if the buffer is full
int isBufferFull();

// removeFromBufferRear removes the last character from
// the buffer
void removeFromBufferRear();

// readKeyBuffer returns the key buffer and clears it if
// its full.
char *readKeyBuffer();

// handleKeyboardInput reads the keyboard data port and applys the corresponding action.
int handleKeyboardInput(struct registers r);

void initPS2Keyboard(int debug);

#endif