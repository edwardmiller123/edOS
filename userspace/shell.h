#ifndef SHELL
#define SHELL

void echo(char *input);

void help();

void parseAndRunCommand(char *command);

void runShell();

// proof that we are infact in user mode
void test();

#endif