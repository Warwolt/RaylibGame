#include "core/debug/logging.h"
#include "test/snapshots.h"

#include <gtest/gtest.h>
#include <raylib.h>

#include <filesystem>
#include <functional>

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

TEST_F(UITests, HelloWorld) {
	Image image = render_to_image(100, 100, []() {
		Raylib_ClearBackground(BLUE);
		Raylib_DrawText("Test", 0, 0, 16, WHITE);
	});
	EXPECT_SNAPSHOT_EQ(image);
}
