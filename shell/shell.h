#ifndef SHELL
#define SHELL

#include "../kernel/utils.h"

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