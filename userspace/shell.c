#include "../stdlib/stdlib.h"
#include "shell.h"

#define PART_COUNT 10
#define PART_SIZE 10

// echo prints the given argument.
void echo(char *input)
{
  printString(input);
  printString("\n");
}

void help()
{
  printString("edOS doesnt do much right now.\nThe only other command is echo.\nUsage: echo {your favourite word}\n");
}

void parseAndRunCommand(char *command)
{
  char commandParts[PART_COUNT][PART_SIZE];

  // zero the memory to remove any garbage already on the stack
  for (int i = 0; i < PART_COUNT; i++) {
    memoryZero(commandParts[i], PART_SIZE);
  }

  int argIdx = 0;
  int j = 0;
  // Specify whether we are parsing the command itself or the argument
  for (int i = 0; i < strLen(command); i++)
  {
    if (command[i] == '\n' || command[i] == 0)
    {
      continue;
    }

    if (command[i] == '\0' || command[i] == ' ')
    {
      commandParts[argIdx][j] = '\0';
      argIdx++;
      j = 0;
    }
    else
    {
      commandParts[argIdx][j] = command[i];
      j++;
    }
  }

  char *baseCommand = commandParts[0];
  char *firstArg = commandParts[1];

  if (strCmp(baseCommand, "echo"))
  {
    echo(firstArg);
  }
  if (strCmp(baseCommand, "help"))
  {
    help();
  }
  if (strCmp(baseCommand, "test"))
  {
    test();
  }
}

// runShell runs a userspace shell interface that takes user input to run commands.
void runShell()
{
  // set the thread running the shell as in focus
  setFocus();
  char *input = NULL;
  int shellRunning = 1;
  int waitingForCommand = 1;
  printString("\n[ edOS.v0.9 ]:> ");

  while (shellRunning == 1)
  {

    if (waitingForCommand == 1)
    {
      // read the stdin buffer
      input = userInput(1);

      for (int i = 0; i < strLen(input); i++)
      {
        if (input[i] == '\n')
        {
          waitingForCommand = 0;
        }
      }
    }
    else
    {
      parseAndRunCommand(input);
      // clear the stdin buffer
      userInput(2);
      waitingForCommand = 1;
      printString("[ edOS.v0.9 ]:> ");
    }
  }
}

void userThread2() {
  for (int i = 1; i <= 3; i++) {
    printString("UT2: ");
    printInt(i);
    sleep(1000);
  }
}

// for testing stuff
void test()
{
  createThread(&userThread2);
}