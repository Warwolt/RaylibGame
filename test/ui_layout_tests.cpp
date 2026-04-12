#include "game/ui.h"

#include "core/debug/logging.h"
#include "test/snapshots.h"

#include <gtest/gtest.h>
#include <raylib.h>

#include <filesystem>
#include <functional>

constexpr int SCREEN_WIDTH = 768;
constexpr int SCREEN_HEIGHT = 432;
constexpr Vector2 SCREEN_SIZE = { SCREEN_WIDTH, SCREEN_HEIGHT };

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
	RenderTexture2D texture = Raylib_LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
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

	EXPECT_EQ(std::get<ui::Box>(root.content).children[0].layout.margin_box.width, SCREEN_WIDTH / 2);
	EXPECT_EQ(std::get<ui::Box>(root.content).children[1].layout.margin_box.width, SCREEN_WIDTH / 2);
	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UILayoutTests, BoxLayout_100_25_100_Gives_37_25_37) {
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
						.width = ui::RelativeSize(25),
						.background_color = GREEN,
					},
				},
				ui::Element {
					.style = {
						.width = ui::RelativeSize(100),
						.background_color = BLUE,
					},
				}
			}
		},
	};

	ui::layout_element(resources, SCREEN_SIZE, &root);
	Image image = render_image([&]() { ui::draw_element(resources, root); });

	EXPECT_EQ(std::get<ui::Box>(root.content).children[0].layout.margin_box.width, SCREEN_WIDTH * 0.375);
	EXPECT_EQ(std::get<ui::Box>(root.content).children[1].layout.margin_box.width, SCREEN_WIDTH * 0.25);
	EXPECT_EQ(std::get<ui::Box>(root.content).children[2].layout.margin_box.width, SCREEN_WIDTH * 0.375);
	EXPECT_SNAPSHOT_EQ(image);
}
