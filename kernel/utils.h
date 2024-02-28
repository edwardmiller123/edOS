#ifndef UTILS
#define UTILS

// kMalloc allocates memory on the heap and returns a pointer. 
// Memory is allocated in blocks of equal size (24 bytes) in a
// structure where the 1st 4 bytes
// indicate whether the block is in use or not and the 2nd 4 bytes denotes how many blocks 
// are used contigously i.e if the size to be allocated is larger than a single block then two are allocated.
// kMalloc will first iterate through allocated blocks to check if any are free and large enough before
// creating a new block.
void * kmalloc(int size);

// kFree frees the heap memory corresponding to the given pointer.
// It  checks if the block is allocated and if so sets the "taken" 4 bytes to 0 i.e free
void kFree(void * ptr);

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