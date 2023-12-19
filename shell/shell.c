#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "shell.h"

char *environmentVariables[100];

// getEnvValue takes a key and a map and retreives the given value
// stored there.
char *getEnvValue(char *key)
{
  unsigned long hashedKey = hash(key);
  int index = hashedKey % 100;
  char *value = environmentVariables[index];
  if (value == "")
  {
    return 0;
  }
  return value;
}

// initMap takes a map and sets all the values to empty strings.
void initEnvMap()
{
  for (int i = 0; i < 100; i++)
  {
    environmentVariables[i] = "";
  }
}

// storeEnvValue stores the given value with the key in the provided map
// TODO: can only store one env for some reason
void storeEnvValue(char *key, char *value)
{
  unsigned long hashedKey = hash(key);
  int index = hashedKey % 100;
  environmentVariables[index] = strAllocAndStore(value);
}

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

void export(char *expression)
{
  char commandParts[10][10];
  int argIdx = 0;
  int j = 0;
  // split command into key and value
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

  char *variableName = commandParts[0];
  char *valueToStore = commandParts[1];

  storeEnvValue(variableName, valueToStore);
}

// replaceEnvVariables takes in a string and replaces any environment variables with
// the corresponding values
char *replaceEnvVariables(char *arg)
{
  char argParts[10][15];
  // Initialise the command and args as empty strings
  argParts[0][0] = "\0";
  argParts[1][0] = "\0";

  int argIdx = 0;
  int j = 0;
  // Specify whether we are parsing the command itself or the argument
  for (int i = 0; i <= strLen(arg); i++)
  {
    if (arg[i] == '\n')
    {
      continue;
    }

    if (arg[i] == ' ' || arg[i] == 0)
    {
      argParts[argIdx][j] = '\0';
      break;
    }

    if (arg[i] == '$' || (argIdx > 0 && arg[i] == ' ') || arg[i] == '\0')
    {
      argParts[argIdx][j] = '\0';
      argIdx++;
      j = 0;
    }
    else
    {
      argParts[argIdx][j] = arg[i];
      j++;
    }
  }

  char *preEnvVar = argParts[0];
  char *envVarKey = argParts[1];

  char *envVar = getEnvValue(envVarKey);
  if (envVar == 0)
  {
    char *noEnvValue = strAllocAndStore(preEnvVar);
    return noEnvValue;
  }

  char *finalOutput = strConcat(preEnvVar, envVar);

  return finalOutput;
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
  char *firstArg = replaceEnvVariables(commandParts[1]);

  if (strCmp(baseCommand, "echo") == 1)
  {
    echo(firstArg);
  }
  if (strCmp(baseCommand, "help") == 1)
  {
    help();
  }
  if (strCmp(baseCommand, "export") == 1)
  {
    export(firstArg);
  }
}

// runShell runs a mock shell in the kernel. Hopefully one day it will run
// as an application in user mode.
void runShell()
{
  initEnvMap(environmentVariables);
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