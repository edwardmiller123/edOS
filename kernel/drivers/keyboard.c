#include "../../stdlib/stdlib.h"
#include "../mem.h"
#include "../io.h"
#include "../log.h"
#include "screen.h"
#include "keyboard.h"

// To talk to the keyboard encoder directly
// we read data and write command codes both to 0x60.
#define PS2_DATA_PORT 0x60
#define PS2_STATUS_AND_COMMAND_REGISTER 0x64
#define QUEUE_SIZE 6
#define KEY_BUFFER_SIZE 256

static int heldKey, keyCodeQueuePosition;
static bool waitingForKeyCode;

// keyCodeQueue holds the list of key codes from the keyboard waiting to be interpreted
// by the driver.
static int keyCodeQueue[QUEUE_SIZE];

// keyBuffer holds the characters printed to the screen to be read from by
// the shell (or any other caller). Define as a ptr since we switch it to
// the buffer for the currently in focus thread.
static char *keyBuffer = NULL;
static int keyBufferRear = 0;
static int keyBufferFront = 0;

// newKeyboardInput initialises a keyboard input structure. Remember to free it once finished with
KeyboardInput *newKeyboardInput()
{
  KeyboardInput *input = (KeyboardInput *)kMalloc(sizeof(KeyboardInput));
  char *buffer = (char *)kMalloc(sizeof(char) * KEY_BUFFER_SIZE);
  input->buffer = buffer;
  input->bufferFront = 0;
  input->bufferRear = 0;
  return input;
}

// switchActiveKeyQueue updates the currently active key buffer to the given one.
void switchActiveKeyBuffer(KeyboardInput *newInputDst)
{
  keyBuffer = newInputDst->buffer;
  keyBufferRear = newInputDst->bufferRear;
  keyBufferFront = newInputDst->bufferFront;
}

void testController()
{
  // test ps2 controller
  int testResult = sendCommand(0xAA, PS2_DATA_PORT, PS2_STATUS_AND_COMMAND_REGISTER);
  int args[] = {testResult};
  switch (testResult)
  {
  case 0x55:
    kLogDebug("PS/2 controller test passed\n");
    break;
  case 0xFC:
    kLogf(ERROR, "PS/2 controller failed with fault: $", args, 1);
    break;
  default:
    kLogf(ERROR, "PS/2 controller failed with unexpected result: $", args, 1);

    break;
  }
}

void testPort1()
{
  // test ps2 port 1
  writeByte(PS2_STATUS_AND_COMMAND_REGISTER, 0xAB);
  int testResult = readByte(PS2_DATA_PORT);
  int args[] = {testResult};
  switch (testResult)
  {
  case 0x00:
    kLogDebug("PS/2 port 1 test passed");
    break;
  default:
    kLogf(ERROR, "PS/2 port 1 failed with unexpected result", args, 1);
    break;
  }
}

void testPort2()
{
  // test ps2 port 1
  writeByte(PS2_STATUS_AND_COMMAND_REGISTER, 0xA9);
  int testResult = readByte(PS2_DATA_PORT);
  int args[] = {testResult};
  switch (testResult)
  {
  case 0x00:
    kLogDebug("PS/2 port 2 Operational");
    break;
  default:
    kLogf(ERROR, "PS/2, port 2 failed with unexpected result", args, 1);
    break;
  }
}

// testPS2Controller runs a simple test to see if the ps2 controller is working.
void testPS2Controller()
{
  // test ps2 controller and ports.
  kLogDebug("Running PS/2 Controller tests...");

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
      return (unsigned char)'£';
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
    case 0x0D:
      return '+';
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
    case 0x0D:
      return '=';
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

// removeFromBufferRear removes the last character in the buffer by reseting
// it to zero
void removeFromBufferRear()
{
  if (isBufferEmpty())
  {
    keyBuffer[keyBufferRear] = 0;
    keyBufferRear--;
  }
}

// addToBuffer adds a character to the key buffer
void addToBuffer(char character)
{
  if ((void *)keyBuffer == NULL)
  {
    kLogWarning("skipped adding character to NULL keyBuffer");
    return;
  }

  if (keyBufferRear <= KEY_BUFFER_SIZE)
  {
    if (keyBufferFront <= keyBufferRear)
    {
      keyBuffer[keyBufferRear] = character;
      keyBufferRear++;
    }
    else
    {
      kLogError("Key buffer overrun. Resetting");
      resetKeyBuffer();
    }
  }
  else
  {
    kLogWarning("skipped adding character as keyBuffer is full");
  }
}

// singleKeyCodeHandler converts single keycode keyboard commands into the correspinding action.
char singleKeyCodeHandler(int keyCode)
{
  int character = 0;
  character = keyCodeToAscii(keyCode, heldKey);
  switch (keyCode)
  {
  case 0x0E:
    // backspace
    character = 0x0E;
    removeFromBufferRear();
    printChar(character, 0);
    character = 0;
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

  return character;
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
char keycodesToActions()
{
  char character = 0;
  if (keyCodeQueue[1] == 0)
  {
    character = singleKeyCodeHandler(keyCodeQueue[0]);
  }
  else
  {
    multipleKeyCodeHandler();
  }
  return character;
}

// addToQueue adds a new keyCode to the front of the queue
void addToQueue(int keyCode)
{
  if (keyCodeQueuePosition <= 6)
  {
    keyCodeQueue[keyCodeQueuePosition] = keyCode;
    keyCodeQueuePosition++;
  }
  else
  {
    // Not strictly nessecary but catches index out of bounds errors.
    kLogWarning("Key code queue is full. Resetting");
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
  keyCodeQueuePosition = 0;
  waitingForKeyCode = false;
}

// setDriverState tells the driver whether to expect more keycodes based on the last keycode sent
// by the keyboard.
void setDriverState(int keyCode)
{
  switch (keyCode)
  {
  case 0xE0:
    // initial keycode for multi code command
    waitingForKeyCode = true;
    break;
  case 0x4B:
    // final code for cursor left pressed
  case 0x4D:
    // final code for cursor right pressed
  case 0xCB:
    // final code for cursor left released
  case 0xCD:
    // final code for cursor right released
  case 0x50:
    // final code for cursor down pressed
  case 0x48:
    // final code for cursor up pressed
  case 0xD0:
    // final code for cursor down released
  case 0xC8:
    // final code for cursor down released
    waitingForKeyCode = false;
    break;
  }
}

// isBufferEmpty checks whether or not the key buffer is empty
bool isBufferEmpty()
{
  if (keyBufferFront == keyBufferRear)
  {
    return true;
  }
  return false;
}

// isBufferFull checks if the buffer is full
bool isBufferFull()
{
  if (keyBufferRear == KEY_BUFFER_SIZE)
  {
    return true;
  }
  return false;
}

// resetKeyBuffer resets all values in the key buffer to 0;
void resetKeyBuffer()
{
  if ((void *)keyBuffer == NULL)
  {
    kLogWarning("skipping reset of NULL keybuffer");
    return;
  }
  memoryZero(keyBuffer, KEY_BUFFER_SIZE);
  keyBufferFront = 0;
  keyBufferRear = 0;
}

// readKeyBuffer returns the key buffer and clears it if
// its full.
char *readKeyBuffer()
{
  if ((void *)keyBuffer == NULL)
  {
    kLogWarning("Cannot read from NULL keybuffer");
    return NULL;
  }

  char *readBuffer = keyBuffer;
  if (isBufferFull())
  {
    resetKeyBuffer();
  }
  return readBuffer;
}

// handleKeyboardInput reads the keyboard data port and applys the corresponding action.
int handleKeyboardInput(struct registers r)
{
  int keyCode = readByte(PS2_DATA_PORT);
  if (keyCode != 0)
  {
    addToQueue(keyCode);
  }

  setDriverState(keyCode);

  int character = keycodesToActions(keyCodeQueue);
  if (!waitingForKeyCode)
  {
    resetQueue();
  }

  if (character != 0)
  {
    addToBuffer(character);
    // TODO: currently the keyboard driver is directly printing to the screen but really
    // it should just add to the "stdin" buffer which is then read from (and printed) by
    // the current running thread.
    printChar(character, 0);
  }
  return 0;
}

void initPS2Keyboard()
{
  // Disable first ps/2 port.
  writeByte(PS2_STATUS_AND_COMMAND_REGISTER, 0xAD);

  // Disable second ps/2 port.
  writeByte(PS2_STATUS_AND_COMMAND_REGISTER, 0xA7);

  // flush output buffer
  readByte(PS2_DATA_PORT);

  // Test the controller and ports are working.
  testPS2Controller();

  // Enable first ps/2 port.
  writeByte(PS2_STATUS_AND_COMMAND_REGISTER, 0xAE);

  // Reset keyboard
  int resetResponse = sendCommand(0xFF, PS2_DATA_PORT, PS2_DATA_PORT);
  int args[] = {resetResponse};
  switch (resetResponse)
  {
  case 0xFA:
    kLogDebug("Keyboard ready");
    break;
  case 0:
    kLogWarning("Keyboard not found (empty response)");
    break;
  default:
    kLogf(ERROR, "Error resetting keyboard", args, 1);
  }

  resetQueue();
  resetKeyBuffer();
  registerInterruptHandler(33, handleKeyboardInput);
  kLogInfo("PS/2 controller initialised");
}
