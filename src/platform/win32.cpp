#include "platform/win32.h"

#include <raylib.h>
#include <shlwapi.h>

#include <stdio.h>

typedef struct RunCommandThreadArgs {
	const char* command;
	void (*on_command_done)(int exit_code);
} RunCommandThreadArgs;

static DWORD WINAPI run_command_thread(LPVOID void_args) {
	RunCommandThreadArgs* args = (RunCommandThreadArgs*)void_args;

	/* Setup pipes for stdout and stderr */
	HANDLE stdout_read = NULL;
	HANDLE stdout_write = NULL;
	SECURITY_ATTRIBUTES security_attributes = {
		.nLength = sizeof(SECURITY_ATTRIBUTES),
		.lpSecurityDescriptor = NULL,
		.bInheritHandle = TRUE,
	};
	BOOL pipe_was_created = CreatePipe(&stdout_read, &stdout_write, &security_attributes, 0);
	if (!pipe_was_created) {
		fprintf(stderr, "%s:%d CreatePipe failed\n", __FILE__, __LINE__);
		return 1;
	}

	/* Ensure the read handle to the pipe is not inherited. */
	SetHandleInformation(stdout_read, HANDLE_FLAG_INHERIT, 0);

	/* Create process */
	PROCESS_INFORMATION process_info = { 0 };
	STARTUPINFOA startup_info = {
		.cb = sizeof(STARTUPINFOA),
		.dwFlags = STARTF_USESTDHANDLES,
		.hStdInput = NULL,
		.hStdOutput = stdout_write,
		.hStdError = stdout_write,
	};
	BOOL process_was_created = CreateProcessA(
		NULL, // lpApplicationName
		(LPSTR)(args->command), // lpCommandLine
		NULL, // lpProcessAttributes
		NULL, // lpThreadAttributes
		TRUE, // bInheritHandles
		0, // dwCreationFlags
		NULL, // lpEnvironment
		NULL, // lpCurrentDirectory
		&startup_info, // lpStartupInfo
		&process_info // lpProcessInformation
	);
	if (!process_was_created) {
		fprintf(stderr, "%s:%d CreateProcess failed: %d\n", __FILE__, __LINE__, GetLastError());
	}

	/* Close write end of the pipe in the parent process */
	CloseHandle(stdout_write);

	/* Print stdout of command */
	while (1) {
		char buffer[4096] = "";
		DWORD num_read_bytes = 0;
		BOOL success = ReadFile(stdout_read, buffer, sizeof(buffer) - 1, &num_read_bytes, NULL);
		if (!success || num_read_bytes == 0) {
			break;
		}

		buffer[num_read_bytes] = '\0';
		printf("%s", buffer);
		fflush(stdout);
	}

	/* Wait for process to finish */
	WaitForSingleObject(process_info.hProcess, INFINITE);

	/* Pass exit code to callback */
	if (args->on_command_done != NULL) {
		DWORD exit_code = 0;
		GetExitCodeProcess(process_info.hProcess, &exit_code);
		args->on_command_done(exit_code);
	}

	/* Free thread args */
	free((void*)args);

	return 0;
}

void Win32_set_process_dpi_aware(void) {
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

void Win32_show_error_message_box(const std::string& text) {
	HWND handle = (HWND)Raylib_GetWindowHandle();
	MessageBoxA(handle, text.c_str(), "Error", MB_OK | MB_ICONERROR);
}

std::string Win32_get_executable_directory() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	PathRemoveFileSpecA(buffer);
	return buffer;
}

bool Win32_file_exists(const std::string& full_path) {
	return PathFileExistsA(full_path.c_str());
}

bool Win32_copy_file(const std::string& src_path, const std::string& dst_path) {
	return CopyFileA(src_path.c_str(), dst_path.c_str(), false) != 0;
}

uint64_t Win32_get_file_last_modified(const std::string& file_path) {
	WIN32_FILE_ATTRIBUTE_DATA file_info;
	if (!GetFileAttributesExA(file_path.c_str(), GetFileExInfoStandard, &file_info)) {
		return 0;
	}

	ULARGE_INTEGER ull;
	ull.LowPart = file_info.ftLastWriteTime.dwLowDateTime;
	ull.HighPart = file_info.ftLastWriteTime.dwHighDateTime;

	return ull.QuadPart;
}

void Win32_run_command(const std::string& command, void (*on_command_done)(int exit_code)) {
	RunCommandThreadArgs* thread_args = (RunCommandThreadArgs*)malloc(sizeof(RunCommandThreadArgs));
	*thread_args = RunCommandThreadArgs {
		.command = _strdup(command.c_str()),
		.on_command_done = on_command_done,
	};
	CreateThread(NULL, 0, run_command_thread, thread_args, 0, NULL);
}
