#include "../stdlib/stdlib.h"
#include "shell.h"

// echo prints the given argument. The first "program".
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
  char commandParts[10][10];

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

  if (strCmp(baseCommand, "echo") == 1)
  {
    echo(firstArg);
  }
  if (strCmp(baseCommand, "help") == 1)
  {
    help();
  }
  if (strCmp(baseCommand, "test") == 1)
  {
    test();
  }
}

// runShell runs a mock shell in the kernel. Hopefully one day it will run
// as an application in user mode.
void runShell()
{
  char *input;
  int shellRunning = 1;
  int waitingForCommand = 1;
  printString("\n[ edOS.v0.9 ]:> ");

  while (shellRunning == 1)
  {

    if (waitingForCommand == 1)
    {
      // read the key buffer
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
      // clear the key buffer
      userInput(2);
      waitingForCommand = 1;
      printString("[ edOS.v0.9 ]:> ");
    }
  }
}

// for testing stuff
void test()
{
  sleep(5);
}