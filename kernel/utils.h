#ifndef UTILS
#define UTILS

// memoryCopy takes the data stored at the source address and copies 
// it to the destination address.
void memoryCopy(char * source, char * destination, int numberOfBytes);

// intToString converts a base 10 integer to a string.
char * intToString(int integer);

#endif