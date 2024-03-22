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
  return syscall(size, 3, 1);
}

// free frees the memory associated with the given pointer
void free(void *ptr)
{
  syscall(ptr, 3, 2);
}

// userInput makes a syscall to either read the keyboard (code = 1) input
// or clear the key buffer (code = 2)
char *userInput(int code)
{
  // use 0 as an input when none is required
  return (char *)syscall(0, 2, code);
}

// memoryCopy takes the data (string) stored at the source address and copies
// it to the destination address.
void memoryCopy(char *source, char *destination, int numberOfBytes)
{
  for (int i = 0; i < numberOfBytes; i++)
  {
    *(destination + i) = *(source + i);
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
char *reverseString(char *str)
{
  int i = 0;
  int j = strLen(str) - 1;
  char *newStr = (char *)malloc((strLen(str) + 1) * sizeof(char));
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
  char *newStr = (char *)malloc(newStrLength * sizeof(char));
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

// printInt prints the given integer by converting it to a string.
void printInt(int integer)
{
  printString(intToString(integer));
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

// sleep waits until the given time in seconds has passed
void sleep(int seconds)
{
  int time = (int)syscall(seconds, 4, 1);
  int timeEnd = time + (seconds * 1000);
  while (time < timeEnd)
  {
    time = (int)syscall(seconds, 4, 1);
  }
}