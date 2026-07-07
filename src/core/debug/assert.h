#pragma once

#include "core/debug/logging.h"

#include "platform/lean_mean_windows.h"
#include "platform/win32.h"

#include <filesystem>
#include <stdio.h>
#include <utility>

#define ASSERT(expr, ...)                                                                     \
	if (!(expr)) {                                                                            \
		constexpr size_t SIZE = 2048;                                                         \
		char msg[SIZE];                                                                       \
		int offset = sprintf_s(msg, SIZE, "ASSERT(%s): ", #expr);                             \
		offset += sprintf_s(msg + offset, SIZE - offset, __VA_ARGS__);                        \
		LOG_FATAL("%s", msg);                                                                 \
		if (should_skip_assert_message_box()) {                                               \
			__debugbreak();                                                                   \
			std::exit(1);                                                                     \
		} else {                                                                              \
			const std::string filename = std::filesystem::path(__FILE__).filename().string(); \
			sprintf_s(msg + offset, SIZE, "\n%s:%d", filename.c_str(), __LINE__);             \
			if (Win32_show_assert_message_box(msg)) {                                         \
				__debugbreak();                                                               \
			}                                                                                 \
		}                                                                                     \
	}

#define ABORT(...)                                                                            \
	do {                                                                                      \
		constexpr size_t SIZE = 2048;                                                         \
		char msg[SIZE];                                                                       \
		int offset = sprintf_s(msg, SIZE, "ABORT: ");                                         \
		offset += sprintf_s(msg + offset, SIZE - offset, __VA_ARGS__);                        \
		LOG_FATAL("%s", msg);                                                                 \
		if (should_skip_assert_message_box()) {                                               \
			__debugbreak();                                                                   \
			std::exit(1);                                                                     \
		} else {                                                                              \
			const std::string filename = std::filesystem::path(__FILE__).filename().string(); \
			sprintf_s(msg + offset, SIZE, "\n%s:%d", filename.c_str(), __LINE__);             \
			if (Win32_show_assert_message_box(msg)) {                                         \
				__debugbreak();                                                               \
			}                                                                                 \
		}                                                                                     \
	} while (0)

void disable_assert_message_box();
bool should_skip_assert_message_box();
