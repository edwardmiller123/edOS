#include "../drivers/screen.h"
#include "../drivers/keyboard.h"

// echo prints the given argument.
int echo(char *input)
{
  printString("\n");
  printString(input);
  printString("\n");
}

void parseAndRunCommand(char *command)
{
  char *baseCommand;
  char *argToPass;

  // Specify whether we are parsing the command itself or the argument
  int commandSection = 1;
  for (int i = 0; i < strLen(command); i++)
  {

    if (command[i] == ' ')
    {
      commandSection == 2;
    }

    if (command[i] == '\0' || command[i] == ' ' || command[i] == 0)
    {
      continue;
    }

    switch (commandSection)
    {
    case 1:
      baseCommand[i] = command[i];
      break;
    case 2:
      argToPass[i] = command[i];
      break;
    }
  }

  if (baseCommand == "echo")
  {
    echo(argToPass);
  }
}

// runShell runs a mock shell in the kernel. Hopefully one day it will run
// as an application in user mode.
void runShell()
{
  char *stdInBuffer;
  int shellRunning = 1;
  int waitingForCommand = 1;
  print_string_at("[ edOS.v0.7 ] > ", 0, 10);
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
      // printString("[ edOS.v0.7 ] > ");
    }
  }
}