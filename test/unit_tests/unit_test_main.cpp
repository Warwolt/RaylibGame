#include <gtest/gtest.h>

#include "core/debug/assert.h"
#include "core/debug/logging.h"

int main(int argc, char** argv) {
	initialize_logging();
	disable_log_colors();
	disable_assert_message_box();
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
