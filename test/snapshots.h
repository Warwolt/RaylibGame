#pragma once

#include <raylib.h>

#include <filesystem>
#include <optional>
#include <string>

#define EXPECT_SNAPSHOT_EQ(image)                                                                                         \
	do {                                                                                                                  \
		const std::string _suite_name = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();         \
		const std::string _test_name = testing::UnitTest::GetInstance()->current_test_info()->name();                     \
		const Image _actual = (image);                                                                                    \
		if (std::optional<Image> _snapshot = snapshots::try_load_snapshot(_suite_name, _test_name)) {                     \
			if (image_pixels(_actual) == image_pixels(_snapshot.value())) {                                               \
				snapshots::report_passed_snapshot(_suite_name, _test_name);                                               \
			} else {                                                                                                      \
				if (snapshots::should_update_snapshots()) {                                                               \
					std::string snapshot_filepath = snapshots::snapshot_filepath(_suite_name, _test_name).string();       \
					printf("[   INFO   ] Updating snapshot file \"%s\"\n", snapshot_filepath.c_str());                    \
					snapshots::report_updated_snapshot(_suite_name, _test_name);                                          \
					snapshots::save_snapshot_diff(_snapshot.value(), _suite_name, _test_name);                            \
					snapshots::save_snapshot(_actual, _suite_name, _test_name);                                           \
				} else {                                                                                                  \
					snapshots::report_failed_snapshot(_suite_name, _test_name);                                           \
					snapshots::save_snapshot_diff(_actual, _suite_name, _test_name);                                      \
					ADD_FAILURE() << "Actual image in test did not match snapshot image, see test report for more info."; \
				}                                                                                                         \
			}                                                                                                             \
		} else {                                                                                                          \
			std::string snapshot_filepath = snapshots::snapshot_filepath(_suite_name, _test_name).string();               \
			printf("[   INFO   ] No snapshot found, saving new file \"%s\"\n", snapshot_filepath.c_str());                \
			snapshots::report_passed_snapshot(_suite_name, _test_name);                                                   \
			snapshots::save_snapshot(_actual, _suite_name, _test_name);                                                   \
		}                                                                                                                 \
	} while (0)

namespace snapshots {

	std::filesystem::path snapshot_directory(std::string test_suite_name);
	std::filesystem::path snapshot_filepath(std::string test_suite_name, std::string test_name);
	bool should_update_snapshots();

	void report_passed_snapshot(std::string test_suite_name, std::string test_name);
	void report_failed_snapshot(std::string test_suite_name, std::string test_name);
	void report_updated_snapshot(std::string test_suite_name, std::string test_name);

	std::optional<Image> try_load_snapshot(std::string test_suite_name, std::string test_name);
	void save_snapshot(const Image& snapshot, std::string test_suite_name, std::string test_name);
	void save_snapshot_diff(const Image& snapshot, std::string test_suite_name, std::string test_name);

	void initialize_snapshot_tests(int argc, char** argv);
	void generate_snapshot_report();

} // namespace snapshots
