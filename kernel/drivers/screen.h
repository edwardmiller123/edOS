#ifndef SCREEN
#define SCREEN

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
int cell_to_mem_address(int col, int row);

// Gets the memory address of the current cursor position.
int get_cursor();

// Sets the new position of the cursor to the provided memory address. Similar to
// get_cursor but instead writing to the I/O registers
// TODO: understand whats happing with the bitwise operations.
void set_cursor(int newCursorAddress);

// handleScreenScroll checks if the provided cursor address is on screen and if not
// scrolls the screen down by one line.
int handleScreenScroll(int cursorAddress);

// moveCursor moves the cursor in the corresponding diraction for the given
// argument. 0: left, 1: right, 2: up, 3: down.
// TODO: Implement up and down.
void moveCursor(int direction);

// print_char takes a character and prints it at the current location of the cursor.
void print_char(char character, char attribute_byte);

// print_char_at prints the given character at the given position on screen.
// Does this by setting the cursor position then printing the character.
void print_char_at(char character, int col, int row, char attribute_byte);

// kPrintString prints the provided string at the current cursor position on screen with the given
// VGA colour.
void kPrintStringColour(char *message, char attributeByte);

// kPrintString prints the provided string at the current cursor on screen.
void kPrintString(char *message);

// kPrintStringAt prints the provided string at the specifed position on screen using
// the given VGA colour.
void kPrintStringColourAt(char *message, int col, int row, char attributeByte);

// kPrintStringAt prints the provided string at the specifed position on screen.
void kPrintStringAt(char *message, int col, int row);

// kPrintInt prints the given integer by converting it to a string.
void kPrintInt(int integer);

// clear_screen clears the screen by printing empty spaces at
// all points on the screen.
void clear_screen();

#endif
