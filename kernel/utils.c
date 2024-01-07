#include "utils.h"
#include "../drivers/screen.h"

// Pre allocate memory for strings. This is quite wasteful but works for now.
char freeMemory[400][256];
int memIncremetor = 0;

// memoryCopy takes the data stored at the source address and copies
// it to the destination address.
void memoryCopy(char *source, char *destination, int numberOfBytes)
{
  for (int i = 0; i < numberOfBytes; i++)
  {
    *(destination + i) = *(source + i);
  }
}

// strAlloc allocates memory for strings. This is a very janky replacement
// for malloc for the time being.
char *strAlloc()
{
  if (memIncremetor < 400)
  {
    char *allocated = &freeMemory[memIncremetor];
    memIncremetor++;
    return allocated;
  }
  else
  {
    kPrintString("\nOut off free memory\n");
    return 0;
  }
}

// strLen returns the length of the provided string
int strLen(char *str)
{
  int length = 0;
  char stopCharacter = str[length];
  while (stopCharacter != '\0')
  {
    length++;
    stopCharacter = str[length];
  }
  return length;
}

// strAllocAndStore allocates memory and stores the passed in string. Returns a
// pointer to the allocated memory.
char *strAllocAndStore(char *strToStore)
{
  char *allocated = strAlloc();
  if (allocated != 0)
  {
    // copy one extra byte to get the strings terminating character.
    memoryCopy(strToStore, allocated, strLen(strToStore) + 1);
  }
  return allocated;
}

// reverseString returns the reverse of the provided string.
char *reverseString(char *str)
{
  int i = 0;
  int j = strLen(str) - 1;
  char *newStr = strAlloc();
  while (i < strLen(str))
  {
    newStr[i] = str[j];
    j--;
    i++;
  }
  // Add back the terminating character
  newStr[strLen(str)] = '\0';
  return newStr;
}

// intToString converts a base 10 integer to a string.
char *intToString(int integer)
{
  char reverseStr[50];
  int n = integer;
  int i = 0;
  int j = 0;
  int a = integer;
  int b;

  while (n > 0)
  {
    b = a % 10;
    reverseStr[i] = b + 0x30;
    a /= 10;
    n /= 10;
    i++;
  }
  reverseStr[i] = '\0';

  char *resultString = reverseString(reverseStr);

  return resultString;
}

// compareIntArrays compares two integer arrays of the same length and returns true if
// all elements in array1 are equal to those of array2. If the lengths are different it returns false.
int compareIntArrays(int array1[], int array2[], int arrayLengths)
{
  // First check that the arrays are actually the same length
  if (sizeof(array1) != sizeof(array2))
  {
    return 0;
  }

  for (int i = 0; i < arrayLengths; i++)
  {
    if (array1[i] != array2[i])
    {
      return 0;
    }
  }
  return 1;
}

// strCmp compares to strings and returns 0 if they are not the same.
int strCmp(char *string1, char *string2)
{
  if (sizeof(string1) != sizeof(string2))
  {
    return 0;
  }

  int i = 0;
  while (string1[i] != '\0')
  {
    if (string1[i] != string2[i])
    {
      return 0;
    }
    i++;
  }
  return 1;
}

// strConcat concatenates two strings
char *strConcat(char *str1, char *str2)
{
  int newStrLength = strLen(str1) + strLen(str2) + 1;
  char *newStr = strAlloc();
  int j = 0;
  for (int i = 0; i < strLen(str1); i++)
  {
    newStr[j] = str1[i];
    j++;
  }
  for (int i = 0; i < strLen(str2); i++)
  {
    newStr[j] = str2[i];
    j++;
  }
  newStr[newStrLength - 1] = '\0';

  return newStr;
}

// hash creates a unique hash from a string. This is the
// djb2 algorithm http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash(unsigned char *str)
{
  unsigned long hash = 5381;
  int c;

  while (c = *str++)
    hash = ((hash << 5) + hash) + c;

  return hash;
}