#include "test/snapshots.h"

#include "core/debug/logging.h"

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

namespace snapshots {

	const std::filesystem::path REPORT_DIRECTORY = "snapshot_report";
	const std::filesystem::path SNAPSHOT_DIRECTORY = "test\\snapshots";

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
	};

	SnapshotTestContext g_context;

	std::filesystem::path snapshot_directory(std::string suite_name) {
		return SNAPSHOT_DIRECTORY / suite_name;
	}

	static std::filesystem::path snapshot_report_directory(std::string suite_name) {
		return REPORT_DIRECTORY / suite_name;
	}

	std::filesystem::path snapshot_filepath(std::string suite_name, std::string test_name) {
		return snapshot_directory(suite_name) / (test_name + ".png");
	}

	static std::filesystem::path snapshot_diff_filepath(std::string test_suite_name, std::string test_name) {
		return snapshot_report_directory(test_suite_name) / (test_name + ".png");
	}

	static std::string report_header_html(std::string title) {
		const auto now = std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now());
		const std::string timestamp = std::format("{:%Y-%m-%d %H:%M}", now);
		return R"(<h1 style="margin-bottom: 0">)" + title + "</h1>" + R"(<p style="margin-top: 0">)" + timestamp + "</p>";
	}

	static std::string snapshot_stats_html(int num_passed_snapshots, int num_failed_snapshots) {
		return "<p>✅ Passed snapshots: " + std::to_string(num_passed_snapshots) + "<br/>❌ Failed snapshots: " + std::to_string(num_failed_snapshots) + "</p>";
	}

	static std::string snapshot_list_html(std::string title, const std::vector<SnapshotTestSuite>& suites, bool only_failed) {
		std::string list_html;
		list_html += "<ol>";
		for (const SnapshotTestSuite& suite : suites) {
			list_html += "<li>";
			list_html += R"(<a href=")" + suite.path.string() + "\">" + suite.name + "</a>";
			if (!suite.tests.empty())
				list_html += "<ol>";
			for (const SnapshotTestCase& test : suite.tests) {
				if (only_failed && test.result != SnapshotTestResult::Failed) {
					continue;
				}
				list_html += "<li>";
				list_html += R"(<a href=")" + suite.path.string() + "#" + test.name + "\">" + test.name + "</a>";
				list_html += "</li>";
			}
			if (!suite.tests.empty())
				list_html += "</ol>";
			list_html += "</li>";
		}
		list_html += "</ol>";

		return "<h2>" + title + "</h2>" + list_html;
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
		html_body += snapshot_stats_html(num_passed_snapshots, num_failed_snapshots);
		if (!g_context.failed_suites.empty()) {
			html_body += snapshot_list_html("Failures", g_context.failed_suites, true);
		}
		if (!g_context.updated_suites.empty()) {
			html_body += snapshot_list_html("Updates", g_context.updated_suites, true);
		}
		if (!g_context.all_suites.empty()) {
			html_body += snapshot_list_html("Snapshots", g_context.all_suites, false);
		}

		return std::format(HTML_TEMPLATE, html_body);
	}

	static std::string snapshot_suite_html(const SnapshotTestSuite& suite) {
		int num_passed_snapshots = 0;
		int num_failed_snapshots = 0;
		int num_updated_snapshots = 0;
		for (const SnapshotTestCase& test : suite.tests) {
			if (test.result == SnapshotTestResult::Failed) {
				num_failed_snapshots++;
			} else {
				num_passed_snapshots++;
				if (test.result == SnapshotTestResult::Updated) {
					num_updated_snapshots++;
				}
			}
		}

		/* Add page header */
		std::string html_body;
		html_body += report_header_html("Snapshot Test Report: " + suite.name);
		html_body += R"(<a href="../index.html">Back to summary</a>)";
		html_body += snapshot_stats_html(num_passed_snapshots, num_failed_snapshots);

		/* List failed snapshots */
		if (num_failed_snapshots > 0) {
			html_body += "<h2>Failures</h2>";
			html_body += "<ol>";
			for (const SnapshotTestCase& test : suite.tests) {
				if (test.result == SnapshotTestResult::Failed) {
					html_body += "<li>";
					html_body += std::format(R"(<a href="#{}">{}</a>)", test.name, test.name);
					html_body += "</li>";
				}
			}
			html_body += "</ol>";
		}

		/* List updated snapshots*/
		if (num_updated_snapshots > 0) {
			html_body += "<h2>Updates</h2>";
			html_body += "<ol>";
			for (const SnapshotTestCase& test : suite.tests) {
				if (test.result == SnapshotTestResult::Updated) {
					html_body += "<li>";
					html_body += std::format(R"(<a href="#{}">{}</a>)", test.name, test.name);
					html_body += "</li>";
				}
			}
			html_body += "</ol>";
		}

		/* Display snapshots */
		html_body += "<h2>Snapshots</h2>";
		html_body += "<ol>";
		for (const SnapshotTestCase& test : suite.tests) {
			html_body += "<li><a href=#" + test.name + ">" + test.name + "</a></li>";
		}
		html_body += "</ol>";
		for (size_t i = 0; i < suite.tests.size(); i++) {
			const SnapshotTestCase& test = suite.tests[i];
			/* Test name */
			html_body += std::format("<h3 id=\"{}\">", test.name);
			html_body += test.result == SnapshotTestResult::Failed ? "❌ " : "✅ ";
			html_body += std::format("{}. ", i + 1);
			html_body += test.name;
			if (test.result == SnapshotTestResult::Failed)
				html_body += " (failed)";
			if (test.result == SnapshotTestResult::Updated)
				html_body += " (updated)";
			html_body += "</h3>";

			/* Snapshot */
			html_body += "<div style=\"display: flex; align-items: center\">";
			std::filesystem::path snapshot_path = "..\\.." / snapshot_filepath(suite.name, test.name);
			std::filesystem::path diff_path = "..\\.." / snapshot_diff_filepath(suite.name, test.name);
			switch (test.result) {
				case SnapshotTestResult::Passed:
				case SnapshotTestResult::Updated:
					html_body += std::format("<img src=\"{}\">", snapshot_path.string());
					break;

				case SnapshotTestResult::Failed:
					html_body += std::format("<img src=\"{}\">", snapshot_path.string());
					html_body += "<p style=\"margin: 1em\">➡️</p>";
					html_body += std::format("<img src=\"{}\">", diff_path.string());
					break;
			}
			html_body += "</div>";
		}

		return std::format(HTML_TEMPLATE, html_body);
	}

	static void report_snapshot(std::string suite_name, std::string test_name, SnapshotTestResult test_result) {
		SnapshotTestCase test_case = {
			.name = test_name,
			.result = test_result,
		};
		auto it = std::find_if(g_context.all_suites.begin(), g_context.all_suites.end(), [&](const SnapshotTestSuite& suite) {
			return suite.name == suite_name;
		});
		if (it == g_context.all_suites.end()) {
			g_context.all_suites.push_back(
				SnapshotTestSuite {
					.name = suite_name,
					.path = suite_name + "/index.html",
					.tests = { test_case },
				}
			);
		} else {
			it->tests.push_back(test_case);
		}
	}

	bool should_update_snapshots() {
		return g_context.should_update_snapshots;
	}

	void report_passed_snapshot(std::string suite_name, std::string test_name) {
		report_snapshot(suite_name, test_name, SnapshotTestResult::Passed);
	}

	void report_failed_snapshot(std::string suite_name, std::string test_name) {
		report_snapshot(suite_name, test_name, SnapshotTestResult::Failed);
	}

	void report_updated_snapshot(std::string suite_name, std::string test_name) {
		report_snapshot(suite_name, test_name, SnapshotTestResult::Updated);
	}

	std::optional<Image> try_load_snapshot(std::string suite_name, std::string test_name) {
		std::filesystem::path path = snapshot_filepath(suite_name, test_name);
		if (std::filesystem::exists(path)) {
			return Raylib_LoadImage(path.string().c_str());
		}
		return {};
	}

	void save_snapshot(const Image& snapshot, std::string suite_name, std::string test_name) {
		std::filesystem::create_directories(snapshot_directory(suite_name));
		Raylib_ExportImage(snapshot, snapshot_filepath(suite_name, test_name).string().c_str());
	}

	void save_snapshot_diff(const Image& snapshot, std::string suite_name, std::string test_name) {
		std::filesystem::create_directories(snapshot_report_directory(suite_name));
		Raylib_ExportImage(snapshot, snapshot_diff_filepath(suite_name, test_name).string().c_str());
	}

	Image render_image(Vector2 image_size, std::function<void()> render) {
		RenderTexture2D texture = Raylib_LoadRenderTexture(image_size.x, image_size.y);
		Raylib_BeginTextureMode(texture);
		Raylib_ClearBackground(BLACK);
		render();
		Raylib_EndTextureMode();
		Image image = Raylib_LoadImageFromTexture(texture.texture);
		Raylib_ImageFlipVertical(&image);
		return image;
	}

	std::vector<Color> image_pixels(const Image& image) {
		Color* pixels_ptr = LoadImageColors(image);
		std::vector<Color> pixels = std::vector<Color>(pixels_ptr, pixels_ptr + image.width * image.height);
		UnloadImageColors(pixels_ptr);
		return pixels;
	}

	void initialize_snapshot_tests(int argc, char** argv) {
		bool clean_snapshots = false;
		for (int i = 0; i < argc; i++) {
			std::string arg = std::string(argv[i]);
			if (arg == "--update-snapshots" || arg == "-u") {
				g_context.should_update_snapshots = true;
			}

			if (arg == "--clean-snapshots") {
				clean_snapshots = true;
			}
		}

		if (clean_snapshots) {
			std::filesystem::remove_all(REPORT_DIRECTORY);
			LOG_INFO("removed %s", REPORT_DIRECTORY.string().c_str());

			std::filesystem::remove_all(SNAPSHOT_DIRECTORY);
			LOG_INFO("removed %s", SNAPSHOT_DIRECTORY.string().c_str());
		}
	}

	void generate_snapshot_report() {
		/* Copy over failed tests */
		for (const SnapshotTestSuite& suite : g_context.all_suites) {
			bool has_any_failures = false;
			bool has_any_updates = false;
			for (const SnapshotTestCase& test : suite.tests) {
				if (test.result == SnapshotTestResult::Failed) {
					has_any_failures = true;
				}
				if (test.result == SnapshotTestResult::Updated) {
					has_any_updates = true;
				}
			}
			if (has_any_failures) {
				g_context.failed_suites.push_back(suite);
			} else if (has_any_updates) {
				g_context.updated_suites.push_back(suite);
			}
		}

		/* Create report HTML file */
		std::filesystem::create_directory(REPORT_DIRECTORY);
		std::ofstream report_file;
		report_file.open(REPORT_DIRECTORY / "index.html");
		report_file << snapshot_report_html() << std::endl;
		report_file.close();

		/* Create suite HTML files */
		for (const SnapshotTestSuite& suite : g_context.all_suites) {
			std::filesystem::create_directory(REPORT_DIRECTORY / suite.path.parent_path());
			std::ofstream suite_file;
			suite_file.open(REPORT_DIRECTORY / suite.path);
			suite_file << snapshot_suite_html(suite) << std::endl;
			suite_file.close();
		}

		if (!g_context.all_suites.empty()) {
			printf("\nSnapshot test report updated: %s\n", "./snapshot_report/index.html");
		}
	}
} // namespace snapshots
