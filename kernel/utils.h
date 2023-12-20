#ifndef UTILS
#define UTILS

// memoryCopy takes the data stored at the source address and copies
// it to the destination address.
void memoryCopy(char *source, char *destination, int numberOfBytes);

// intToString converts a base 10 integer to a string.
char *intToString(int integer);

char *reverseString(char *str);

int strLen(char *str);

// strAllocAndStore allocates memory for a string and stores the passed in string
// there.
char *strAllocAndStore(char *strToStore);

int compareIntArrays(int array1[], int array2[], int arrayLengths);

// strCmp compares to strings and returns 0 if they are not the same.
int strCmp(char *string1, char *string2);

char *strConcat(char *str1, char *str2);

// hash creates a unique hash from a string. This is the
// djb2 algorithm http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash(unsigned char *str);

#endif