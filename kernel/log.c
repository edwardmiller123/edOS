#include "log.h"
#include "drivers/screen.h"
#include "consts.h"
#include "../stdlib/stdlib.h"

static LogLevel level = INFO;

// setLogLevel sets the global logging level
void setLogLevel(LogLevel logLevel)
{
    level = logLevel;
}

void log(char *levelStr, char *msg, char attributeByte)
{
    if (msg == NULL)
    {
        kPrintStringColour("Log message cannot be NULL\n", attributeByte);
        return;
    }
    int cursorAddress = get_cursor();
    int currentRow = cursorAddress / (2 * MAX_COLS);

    int col = 0;
    kPrintStringColourAt(levelStr, col, currentRow, attributeByte);
    kPrintString(msg);
    kPrintString("\n");
}

void kLogDebug(char *msg)
{
    if (level < DEBUG)
    {
        return;
    }

    char colour = 0x03;
    char *levelStr = "DEBUG: ";
    log(levelStr, msg, colour);
}

void kLogInfo(char *msg)
{
    if (level < INFO)
    {
        return;
    }

    char colour = 0x02;
    char *levelStr = "INFO : ";
    log(levelStr, msg, colour);
}

void kLogWarning(char *msg)
{
    if (level < WARNING)
    {
        return;
    }

    char colour = 0xE;
    char *levelStr = "WARNING: ";
    log(levelStr, msg, colour);
}

void kLogError(char *msg)
{
    if (level < ERROR)
    {
        return;
    }

    char colour = 0x04;
    char *levelStr = "ERROR: ";
    log(levelStr, msg, colour);
}

void kLogFatal(char *msg)
{
    char colour = 0x04;
    char *levelStr = "FATAL: ";
    log(levelStr, msg, colour);
    hlt();
}

// kLogf logs a formatted message at the given log level
void kLogf(LogLevel level, char *msg, int args[], int argCount)
{
    if (msg == NULL)
    {
        kLogError("Log message cannot be NULL");
        return;
    }

    const partLimit = 15;

    // TODO: This uses alot of stack. May need to bump the thread stack allowence
    char msgParts[partLimit][128];
    int argIdx = 0;
    int partIdx = 0;
    int j = 0;
    for (int i = 0; i < strLen(msg); i++)
    {
        if (msg[i] == '$')
        {
            msgParts[partIdx][j] = '\0';
            partIdx++;
            j = 0;
            // intToString needs the space for the string pre allocated so we pass in the
            // next array in msgParts
            intToString(args[argIdx], msgParts[partIdx]);
            argIdx++;
            // skip again to the next part.
            partIdx++;
        }
        else
        {
            msgParts[partIdx][j] = msg[i];
            j++;
        }

        if (partIdx > partLimit)
        {
            kLogError("Log message has to many parts");
            return;
        }
        if (argIdx > argCount)
        {
            kLogError("Found to many input args");
            return;
        }
    }
    // now concatenate the parts of the string
    char *formattedMsg = msgParts[0];
    for (int i = 1; i <= partIdx; i++)
    {
        char newStr[128];
        formattedMsg = strConcat(formattedMsg, msgParts[i], newStr);
    }

    switch (level)
    {
    case DEBUG:
        kLogDebug(formattedMsg);
        break;
    case INFO:
        kLogInfo(formattedMsg);
        break;
    case WARNING:
        kLogWarning(formattedMsg);
        break;
    case ERROR:
        kLogError(formattedMsg);
        break;
    case FATAL:
        kLogFatal(formattedMsg);
        break;
    }
}