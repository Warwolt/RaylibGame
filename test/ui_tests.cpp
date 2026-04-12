#include "core/debug/logging.h"
#include "test/snapshots.h"

#include <gtest/gtest.h>
#include <raylib.h>

#include <filesystem>
#include <functional>

constexpr int SCREEN_WIDTH = 768;
constexpr int SCREEN_HEIGHT = 432;

class UITests : public ::testing::Test {
public:
	void SetUp() {
		Raylib_SetTraceLogLevel(LOG_WARNING);
		Raylib_SetConfigFlags(FLAG_WINDOW_HIDDEN);
		Raylib_InitWindow(1, 1, "Unit Test");
	}

	void TearDown() {
		Raylib_CloseWindow();
	}
};

Image render_image(int width, int height, std::function<void()> render) {
	RenderTexture2D texture = Raylib_LoadRenderTexture(width, height);
	Raylib_BeginTextureMode(texture);
	render();
	Raylib_EndTextureMode();
	Image image = Raylib_LoadImageFromTexture(texture.texture);
	Raylib_ImageFlipVertical(&image);
	return image;
}

TEST_F(UITests, HelloWorld) {
	Image image = render_image(SCREEN_WIDTH, SCREEN_HEIGHT, []() {
		Raylib_ClearBackground(BLUE);
		Raylib_DrawText("Test", 0, 0, 16, WHITE);
	});
	EXPECT_SNAPSHOT_EQ(image);
}
