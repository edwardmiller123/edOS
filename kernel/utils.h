#ifndef UTILS
#define UTILS

// memoryCopy takes the data stored at the source address and copies 
// it to the destination address.
void memoryCopy(char * source, char * destination, int numberOfBytes);

// intToString converts a base 10 integer to a string.
char * intToString(int integer);

char * reverseString(char * str);

int strLen(char * str);

int compareIntArrays(int array1[], int array2[], int arrayLengths);

// strCmp compares to strings and returns 0 if they are not the same.
int strCmp(char *string1, char *string2);

#endif