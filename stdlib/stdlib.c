#include "stdlib.h"

// Maximum number of digits a 32 bit base 10 integer can have
#define MAX_DIGITS 10

// syscall triggers a system call to run the appropriate function based on the provided driver
// code (DC) and function code (FC).
// screen: DC = 1
// keyboard: DC = 2
// read: FC = 1
// write: FC = 2
// syscall always returns a void pointer and it is left up to the caller to cast
// the type appropriately.
// The input type is always an int since as can either be just a number or a pointer.
void *syscall(int input, int driverCode, int functionCode)
{
  // trigger the interrupt to make the call then read the response. If the handler
  // has no response then a 0 will be returned.
  __asm__ volatile("int $47" : : "a"(input), "b"(driverCode), "c"(functionCode));

  void *output = 0;
  if (functionCode == 1)
  {
    __asm__ volatile("movl %%eax, %0" : "=r"(output) :);
  }

  return output;
}

// malloc makes a call to the kernel to allocate heap memory
void *malloc(int size)
{
  // right now this is just a system call to kMalloc but if we ever implement paging
  // then this will allocate memory for the current process and make the system call
  // to request more from the kernel if needed.
  return syscall(size, 3, 1);
}

// free frees the memory associated with the given pointer
void free(void *ptr)
{
  syscall(ptr, 3, 2);
}

// userInput makes a syscall to either read the keyboard stdin buffer (code = 1
// or clear it (code = 2)
char *userInput(int code)
{
  // use 0 as an input when none is required
  return (char *)syscall(0, 2, code);
}

// createThread runs the given function in a new thread
void createThread(void *threadFunction)
{
  syscall(threadFunction, 5, 2);
}

// setFocus gives the currently executing thread ownership of the stdin buffer
void setFocus()
{
  syscall(0, 5, 1);
}

// memoryCopy takes the data stored at the source address and copies
// it to the destination address.
void memoryCopy(char *source, char *destination, int numberOfBytes)
{
  for (int i = 0; i < numberOfBytes; i++)
  {
    *(destination + i) = *(source + i);
  }
}

// memoryZero sets the given number bytes at the given address to zero
void *memoryZero(char *dst, int numBytes)
{
  if (dst == NULL)
  {
    return NULL;
  }
  for (int i = 0; i < numBytes; i++)
  {
    *(dst + i) = 0;
  }
}

// printString makes a syscall to print the provided string to the screen
void printString(char *strToPrint)
{
  syscall(strToPrint, 1, 2);
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

// strMallocAndStore allocates memory and stores the passed in string. Returns a
// pointer to the allocated memory.
char *strMallocAndStore(char *strToStore)
{
  char *allocated = (char *)malloc((strLen(strToStore) + 1) * sizeof(char));
  if (allocated != 0)
  {
    // copy one extra byte to get the strings terminating character.
    memoryCopy(strToStore, allocated, strLen(strToStore) + 1);
  }
  return allocated;
}

// reverseString returns the reverse of the provided string.
char *reverseString(char *str, char *newStr)
{
  int i = 0;
  int j = strLen(str) - 1;
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

// strCmp compares two strings and returns 0 if they are not the same.
bool strCmp(char *string1, char *string2)
{
  // check for the case of NULL strings
  if (string1 == NULL)
  {
    if (string2 == NULL)
    {
      return true;
    }
    return false;
  }

  if (string2 == NULL)
  {
    if (string1 == NULL)
    {
      return true;
    }
    return false;
  }

  if (sizeof(string1) != sizeof(string2))
  {
    return false;
  }

  int i = 0;
  while (string1[i] != '\0')
  {
    if (string1[i] != string2[i])
    {
      return false;
    }
    i++;
  }

  // check if a string1 is just an exit character
  if (string1[0] == '\0' && string2[0] != '\0')
  {
    return false;
  }
  return true;
}

// strConcat concatenates two strings in the given buffer.
// The allocated buffer must be large enough for both strings.
char *strConcat(char *str1, char *str2, char *newStr)
{
  int j = 0;
  int newStrLength = strLen(str1) + strLen(str2) + 1;
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

// strConcatAppend concatenates two strings by appending the second string to the
// end of the first string. The first string must be a buffer with enough
// extra space to append the second. Useful if you need to concatenate alot of
// strings in a loop
char *strConcatAppend(char *str1, char *str2)
{
  int newStrLength = strLen(str1) + strLen(str2) + 1;
  int j = strLen(str1);
  for (int i = 0; i < strLen(str2); i++)
  {
    str1[j] = str2[i];
    j++;
  }
  str1[newStrLength] = '\0';
  return str1;
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

// getDigitCount returns the number of digits in the given base 10 integer
int getDigitCount(int num)
{
  int multiple = 10;
  int count = 1;
  while (num > multiple)
  {
    count++;
    multiple *= 10;
  }
  return count;
}

// intToString converts a base 10 integer to a string.
char *intToString(int integer, char *newStr)
{
  int digitCount = getDigitCount(integer);
  // check the number doesnt exceed the digit count. This shouldnt be possible
  // on a 32 bit system but checking just to be safe.
  if (digitCount > MAX_DIGITS)
  {
    return NULL;
  }
  char reverseStr[MAX_DIGITS];
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

  char *resultString = reverseString(reverseStr, newStr);

  return resultString;
}

// printInt prints the given integer by converting it to a string.
void printInt(int integer)
{

  int digitCount = getDigitCount(integer);
  char newStr[digitCount + 1];
  printString(intToString(integer, newStr));
  printString("\n");
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

// sleep waits until the given time in milliseconds has passed
void sleep(int ms)
{
  int time = (int)syscall(ms, 4, 1);
  int timeEnd = time + ms;
  while (time < timeEnd)
  {
    time = (int)syscall(ms, 4, 1);
  }
}