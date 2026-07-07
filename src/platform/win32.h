#pragma once

#include <string>

// window
void Win32_set_process_dpi_aware(void);
void Win32_show_error_message_box(const std::string& text);
bool Win32_show_assert_message_box(const std::string& text); // true if should break debugger

// file
std::string Win32_get_executable_directory();
bool Win32_file_exists(const std::string& full_path);
bool Win32_copy_file(const std::string& src_path, const std::string& dst_path); // true if file copied
uint64_t Win32_get_file_last_modified(const std::string& file_path);

// command
void Win32_run_command(const std::string& command, void (*on_command_done)(int exit_code));
