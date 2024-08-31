#include "../stdlib/stdlib.h"
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

// initMap sets all the environment variables to empty strings.
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
  environmentVariables[index] = strMallocAndStore(value);
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

// replaceEnvVariables takes in a string and replaces up to 9 environment variables with
// the corresponding values
char *replaceEnvVariables(char *arg)
{
  char argParts[10][15];

  // Initialise the command and args as empty strings
  for (int n = 0; n < 10; n++)
  {
    argParts[n][0] = '\0';
  }

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

  char *envValues[10];

  // first element in the array is reserved for the part of the string with
  // no envronment variables.
  for (int m = 1; m < 10; m++)
  {
    char *envKey = argParts[m];
    envValues[m] = getEnvValue(envKey);
    if (envValues[m] == 0)
    {
      envValues[m] = "";
    }
  }

  char *finalOutput = strConcat(preEnvVar, envValues[1]);
  char *output;

  for (int k = 2; k < 10; k++)
  {
    output = strConcat(finalOutput, envValues[k]);
    finalOutput = output;
  }

  return finalOutput;
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
  if (strCmp(baseCommand, "test") == 1)
  {
    test();
  }
}

// runShell runs a mock shell in the kernel. Hopefully one day it will run
// as an application in user mode.
void runShell()
{
  initEnvMap(environmentVariables);
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