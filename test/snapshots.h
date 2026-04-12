#pragma once

#include <raylib.h>

#include <filesystem>
#include <optional>
#include <string>

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
