#include "core/debug/assert.h"

static bool g_skip_assert_message_box = false;

void disable_assert_message_box() {
	g_skip_assert_message_box = true;
}

bool should_skip_assert_message_box() {
	return g_skip_assert_message_box;
}
