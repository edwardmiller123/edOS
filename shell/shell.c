#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../kernel/utils.h"

struct mapElement enviromentVariables[100];

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

void export(char * expression, struct mapElement enviromentVariables) {
  char commandParts[10][10];
  int argIdx = 0;
  int j = 0;
  // Specify whether we are parsing the command itself or the argument
  for (int i = 0; i < strLen(expression); i++)
  {

    if (expression[i] == '=')
    {
      commandParts[argIdx][j] = '\0';
      argIdx++;
      j = 0;
    }
    else
    {
      commandParts[argIdx][j] = expression[i];
      j++;
    }
  }

  char * variableName = commandParts[0];
  char * valueToStore = commandParts[1];

} 

void parseAndRunCommand(char *command)
{
  char commandParts[10][10];

  int argIdx = 0;
  int j = 0;
  // Specify whether we are parsing the command itself or the argument
  for (int i = 0; i < BUFFER_SIZE; i++)
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
}

// runShell runs a mock shell in the kernel. Hopefully one day it will run
// as an application in user mode.
void runShell()
{
  char *stdInBuffer;
  int shellRunning = 1;
  int waitingForCommand = 1;
  printString("\n[ edOS.v0.9 ]:> ");
  while (shellRunning == 1)
  {

    if (waitingForCommand == 1)
    {
      stdInBuffer = readKeyBuffer();

      for (int i = 0; i < BUFFER_SIZE; i++)
      {
        if (stdInBuffer[i] == '\n')
        {
          waitingForCommand = 0;
        }
      }
    }
    else
    {
      parseAndRunCommand(stdInBuffer);
      resetKeyBuffer();
      waitingForCommand = 1;
      printString("[ edOS.v0.9 ]:> ");
    }
  }
}