#include "screen.h"
#include "../kernel/I_O_asm_helpers.h"
#include "../kernel/utils.h"

// This maps the row and column coordinates to the corresponding memory address in
// video memory.
int cell_to_mem_address(int col, int row)
{
  int memoryAddress;
  memoryAddress = (row * MAX_COLS + col) * 2;
  return memoryAddress;
}

// Gets the memory address of the current cursor position.
int get_cursor()
{
  // We need to access the VGA control registers
  // register 14 is the high byte of the cursor's memory address space
  // register 15 is the low byte of the cursor's memory addres space
  port_byte_out(REG_SCREEN_CTRL, 14);

  // read the value from the VGA controller (i.e whats currently in displayed by the
  // cursor).
  // We then shift it by 8 i.e address = address * 2^8
  int cursorAddress = port_byte_in(REG_SCREEN_DATA) << 8;
  port_byte_out(REG_SCREEN_CTRL, 15);

  cursorAddress += port_byte_in(REG_SCREEN_DATA);
  // we multiply by two because only every other byte is actually a character
  // since the second byte in video memory is the cell attribute.
  return cursorAddress * 2;
}

// Sets the new position of the cursor to the provided memory address. Similar to
// get_cursor but instead writing to the I/O registers
// TODO: understand whats happing with the bitwise operations.
void set_cursor(int newCursorAddress)
{
  newCursorAddress /= 2;
  port_byte_out(REG_SCREEN_CTRL, 15);
  port_byte_out(REG_SCREEN_DATA, (unsigned char)(newCursorAddress & 0xFF));
  port_byte_out(REG_SCREEN_CTRL, 14);
  port_byte_out(REG_SCREEN_DATA, (unsigned char)((newCursorAddress >> 8) & 0xFF));
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
        (char *)(cell_to_mem_address(0, i) + VIDEO_MEMORY_START),
        (char *)(cell_to_mem_address(0, i - 1) + VIDEO_MEMORY_START),
        MAX_COLS * 2);
  }

  // Clear the last line for printing too.
  char *lastLine = (char *)(cell_to_mem_address(0, MAX_ROWS - 1) + VIDEO_MEMORY_START);
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
// TODO: Implement up and down.
void moveCursor(int direction)
{
  int cursorAddress = get_cursor();
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
  set_cursor(cursorAddress);
}

// print_char takes a character and prints it at the current location of the cursor.
void print_char(char character, char attribute_byte)
{
  unsigned char *videoMemory = (unsigned char *)VIDEO_MEMORY_START;

  if (!attribute_byte)
  {
    attribute_byte = WHITE_ON_BLACK;
  }

  int cursorAddress = get_cursor();

  int isBackspace = 0;

  int currentRow = cursorAddress / (2 * MAX_COLS);

  // handle a backspace
  if (character == 0x0E)
  {
    isBackspace = 1;
    // check we arent about to go off the back of the 
    // screen (or delete the terminal prompt)
    if (cursorAddress > cell_to_mem_address(16, currentRow))
    {
      cursorAddress -= 2;
    }
    character = ' ';
  }

  // Move to the start of the next row for a new line character
  if (character == '\n')
  {
    cursorAddress = cell_to_mem_address(79, currentRow);
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

  // Check to see if we have reached the bottom of the srceen and if so
  // scroll it.
  cursorAddress = handleScreenScroll(cursorAddress);

  // finally update the cursor position.
  set_cursor(cursorAddress);
}

// print_char_at prints the given character at the given position on screen.
// Does this by setting the cursor position then printing the character.
void print_char_at(char character, int col, int row, char attribute_byte)
{
  int screenAddress;
  if (col > 0 && row > 0)
  {
    screenAddress = cell_to_mem_address(col, row);
    set_cursor(screenAddress);
  }
  print_char(character, attribute_byte);
}

// printString prints the provided string at the current cursor position on screen.
void printString(char *message)
{
  int i = 0;
  while (message[i] != '\0' || message[i] != 0)
  {
    print_char(message[i], 0);
    i++;
  }
}

// print_string_at prints the provided string at the specifed position on screen.
void print_string_at(char *message, int col, int row)
{
  if (col >= 0 && row >= 0)
  {
    set_cursor(cell_to_mem_address(col, row));
  }

  printString(message);
}

// clear_screen clears the screen by printing empty spaces at
// all points on the screen.
void clear_screen()
{
  set_cursor(cell_to_mem_address(0, 0));

  for (int j = 0; j < MAX_COLS; j++)
  {
    for (int i = 0; i < MAX_ROWS; i++)
    {
      print_char(' ', 0);
    }
  }
}

// printInt prints the given integer by converting it to a string.
void printInt(int integer)
{
  printString(intToString(integer));
}
