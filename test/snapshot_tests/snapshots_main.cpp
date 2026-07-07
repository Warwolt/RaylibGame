#include <gtest/gtest.h>

#include "test/snapshot_tests/snapshots.h"

#include "core/debug/assert.h"
#include "core/debug/logging.h"

int main(int argc, char** argv) {
	initialize_logging();
	disable_log_colors();
	disable_assert_message_box();
	testing::InitGoogleTest(&argc, argv);
	snapshots::initialize_snapshot_tests(argc, argv);

	int result = RUN_ALL_TESTS();
	snapshots::generate_snapshot_report();

	return result;
}
