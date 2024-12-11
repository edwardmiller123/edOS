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

// printChar takes a character and prints it at the current location of the cursor.
void printChar(char character, char attribute_byte);

// printCharAt prints the given character at the given position on screen.
// Does this by setting the cursor position then printing the character.
void printCharAt(char character, int col, int row, char attribute_byte);

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

// clearScreen clears the screen by printing empty spaces at
// all points on the screen.
void clearScreen();

#endif
