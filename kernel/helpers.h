#ifndef HELPERS
#define HELPERS

// kReverseString returns the reverse of the provided string.
// Uses kMalloc rather than a system call so useful for testing.
char *kReverseString(char *str);

// kIntToString converts a base 10 integer to a string.
char *kIntToString(int integer);

#endif