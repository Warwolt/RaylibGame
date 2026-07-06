#pragma once

#include "core/debug/logging.h"

#include "platform/lean_mean_windows.h"
#include "platform/win32.h"

#include <stdio.h>
#include <utility>

#define ASSERT(expr, ...)                                                                           \
	if (!(expr)) {                                                                                  \
		constexpr size_t SIZE = 2048;                                                               \
		char msg[SIZE];                                                                             \
		int offset = sprintf_s(msg, SIZE, "%s:%d\nASSERT(%s) failed: ", __FILE__, __LINE__, #expr); \
		sprintf_s(msg + offset, SIZE - offset, __VA_ARGS__);                                        \
		LOG_FATAL("%s", msg);                                                                       \
		if (should_skip_assert_message_box()) {                                                     \
			__debugbreak();                                                                         \
			std::exit(1);                                                                           \
		} else {                                                                                    \
			if (Win32_show_assert_message_box(msg)) {                                               \
				__debugbreak();                                                                     \
			}                                                                                       \
		}                                                                                           \
	}

#define ABORT(...)                                                                 \
	do {                                                                           \
		constexpr size_t SIZE = 2048;                                              \
		char msg[SIZE];                                                            \
		int offset = sprintf_s(msg, SIZE, "%s:%d\nABORT(): ", __FILE__, __LINE__); \
		sprintf_s(msg + offset, SIZE - offset, __VA_ARGS__);                       \
		LOG_FATAL("%s", msg);                                                      \
		if (should_skip_assert_message_box()) {                                    \
			__debugbreak();                                                        \
			std::exit(1);                                                          \
		} else {                                                                   \
			if (Win32_show_assert_message_box(msg)) {                              \
				__debugbreak();                                                    \
			}                                                                      \
		}                                                                          \
	} while (0)

void enable_skip_assert_message_box();
bool should_skip_assert_message_box();
