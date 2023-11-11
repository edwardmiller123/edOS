#include "../drivers/screen.h"
#include "../drivers/keyboard.h"

// echo prints the given argument. The first "program".
int echo(char *input)
{
  printString(input);
  printString("\n");
}

void parseAndRunCommand(char *command)
{
  char commandParts[10][10];

  int argIdx = 0;
  int j = 0;
  // Specify whether we are parsing the command itself or the argument
  for (int i = 0; i < strLen(command); i++)
  {

    if (command[i] == '\0' || command[i] == ' ' || command[i] == 0)
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

    char *baseCommand = commandParts[0];
    char *firstArg = commandParts[1];
    
    if (strCmp(baseCommand, "echo") == 1)
    {
      echo(firstArg);
    }
  }
}

// runShell runs a mock shell in the kernel. Hopefully one day it will run
// as an application in user mode.
void runShell()
{
  char *stdInBuffer;
  int shellRunning = 1;
  int waitingForCommand = 1;
  printString("\n[ edOS.v0.7 ]:> ");
  while (shellRunning == 1)
  {

    if (waitingForCommand == 1)
    {
      stdInBuffer = readKeyBuffer();

      for (int i = 0; i < strLen(stdInBuffer); i++)
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
      printString("[ edOS.v0.7 ]:> ");
    }
  }
}