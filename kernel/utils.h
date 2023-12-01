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

// hash creates a unique hash from a string. This is the 
// djb2 algorithm http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash(unsigned char *str);

struct mapValue {
  char * stringType;
  int integerType;
};

typedef struct {
  unsigned long hash;
  struct mapValue value;
} mapElement;

// getMapValue takes a key and a map and retreives the given value 
// stored there.
char * getMapValue(char * key, mapElement map[100]);

// storeMapValue stores the given value with the key in the provided map
void storeMapValue(char * key, char * value, mapElement map[100]);

#endif