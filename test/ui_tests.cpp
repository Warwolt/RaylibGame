#include <gtest/gtest.h>

#include <raylib.h>

#include <functional>

class UITests : public ::testing::Test {
public:
	void SetUp() {
		Raylib_SetTraceLogLevel(LOG_WARNING); // disable verbose raylib output
		Raylib_SetConfigFlags(FLAG_WINDOW_HIDDEN | FLAG_WINDOW_ALWAYS_RUN);
		Raylib_InitWindow(0, 0, "Unit Test");
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
		Raylib_DrawText("Hello!", 0, 0, 16, WHITE);
	});
	Raylib_ExportImage(image, "image.png");
	Raylib_UnloadImage(image);
}
