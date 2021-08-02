#pragma once

#include "defines.h"

typedef enum log_level {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5
} log_level;

// Initializes the logging system.
// TODO: Allow user to define the log output stream.
bool log_startup();

// Shuts down the logging system.
void log_shutdown();

// Logs a message using 'platform_log_to_stdout'.
// Consider using the defined macros for logging.
void log_output(log_level level, const char* message, ...);

#define LogFatal(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);
#define LogError(message, ...) log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#define LowWarn(message, ...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#define LogInfo(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#define LogDebug(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#define LogTrace(message, ...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);