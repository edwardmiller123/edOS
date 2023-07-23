#include "../kernel/I_O_asm_helpers.h"
// Can print using simple VGA mode in text mode.
// Screen divided into a grid with each cell represented by two bytes in memory
// the first is the character to be displayed the second is the attribute to be applied.
#define VIDEO_MEMORY_START 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f

// Screen device I/O addresses
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

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
  // We need to access the VGA controll registers
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

// print_char takes a character and prints it at the current location of the cursor.
void print_char(char character, char attribute_byte)
{
  unsigned char *videoMemory = (unsigned char *)VIDEO_MEMORY_START;

  if (!attribute_byte)
  {
    attribute_byte = WHITE_ON_BLACK;
  }

  int screenAddress = get_cursor();

  // Move to the start of the next row for a new line character
  if (character == '\n')
  {
    int rows = screenAddress / (2 * MAX_COLS);
    screenAddress = cell_to_mem_address(79, rows);
  }
  else
  {
    videoMemory[screenAddress] = character;
    videoMemory[screenAddress + 1] = attribute_byte;
  }

  // update the screen address to the next character cell.
  screenAddress += 2;

  // finally update the cursor position.
  set_cursor(screenAddress);
}

// print_char_at prints the given character at the given position on screen.
// Does this by setting the cursor position then printing the character.
void print_char_at(char character, int col, int row, char attribute_byte)
{
  int screenAddress;
  if (col > 0 && row > 0)
  {
    screenAddress = cell_to_mem_address(col, row);
    set_cursor(cell_to_mem_address(col, row));
  }
  print_char(character, attribute_byte);
}

// print_string prints the provided string at the current cursor on screen.
void print_string(char *message)
{
  int i = 0;
  while (message[i] != 0)
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

  print_string(message);
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

// intToString converts a base 10 integer to a string.
// TODO: Fix random characters at end of string.
char * intToString(int integer)
{
  char* string;
  int n = integer;
  int i = 0;
  int a = integer;
  int b;

  while (n > 0)
  {
    b = a % 10;
    string[i] = b + 0x30;
    a /= 10;
    
    i++;
    n /= 10;
  }
  return string;
}

