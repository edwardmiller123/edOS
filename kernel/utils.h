#ifndef UTILS
#define UTILS

// memoryCopy takes the data stored at the source address and copies 
// it to the destination address.
void memoryCopy(char * source, char * destination, int numberOfBytes) {
  for (int i = 0; i < numberOfBytes; i++) {
    *(destination + i) = *(source + i);
  }
}

// intToString converts a base 10 integer to a string.
char * intToString(int integer)
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

#endif