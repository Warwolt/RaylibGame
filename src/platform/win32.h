#pragma once

#include "platform/lean_mean_windows.h"

#include <processthreadsapi.h>
#include <stdbool.h>
#include <stdint.h>

// window
void Win32_set_process_dpi_aware(void);
void Win32_show_error_message_box(const char* text);

// file
void Win32_get_executable_directory(char* buffer);
bool Win32_file_exists(const char* full_path);
bool Win32_copy_file(const char* src_path, const char* dst_path); // true if file copied
uint64_t Win32_get_file_last_modified(const char* file_path);

// command
void Win32_run_command(const char* command, void (*on_command_done)(int exit_code));
