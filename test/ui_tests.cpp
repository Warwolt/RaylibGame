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

TEST_F(UITests, HelloWorld) {
	Image image = render_to_image(64, 64, []() {
		Raylib_ClearBackground(GREEN);
		Raylib_DrawText("Test", 0, 0, 16, WHITE);
	});

	// IF should update THEN update snapshot with actual
	{
		std::string test_name = testing::UnitTest::GetInstance()->current_test_info()->name();
		std::filesystem::path testfile_dir = std::filesystem::path(__FILE__).parent_path();
		std::filesystem::path testfile_filename = std::filesystem::path(__FILE__).filename();
		std::filesystem::path snapshot_dir = testfile_dir / "snapshots" / testfile_filename;
		std::filesystem::path snapshot_filename = snapshot_dir / (test_name + ".png");
		LOG_DEBUG("%s", snapshot_filename.string().c_str());
		std::filesystem::create_directories(snapshot_dir);
		Raylib_ExportImage(image, snapshot_filename.string().c_str());
	}
	// ELSE IF snapshot exists THEN compare actual to snapshot
	{
		// check if snapshot file exists
		// if it does, load it then compare snapshot image with actual image
	}
}
