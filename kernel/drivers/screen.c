#include "screen.h"
#include "../io.h"
#include "../../stdlib/stdlib.h"

// This maps the row and column coordinates to the corresponding memory address in
// video memory.
int cellToMemAddress(int col, int row)
{
  int memoryAddress;
  memoryAddress = (row * MAX_COLS + col) * 2;
  return memoryAddress;
}

// Gets the memory address of the current cursor position.
int getCursor()
{
  // We need to access the VGA control registers
  // register 14 is the high byte of the cursor's memory address space
  // register 15 is the low byte of the cursor's memory address space
  writeByte(REG_SCREEN_CTRL, 14);

  // read the value from the VGA controller (i.e whats currently in displayed by the
  // cursor).
  // We then shift it by 8 i.e address = address * 2^8
  int cursorAddress = readByte(REG_SCREEN_DATA) << 8;
  writeByte(REG_SCREEN_CTRL, 15);

  cursorAddress += readByte(REG_SCREEN_DATA);
  // we multiply by two because only every other byte is actually a character
  // since the second byte in video memory is the cell attribute.
  return cursorAddress * 2;
}

// Sets the new position of the cursor to the provided memory address. Similar to
// getCursor but instead writing to the IO registers
void setCursor(int newCursorAddress)
{
  newCursorAddress /= 2;
  writeByte(REG_SCREEN_CTRL, 15);
  writeByte(REG_SCREEN_DATA, (unsigned char)(newCursorAddress & 0xFF));
  writeByte(REG_SCREEN_CTRL, 14);
  writeByte(REG_SCREEN_DATA, (unsigned char)((newCursorAddress >> 8) & 0xFF));
}

// handleScreenScroll checks if the provided cursor address is on screen and if not
// scrolls the screen down by one line.
int handleScreenScroll(int cursorAddress)
{
  // If the currentAddress about to be accessed is on screen then leave
  // it unmodifed.
  if (cursorAddress < MAX_ROWS * MAX_COLS * 2)
  {
    return cursorAddress;
  }

  // Copy each line to the line above.
  for (int i = 0; i < MAX_ROWS; i++)
  {
    memoryCopy(
        (char *)(cellToMemAddress(0, i) + VIDEO_MEMORY_START),
        (char *)(cellToMemAddress(0, i - 1) + VIDEO_MEMORY_START),
        MAX_COLS * 2);
  }

  // Clear the last line for printing too.
  char *lastLine = (char *)(cellToMemAddress(0, MAX_ROWS - 1) + VIDEO_MEMORY_START);
  for (int i = 0; i < MAX_COLS * 2; i++)
  {
    lastLine[i] = 0;
  }

  // Finally move the cursor back one row
  cursorAddress -= 2 * MAX_COLS;
  return cursorAddress;
}

// moveCursor moves the cursor in the corresponding direction for the given
// argument. 0: left, 1: right, 2: up, 3: down.
void moveCursor(int direction)
{
  int cursorAddress = getCursor();
  switch (direction)
  {
  case 0:
    cursorAddress += 2;
    break;
  case 1:
    cursorAddress -= 2;
    break;
  }
  cursorAddress = handleScreenScroll(cursorAddress);
  setCursor(cursorAddress);
}

// printChar takes a character and prints it at the current location of the cursor.
void printChar(char character, char attribute_byte)
{
  unsigned char *videoMemory = (unsigned char *)VIDEO_MEMORY_START;

  if (!attribute_byte)
  {
    attribute_byte = WHITE_ON_BLACK;
  }

  int cursorAddress = getCursor();

  int isBackspace = 0;

  int currentRow = cursorAddress / (2 * MAX_COLS);

  // handle a backspace
  if (character == 0x0E)
  {
    isBackspace = 1;
    // check we arent about to go off the back of the 
    // screen (or delete the terminal prompt)
    if (cursorAddress > cellToMemAddress(16, currentRow))
    {
      cursorAddress -= 2;
    }
    character = ' ';
  }

  // Move to the start of the next row for a new line character
  if (character == '\n')
  {
    cursorAddress = cellToMemAddress(79, currentRow);
  }
  else
  {
    videoMemory[cursorAddress] = character;
    videoMemory[cursorAddress + 1] = attribute_byte;
  }

  // update the screen address to the next character cell unless its a backspace.
  if (!isBackspace)
  {
    cursorAddress += 2;
  }

  // Check to see if we have reached the bottom of the screen and if so
  // scroll it.
  cursorAddress = handleScreenScroll(cursorAddress);

  // finally update the cursor position.
  setCursor(cursorAddress);
}

// printCharAt prints the given character at the given position on screen.
// Does this by setting the cursor position then printing the character.
void printCharAt(char character, int col, int row, char attribute_byte)
{
  int screenAddress;
  if (col > 0 && row > 0)
  {
    screenAddress = cellToMemAddress(col, row);
    setCursor(screenAddress);
  }
  printChar(character, attribute_byte);
}

// kPrintString prints the provided string at the current cursor position on screen with the given
// VGA colour.
void kPrintStringColour(char *message, char attributeByte) {
  int i = 0;
  while (message[i] != '\0' || message[i] != 0)
  {
    printChar(message[i], attributeByte);
    i++;
  }
}

// kPrintString prints the provided string at the current cursor position on screen.
void kPrintString(char *message)
{
  kPrintStringColour(message, 0);
}

// kPrintStringAt prints the provided string at the specifed position on screen using
// the given VGA colour.
void kPrintStringColourAt(char *message, int col, int row, char attributeByte)
{
  if (col >= 0 && row >= 0)
  {
    setCursor(cellToMemAddress(col, row));
  }

  kPrintStringColour(message, attributeByte);
}

// kPrintStringAt prints the provided string at the specifed position on screen.
void kPrintStringAt(char *message, int col, int row)
{
  kPrintStringColourAt(message, col, row, 0);
}

// kPrintInt prints the given integer by converting it to a string.
void kPrintInt(int integer)
{
  int digitCount = getDigitCount(integer);
  char newStr[digitCount + 1];
  kPrintString(intToString(integer, newStr));
  kPrintString("\n");
}

// clearScreen clears the screen by printing empty spaces at
// all points on the screen.
void clearScreen()
{
  setCursor(cellToMemAddress(0, 0));

  for (int j = 0; j < MAX_COLS; j++)
  {
    for (int i = 0; i < MAX_ROWS; i++)
    {
      printChar(' ', 0);
    }
  }
}
