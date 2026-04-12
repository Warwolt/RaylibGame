#include "snapshots.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

namespace snapshots {

	const std::string SNAPSHOT_DIR = "snapshot_report";
	const std::string SNAPSHOT_INDEX_HTML = SNAPSHOT_DIR + "/index.html";

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

	void initialize_snapshot_tests(int argc, char** argv) {
		//
	}

	std::string snapshot_report_html() {
		return std::format(HTML_TEMPLATE, "<p>Hello world</p>");
	}

	void generate_snapshot_report() {
		/* Create report HTML file */
		std::filesystem::create_directory(SNAPSHOT_DIR);
		std::ofstream report_file;
		report_file.open(SNAPSHOT_INDEX_HTML);
		report_file << snapshot_report_html() << std::endl;
		report_file.close();
	}

} // namespace snapshots
