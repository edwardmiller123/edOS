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
char * reverseString(char *str)
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