#include "../drivers/screen.h"
#include "../drivers/keyboard.h"

// echo prints the given argument.
void echo(char *input)
{
  printString("\n");
  printString(input);
  printString("\n");
}

void parseCommand(char *command)
{
  char *baseCommand;
  char *argToPass;

  // Specify whether we are parsing the command itself or the argument
  int commandSection = 1;
  for (int i = 0; i < strLen(command); i++)
  {
    if (command[i] == " ")
    {
      commandSection = 2;
    }

    switch (commandSection)
    {
    case 1:
      if (command[i] == " ")
      {
        break;
      }
      baseCommand[i] = command[i];
    case 2:
      if (command[i] == " ")
      {
        break;
      }
      argToPass[i] = command[i];
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
    print_string_at("edOS.v0.4 >", 0, 23);
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
        } else {
            parseCommand(stdInBuffer);
            resetKeyBuffer();
            waitingForCommand = 1;
            print_string_at("\nedOS.v0.4 >", 0, 23);
        }
    }
}