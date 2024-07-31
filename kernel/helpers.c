#include "mem.h"

// kReverseString returns the reverse of the provided string.
char *kReverseString(char *str)
{
  int i = 0;
  int j = strLen(str) - 1;
  char *newStr = (char *)kMalloc((strLen(str) + 1) * sizeof(char));
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

// kIntToString converts a base 10 integer to a string.
char *kIntToString(int integer)
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

  char *resultString = kReverseString(reverseStr);

  return resultString;
}

// toggleBit toggles the nth bit of the given number
int toggleBit(int number, int n)
{
  return number ^ (1 << n);
}