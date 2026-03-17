#include "core/debug/logging.h"

#include "platform/lean_mean_windows.h"

#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#pragma warning(disable: 4005) // consoleapi2.h from windows.h has conflicting macros

// foreground color
#define COLOR_BLACK "\033[30m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_PURPLE "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"
#define COLOR_RESET "\033[0m"

// background color
#define BACKGROUND_BLACK "\033[40m"
#define BACKGROUND_RED "\033[41m"
#define BACKGROUND_GREEN "\033[42m"
#define BACKGROUND_YELLOW "\033[43m"
#define BACKGROUND_BLUE "\033[44m"
#define BACKGROUND_PURPLE "\033[45m"
#define BACKGROUND_CYAN "\033[46m"
#define BACKGROUND_WHITE "\033[47m"

static bool g_unit_test_mode = false;

static const char* log_level_to_str(LogLevel level) {
	switch (level) {
		case LogLevel_Debug:
			return "[DEBUG]";
		case LogLevel_Info:
			return "[INFO]";
		case LogLevel_Warning:
			return "[WARNING]";
		case LogLevel_Error:
			return "[ERROR]";
		case LogLevel_Fatal:
			return "[FATAL]";
	}
	return "";
}

static const char* log_level_color(LogLevel level) {
	switch (level) {
		case LogLevel_Debug:
			return COLOR_GREEN;
		case LogLevel_Info:
			return "";
		case LogLevel_Warning:
			return COLOR_YELLOW;
		case LogLevel_Error:
			return COLOR_RED;
		case LogLevel_Fatal:
			return BACKGROUND_RED COLOR_BLACK;
	}
	return "";
}

static char* filename_from_path(const char* path_str) {
	char* filename = (char*)path_str;
	for (char* c = (char*)path_str; *c != '\0'; c++) {
		if (*c == '\\') {
			filename = c + 1;
		}
	}
	return filename;
}

void initialize_logging() {
	// If we're debugging, we're using the Visual Studio debug output window
	// instead of a terminal that uses stdout. Skip color init in that case.
	if (IsDebuggerPresent()) {
		return;
	}

	/* Enable colors */
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
}

void initialize_unit_test_logging() {
	g_unit_test_mode = true;
	initialize_logging();
}

void debug_log(LogLevel log_level, const char* filename, int line, const char* fmt, ...) {
	const bool debugger_is_present = IsDebuggerPresent();
	const bool should_add_formatting = !g_unit_test_mode;
	char buffer[1024] = { 0 };
	int offset = 0;

	if (should_add_formatting) {
		/* Add color */
		if (!debugger_is_present) {
			offset += snprintf(buffer + offset, 1024 - offset, log_level_color(log_level));
		}

		/* Add timestamp */
		time_t t = time(0); // get time now
		struct tm* now = localtime(&t);
		offset += snprintf(buffer + offset, 1024 - offset, "%02d-%02d-%02d ", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday); // year
		offset += snprintf(buffer + offset, 1024 - offset, "%02d:%02d ", now->tm_hour, now->tm_min); // hour

		/* File and line */
		offset += snprintf(buffer + offset, 1024 - offset, "%s:%d ", filename_from_path(filename), line);

		/* Log level */
		offset += snprintf(buffer + offset, 1024 - offset, "%s ", log_level_to_str(log_level));
	}

	/* Add message */
	va_list args;
	va_start(args, fmt);
	offset += vsnprintf(buffer + offset, 1024 - offset, fmt, args);
	snprintf(buffer + offset, 1024 - offset, "\n");
	va_end(args);

	/* Output */
	if (debugger_is_present) {
		// log to Visual Studio debug output window
		OutputDebugStringA(buffer);
	} else {
		// log to console
		bool is_error = log_level == LogLevel_Error || log_level == LogLevel_Fatal;
		FILE* stream = is_error ? stderr : stdout;
		fprintf(stream, buffer);
		if (should_add_formatting) {
			fprintf(stream, COLOR_RESET);
		}
		fflush(stream);
	}
}
