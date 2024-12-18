#ifndef KEYBOARD
#define KEYBOARD

#include "../types.h"
#include "../../stdlib/stdlib.h"

typedef struct KeyboardInput
{
    char *buffer;
    int bufferFront;
    int bufferRear;
} KeyboardInput;

// newKeyboardInput initialises a keyboard input structure. Remember to free it once finished with
KeyboardInput *newKeyboardInput();

// switchActiveKeyQueue updates the currently active key buffer to the given one.
void switchActiveKeyBuffer(KeyboardInput *newInputDst);

// resetKeyBuffer resets all values in the key buffer to 0
void resetKeyBuffer();

// isBufferEmpty checks whether or not the key buffer is empty
bool isBufferEmpty();

// isBufferFull checks if the buffer is full
bool isBufferFull();

// readKeyBuffer returns the key buffer and clears it if
// its full.
char *readKeyBuffer();

void initPS2Keyboard();

#endif