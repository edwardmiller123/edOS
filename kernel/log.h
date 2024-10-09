#ifndef LOG
#define LOG

typedef enum LogLevel {
    ERROR,
    WARNING,
    INFO,
} LogLevel;

// setLogLevel sets the global logging level
void setLogLevel(LogLevel logLevel);

void kLogInfo(char* msg);

void kLogWarning(char* msg);

void kLogError(char* msg);

#endif