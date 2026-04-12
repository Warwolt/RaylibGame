#include "core/debug/logging.h"
#include "test/snapshots.h"

#include <gtest/gtest.h>
#include <raylib.h>

#include <filesystem>
#include <functional>

class UITests : public ::testing::Test {
public:
	void SetUp() {
		Raylib_SetTraceLogLevel(LOG_WARNING); // disable verbose raylib output
		Raylib_SetConfigFlags(FLAG_WINDOW_HIDDEN);
		Raylib_InitWindow(1, 1, "Unit Test");
	}

	void TearDown() {
		Raylib_CloseWindow();
	}
};

Image render_to_image(int width, int height, std::function<void()> render) {
	RenderTexture2D texture = Raylib_LoadRenderTexture(width, height);
	Raylib_BeginTextureMode(texture);
	render();
	Raylib_EndTextureMode();
	Image image = Raylib_LoadImageFromTexture(texture.texture);
	Raylib_ImageFlipVertical(&image);
	return image;
}

std::vector<Color> image_pixels(Image image) {
	Color* pixels_ptr = LoadImageColors(image);
	std::vector<Color> pixels = std::vector<Color>(pixels_ptr, pixels_ptr + image.width * image.height);
	UnloadImageColors(pixels_ptr);
	return pixels;
}

std::optional<Image> try_load_snapshot(std::filesystem::path snapshot_path) {
	if (std::filesystem::exists(snapshot_path)) {
		return Raylib_LoadImage(snapshot_path.string().c_str());
	}
	return {};
}

TEST_F(UITests, HelloWorld) {
	Image image = render_to_image(64, 64, []() {
		Raylib_ClearBackground(BLUE);
		Raylib_DrawText("Test", 0, 0, 16, WHITE);
	});

	// EXPECT_SNAPSHOT_EQ(image)
	do {
		const std::string _suite_name = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
		const std::string _test_name = testing::UnitTest::GetInstance()->current_test_info()->name();
		const Image _actual = (image);
		if (std::optional<Image> _snapshot = snapshots::try_load_snapshot(_suite_name, _test_name)) {
			if (image_pixels(_actual) == image_pixels(_snapshot.value())) {
				snapshots::report_passed_snapshot(_suite_name, _test_name);
			} else {
				if (snapshots::should_update_snapshots()) {
					std::string snapshot_filepath = snapshots::snapshot_filepath(_suite_name, _test_name).string();
					printf("[   INFO   ] Updating snapshot file \"%s\"\n", snapshot_filepath.c_str());
					snapshots::report_updated_snapshot(_suite_name, _test_name);
					snapshots::save_snapshot_diff(_snapshot.value(), _suite_name, _test_name);
					snapshots::save_snapshot(_actual, _suite_name, _test_name);
				} else {
					snapshots::report_failed_snapshot(_suite_name, _test_name);
					snapshots::save_snapshot_diff(_actual, _suite_name, _test_name);
					ADD_FAILURE() << "Actual image in test did not match snapshot image, see test report for more info.";
				}
			}
		} else {
			std::string snapshot_filepath = snapshots::snapshot_filepath(_suite_name, _test_name).string();
			printf("[   INFO   ] No snapshot found, saving new file \"%s\"\n", snapshot_filepath.c_str());
			snapshots::report_passed_snapshot(_suite_name, _test_name);
			snapshots::save_snapshot(_actual, _suite_name, _test_name);
		}
	} while (0);
}
