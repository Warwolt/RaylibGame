#include "game/ui.h"

#include <gtest/gtest.h>
#include <raylib.h>

constexpr Vector2 SCREEN_SIZE = { 768, 432 };

constexpr Vector2 size = { 100, 100 };
constexpr Vector2 top_left = { 0, 0 };
constexpr Vector2 middle = { (size.x / 2) - 1, (size.y / 2) - 1 };
constexpr Vector2 bottom_right = { size.x - 1, size.y - 1 };
constexpr Vector2 outside = { size.x + 1, size.y + 1 };

TEST(UIInteractionTests, Box_Mouse_IsHovered) {
	ResourceManager resources;
	ui::Element element = {
		.style = {
			.width = ui::AbsoluteSize(size.x),
			.height = ui::AbsoluteSize(size.y),
		},
		.content = ui::Box {},
	};

	/* Element initially not hovered */
	ui::layout_element(resources, SCREEN_SIZE, &element);
	EXPECT_FALSE(element.state.is_hovered);

	/* Hover top left */
	ui::update_element(ui::Input { .mouse_pos = top_left }, &element);
	EXPECT_TRUE(element.state.is_hovered);

	/* Hover middle */
	ui::update_element(ui::Input { .mouse_pos = middle }, &element);
	EXPECT_TRUE(element.state.is_hovered);

	/* Hover bottom right */
	ui::update_element(ui::Input { .mouse_pos = bottom_right }, &element);
	EXPECT_TRUE(element.state.is_hovered);
}

TEST(UIInteractionTests, Box_Mouse_IsActive) {
	ResourceManager resources;
	ui::Element element = {
		.style = {
			.width = ui::AbsoluteSize(size.x),
			.height = ui::AbsoluteSize(size.y),
		},
		.content = ui::Box {},
	};

	/* Element initially not active */
	ui::layout_element(resources, SCREEN_SIZE, &element);
	EXPECT_FALSE(element.state.is_active);

	/* Click top left */
	ui::update_element(ui::Input { .mouse_pos = top_left, .left_mouse_is_down = true }, &element);
	EXPECT_TRUE(element.state.is_active);

	/* Release top left */
	ui::update_element(ui::Input { .mouse_pos = top_left, .left_mouse_is_down = false }, &element);
	EXPECT_FALSE(element.state.is_active);

	/* Click outside box */
	ui::update_element(ui::Input { .mouse_pos = outside, .left_mouse_is_down = true }, &element);
	EXPECT_FALSE(element.state.is_active);
}
