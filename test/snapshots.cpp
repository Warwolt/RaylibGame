#include "snapshots.h"

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

namespace snapshots {

	const std::filesystem::path SNAPSHOT_DIR = "snapshot_report";

	constexpr char HTML_TEMPLATE[] = R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8" />
    <title>Snapshot Test Report</title>
    <style>
    a {{
        color: blue;
    }}

    img {{
        image-rendering: pixelated;
        zoom: 1;
    }}
    </style>
</head>
<body>
    {}
</body>
</html>)";

	enum class SnapshotTestResult {
		Passed,
		Failed,
		Updated,
	};

	struct SnapshotTestCase {
		std::string name;
		SnapshotTestResult result;
	};

	struct SnapshotTestSuite {
		std::string name;
		std::filesystem::path path;
		std::vector<SnapshotTestCase> tests;
	};

	struct SnapshotTestContext {
		bool should_update_snapshots = false;
		std::vector<SnapshotTestSuite> all_suites;
		std::vector<SnapshotTestSuite> failed_suites;
		std::vector<SnapshotTestSuite> updated_suites;
	} g_context;

	static std::string report_header_html(std::string title) {
		const auto now = std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now());
		const std::string timestamp = std::format("{:%Y-%m-%d %H:%M}", now);
		return R"(<h1 style="margin-bottom: 0">)" + title + "</h1>" + R"(<p style="margin-top: 0">)" + timestamp + "</p>";
	}

	static std::string snapshot_report_html() {
		int num_passed_snapshots = 0;
		int num_failed_snapshots = 0;
		for (const SnapshotTestSuite& suite : g_context.all_suites) {
			for (const SnapshotTestCase& test : suite.tests) {
				if (test.result == SnapshotTestResult::Failed) {
					num_failed_snapshots++;
				} else {
					num_passed_snapshots++;
				}
			}
		}
		const int num_snapshots = num_passed_snapshots + num_failed_snapshots;
		const int num_suites = (int)g_context.all_suites.size();

		std::string html_body;
		html_body += report_header_html("Snapshot Test Report");
		html_body += "<p>" + std::to_string(num_snapshots) + " snapshot" + (num_snapshots == 1 ? "" : "s") + " from " + std::to_string(num_suites) + " test suite" + (num_suites == 1 ? "" : "s") + " ran." + "</p>";
		// html_body += snapshot_stats_html(num_passed_snapshots, num_failed_snapshots);
		// if (!g_context.failed_suites.empty()) {
		// 	html_body += snapshot_list_html("Failures", g_context.failed_suites, true);
		// }
		// if (!g_context.updated_suites.empty()) {
		// 	html_body += snapshot_list_html("Updates", g_context.updated_suites, true);
		// }
		// html_body += snapshot_list_html("Snapshots", g_context.all_suites, false);

		return std::format(HTML_TEMPLATE, html_body);
	}

	void initialize_snapshot_tests(int argc, char** argv) {
		//
	}

	void generate_snapshot_report() {
		/* Create report HTML file */
		std::filesystem::create_directory(SNAPSHOT_DIR);
		std::ofstream report_file;
		report_file.open(SNAPSHOT_DIR / "index.html");
		report_file << snapshot_report_html() << std::endl;
		report_file.close();
	}

} // namespace snapshots
