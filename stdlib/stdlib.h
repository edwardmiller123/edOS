#ifndef STDLIB
#define STDLIB

// syscall triggers a system call to run the appropriate function based on the provided driver
// code (DC) and function code (FC).
// screen: DC = 1
// keyboard: DC = 2
// read: FC = 1
// write: FC = 2
// syscall always returns a void pointer and it is left up to the caller to cast
// the type appropriately.
// The input type is always an int since as can either be just a number or a pointer.
void *syscall(int input, int driverCode, int functionCode);

// malloc makes a call to the kernel to allocate heap memory
void *malloc(int size);

// free frees the memory associated with the given pointer
void free(void * ptr);

// userInput makes a syscall to either read the keyboard (code = 1) input
// or clear the key buffer (code = 2)
char *userInput(int code);

// memoryCopy takes the data (string) stored at the source address and copies
// it to the destination address.
void memoryCopy(char *source, char *destination, int numberOfBytes);

// printString makes a syscall to print the provided string to the screen
void printString(char *strToPrint);

// strLen returns the length of the provided string
int strLen(char *str);

// strMallocAndStore allocates memory and stores the passed in string. Returns a
// pointer to the allocated memory.
char *strMallocAndStore(char *strToStore);

// reverseString returns the reverse of the provided string.
char *reverseString(char *str);

// strCmp compares to strings and returns 0 if they are not the same.
int strCmp(char *string1, char *string2);

// strConcat concatenates two strings
char *strConcat(char *str1, char *str2);

// hash creates a unique hash from a string. This is the
// djb2 algorithm http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash(unsigned char *str);

// intToString converts a base 10 integer to a string.
char *intToString(int integer);

// printInt prints the given integer by converting it to a string.
void printInt(int integer);

// compareIntArrays compares two integer arrays of the same length and returns true if
// all elements in array1 are equal to those of array2. If the lengths are different it returns false.
int compareIntArrays(int array1[], int array2[], int arrayLengths);

// sleep waits until the given time in seconds has passed
void sleep(int seconds);

#endif