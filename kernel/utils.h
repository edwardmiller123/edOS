#ifndef UTILS
#define UTILS

// memoryCopy takes the data stored at the source address and copies 
// it to the destination address.
void memoryCopy(char * source, char * destination, int numberOfBytes) {
  for (int i = 0; i < numberOfBytes; i++) {
    *(destination + i) = *(source + i);
  }
}

#endif