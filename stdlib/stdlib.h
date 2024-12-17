#ifndef STDLIB
#define STDLIB

#define NULL (void*) 0

typedef enum { false, true } bool;

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

// userInput makes a syscall to either read the keyboard stdin buffer (code = 1
// or clear it (code = 2)
char *userInput(int code);

// createThread runs the given function in a new thread
void createThread(void *threadFunction);

// setFocus gives the currently executing thread ownership of the stdin buffer
void setFocus();

// memoryCopy takes the data stored at the source address and copies
// it to the destination address.
void memoryCopy(char *source, char *destination, int numberOfBytes);

// memoryZero sets the given number bytes at the given address to zero
void * memoryZero(char * dst, int numBytes);

// printString makes a syscall to print the provided string to the screen
void printString(char *strToPrint);

// strLen returns the length of the provided string
int strLen(char *str);

// strMallocAndStore allocates memory and stores the passed in string. Returns a
// pointer to the allocated memory.
char *strMallocAndStore(char *strToStore);

// reverseString returns the reverse of the provided string.
char *reverseString(char *str, char* newStr);

// strCmp compares to strings and returns 0 if they are not the same.
int strCmp(char *string1, char *string2);

// strConcat concatenates two strings
char *strConcat(char *str1, char *str2, char dstArr[]);

// strConcatAppend concatenates two strings by appending the second string to the
// end of the firts string. The first string must be a buffer with enough
// extra space to append the second. Useful if you need to concatenate alot of 
// strings in a loop
char * strConcatAppend(char *str1, char *str2);

// hash creates a unique hash from a string. This is the
// djb2 algorithm http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash(unsigned char *str);

// intToString converts a base 10 integer to a string.
char *intToString(int integer, char * newStr);

// getDigitCount returns the number of digits in the given base 10 integer
int getDigitCount(int num);

// printInt prints the given integer by converting it to a string.
void printInt(int integer);

// compareIntArrays compares two integer arrays of the same length and returns true if
// all elements in array1 are equal to those of array2. If the lengths are different it returns false.
int compareIntArrays(int array1[], int array2[], int arrayLengths);

// sleep waits until the given time in milliseconds has passed
void sleep(int ms);

#endif