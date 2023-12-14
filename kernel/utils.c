#include "utils.h"

// memoryCopy takes the data stored at the source address and copies
// it to the destination address.
void memoryCopy(char *source, char *destination, int numberOfBytes)
{
  for (int i = 0; i < numberOfBytes; i++)
  {
    *(destination + i) = *(source + i);
  }
}

// intToString converts a base 10 integer to a string.
// TODO: really need to make this work correctly
char *intToString(int integer)
{
  char *reverseString;
  char *resultString;
  int n = integer;
  int i = 0;
  int j = 0;
  int a = integer;
  int b;

  while (n > 0)
  {
    b = a % 10;
    reverseString[i] = b + 0x30;
    a /= 10;
    i++;
    n /= 10;
  }

  while (i > 0)
  {
    resultString[j] = reverseString[i - 1];
    i--;
    j++;
  }

  return resultString;
}

// reverseString returns the reverse of the provided string.
// TODO: fix this it doesnt work
char *reverseString(char *str)
{
  int i;
  int j = strLen(str);
  char *newStr;
  while (i < strLen(str))
  {
    newStr[i] = str[j - 1];
    j--;
    i++;
  }
  return newStr;
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
// TODO: Why are last few letters missing
const char *strConcat(char *str1, char *str2)
{
  int newStrLength = strLen(str1) + strLen(str2) + 1;
  char newStr[newStrLength];
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

  char *newStrPtr = newStr;
  return newStrPtr;
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