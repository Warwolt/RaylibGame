#include <gtest/gtest.h>

#include <raylib.h>

TEST(UITests, HelloWorld) {
	Raylib_SetTraceLogLevel(LOG_WARNING); // disable verbose raylib output
	Raylib_SetConfigFlags(FLAG_WINDOW_HIDDEN | FLAG_WINDOW_ALWAYS_RUN);
	Raylib_InitWindow(0, 0, "Unit Test");
	RenderTexture2D texture = Raylib_LoadRenderTexture(64, 64);
	Raylib_BeginTextureMode(texture);
	{
		Raylib_ClearBackground(GREEN);
		Raylib_DrawText("Hello!", 0, 0, 16, WHITE);
	}
	Raylib_EndTextureMode();
	Image image = Raylib_LoadImageFromTexture(texture.texture);
	Raylib_ImageFlipVertical(&image);
	Raylib_ExportImage(image, "image.png");
	Raylib_UnloadImage(image);
	Raylib_CloseWindow();
}
