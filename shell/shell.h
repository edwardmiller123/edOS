#ifndef SHELL
#define SHELL

#include "../kernel/utils.h"

// makeSyscall triggers a system call to run the appropriate function based of the provided
// codes. i.e driver code 1 = screen, function code 2 = write
char * makeSyscall(char * input, int driverCode, int functionCode);

// readInput makes a syscall to read the keyBuffer;
char * readInput();

// shellPrint makes a syscall to print the provided string to the screen
void shellPrintStr(char * strToPrint);

// getMapValue takes a key and a map and retreives the given value
// stored there.
char *getEnvValue(char *key);

// initMap sets all the environment variables to empty strings.
void initEnvMap();

// storeMapValue stores the given value with the key in the provided map
void storeEnvValue(char *key, char *value);

void echo(char *input);

void help();

void export(char *expression);

char * replaceEnvVariables(char * arg);

void parseAndRunCommand(char *command);

void runShell();

#endif