#ifndef SHELL
#define SHELL

#include "../kernel/utils.h"

void echo(char *input);

void help();

void export(char *expression, mapElement enviromentVariables[100]);

void parseAndRunCommand(char *command);

void runShell();

#endif