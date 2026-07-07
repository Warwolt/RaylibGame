#include <gtest/gtest.h>

#include "test/snapshot_tests/snapshots.h"

#include "core/debug/logging.h"
#include "core/debug/profiling.h"

int main(int argc, char** argv) {
	initialize_logging();
	disable_log_colors();
	PROFILING_STARTUP_PROFILER();
	testing::InitGoogleTest(&argc, argv);
	snapshots::initialize_snapshot_tests(argc, argv);

	int result = RUN_ALL_TESTS();
	snapshots::generate_snapshot_report();
	PROFILING_SHUTDOWN_PROFILER();

	return result;
}
