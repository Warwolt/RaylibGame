#include <gtest/gtest.h>

#include "core/debug/logging.h"

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
	RenderTexture2D texture = Raylib_LoadRenderTexture(64, 64);
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
	{
		const std::string testcase_name = testing::UnitTest::GetInstance()->current_test_info()->name();
		const std::filesystem::path testfile_path = std::filesystem::path(__FILE__);
		const std::filesystem::path testfile_dir = testfile_path.parent_path();
		const std::filesystem::path testfile_name = testfile_path.stem();
		const std::filesystem::path snapshot_dir = testfile_dir / "snapshots" / testfile_name;
		const std::filesystem::path snapshot_path = snapshot_dir / (testcase_name + ".png");

		const bool should_update = false;
		if (should_update) {
			// update snapshot with actual
			std::filesystem::create_directories(snapshot_dir);
			Raylib_ExportImage(image, snapshot_path.string().c_str());
		} else {
			if (std::optional<Image> snapshot = try_load_snapshot(snapshot_path)) {
				// compare actual image with snapshot
				const bool actual_matches_snapshot = image_pixels(image) == image_pixels(snapshot.value());
				if (!actual_matches_snapshot) {
					const std::filesystem::path report_dir = "./snapshot_report";
					const std::filesystem::path diff_dir = report_dir / "diffs" / testfile_name;
					const std::filesystem::path diff_path = diff_dir / (testcase_name + ".png");
					std::filesystem::create_directories(diff_dir);
					Raylib_ExportImage(image, diff_path.string().c_str());
					ADD_FAILURE() << "Actual image in test did not match snapshot image, see test report for more info.";
				}
			} else {
				// snapshot doesn't exist, write actual as snapshot
				std::filesystem::create_directories(snapshot_dir);
				Raylib_ExportImage(image, snapshot_path.string().c_str());
			}
		}
	}
}
