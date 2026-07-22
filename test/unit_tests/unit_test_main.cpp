#include <gtest/gtest.h>

#include "core/debug/assert.h"
#include "core/debug/logging.h"
#include "core/debug/profiling.h"

int main(int argc, char** argv) {
	initialize_logging();
	disable_log_colors();
	disable_assert_message_box();
	set_log_prefix("[    LOG   ]");
	PROFILING_STARTUP_PROFILER();
	testing::InitGoogleTest(&argc, argv);
	int result = RUN_ALL_TESTS();
	PROFILING_SHUTDOWN_PROFILER();
	return result;
}
