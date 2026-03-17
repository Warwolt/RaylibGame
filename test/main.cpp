#include <gtest/gtest.h>

#include "core/debug/logging.h"

int main(int argc, char** argv) {
	// initialize_unit_test_logging();
	// enable_unit_test_debug_exit();
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
