#pragma once

typedef enum LogLevel {
	LogLevel_Debug = 0,
	LogLevel_Info = 1,
	LogLevel_Warning = 2,
	LogLevel_Error = 3,
	LogLevel_Fatal = 4,
} LogLevel;

void initialize_logging();
void initialize_unit_test_logging();
void debug_log(LogLevel level, const char* filename, int line, const char* fmt, ...);

#define LOG_DEBUG(...) debug_log(LogLevel_Debug, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) debug_log(LogLevel_Info, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) debug_log(LogLevel_Warning, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) debug_log(LogLevel_Error, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) debug_log(LogLevel_Fatal, __FILE__, __LINE__, __VA_ARGS__)
