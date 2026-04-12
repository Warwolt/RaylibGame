#include "game/ui.h"

#include "core/debug/logging.h"
#include "test/snapshots.h"

#include <gtest/gtest.h>
#include <raylib.h>

#include <filesystem>
#include <functional>

constexpr Vector2 SCREEN_SIZE = { 768, 432 };

class UILayoutTests : public ::testing::Test {
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

Image render_image(std::function<void()> render) {
	RenderTexture2D texture = Raylib_LoadRenderTexture(SCREEN_SIZE.x, SCREEN_SIZE.y);
	Raylib_BeginTextureMode(texture);
	render();
	Raylib_EndTextureMode();
	Image image = Raylib_LoadImageFromTexture(texture.texture);
	Raylib_ImageFlipVertical(&image);
	return image;
}

TEST_F(UILayoutTests, BoxLayout_100_100_Gives_50_50) {
	ResourceManager resources;
	ui::Element root = {
		.content = ui::Box {
			.direction = ui::Direction::Horizontal,
			.children = {
				ui::Element {
					.style = {
						.width = ui::RelativeSize(100),
						.background_color = RED,
					},
				},
				ui::Element {
					.style = {
						.width = ui::RelativeSize(100),
						.background_color = GREEN,
					},
				}
			}
		},
	};

	ui::layout_element(resources, SCREEN_SIZE, &root);
	Image image = render_image([&]() { ui::draw_element(resources, root); });

	EXPECT_SNAPSHOT_EQ(image);
}
