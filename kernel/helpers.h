#ifndef HELPERS
#define HELPERS

// kReverseString returns the reverse of the provided string.
// Uses kMalloc rather than a system call so useful for testing.
char *kReverseString(char *str);

// kIntToString converts a base 10 integer to a string.
char *kIntToString(int integer);

// toggleBit toggles the nth bit of the given number
int toggleBit(int number, int n);

#endif