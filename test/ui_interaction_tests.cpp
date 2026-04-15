#include "game/ui.h"

#include <gtest/gtest.h>
#include <raylib.h>

constexpr Vector2 SCREEN_SIZE = { 768, 432 };

constexpr Vector2 size = { 100, 100 };
constexpr Vector2 top_left = { 0, 0 };
constexpr Vector2 middle = { (size.x / 2) - 1, (size.y / 2) - 1 };
constexpr Vector2 bottom_right = { size.x - 1, size.y - 1 };
constexpr Vector2 outside = { size.x + 1, size.y + 1 };

// Child box occupies upper left quadrant of parent
//
// +---------+--------+
// |         |        |
// |  child  |        |
// |         |        |
// +---------+        |
// |      parent      |
// |                  |
// +------------------+
ui::Element box_element_with_child() {
	return ui::Element {
		.style = {
			.width = ui::AbsoluteSize(size.x),
			.height = ui::AbsoluteSize(size.y),
		},
		.content = ui::Box {
			.children = {
				ui::Element {
					.style = {
						.width = ui::AbsoluteSize(size.x / 2),
						.height = ui::AbsoluteSize(size.y / 2),
					},
					.content = ui::Box {},
				},
			},
		},
	};
}

TEST(UIInteractionTests, BoxElementWithChild_IsHovered) {
	ResourceManager resources;
	ui::Element element = box_element_with_child();
	const ui::Element& child = element.box()->children[0];

	/* Element initially not hovered */
	ui::layout_element(resources, SCREEN_SIZE, &element);
	EXPECT_EQ(element.state.is_hovered, false);
	EXPECT_EQ(child.state.is_hovered, false);

	/* Hover top left */
	ui::update_element(ui::Input { .mouse_pos = top_left }, &element);
	EXPECT_EQ(element.state.is_hovered, true);
	EXPECT_EQ(child.state.is_hovered, true);

	/* Hover middle */
	ui::update_element(ui::Input { .mouse_pos = middle }, &element);
	EXPECT_EQ(element.state.is_hovered, true);
	EXPECT_EQ(child.state.is_hovered, true);

	/* Hover bottom right */
	ui::update_element(ui::Input { .mouse_pos = bottom_right }, &element);
	EXPECT_EQ(element.state.is_hovered, true);
	EXPECT_EQ(child.state.is_hovered, false);

	/* Hover outside */
	ui::update_element(ui::Input { .mouse_pos = outside }, &element);
	EXPECT_EQ(element.state.is_hovered, false);
	EXPECT_EQ(child.state.is_hovered, false);
}

TEST(UIInteractionTests, BoxElementWithChild_IsActive) {
	ResourceManager resources;
	ui::Element element = box_element_with_child();
	const ui::Element& child = element.box()->children[0];

	/* Element initially not active */
	ui::layout_element(resources, SCREEN_SIZE, &element);
	EXPECT_EQ(element.state.is_active, false);
	EXPECT_EQ(child.state.is_active, false);

	/* Click top left */
	ui::update_element(ui::Input { .mouse_pos = top_left, .left_mouse_button = ui::KeyState::Pressed }, &element);
	EXPECT_EQ(element.state.is_active, true);
	EXPECT_EQ(child.state.is_active, true);

	/* Hold down button top left */
	ui::update_element(ui::Input { .mouse_pos = top_left, .left_mouse_button = ui::KeyState::Down }, &element);
	EXPECT_EQ(element.state.is_active, true);
	EXPECT_EQ(child.state.is_active, true);

	/* Release top left */
	ui::update_element(ui::Input { .mouse_pos = top_left, .left_mouse_button = ui::KeyState::Released }, &element);
	EXPECT_EQ(element.state.is_active, false);
	EXPECT_EQ(child.state.is_active, false);

	/* Click outside box */
	ui::update_element(ui::Input { .mouse_pos = outside, .left_mouse_button = ui::KeyState::Pressed }, &element);
	EXPECT_EQ(element.state.is_active, false);
	EXPECT_EQ(child.state.is_active, false);
}

TEST(UIInteractionTests, BoxElementWithChild_IsClicked) {
	ResourceManager resources;
	ui::Element element = box_element_with_child();
	const ui::Element& child = element.box()->children[0];

	/* Element initially not clicked */
	ui::layout_element(resources, SCREEN_SIZE, &element);
	EXPECT_EQ(element.state.is_clicked, false);
	EXPECT_EQ(child.state.is_clicked, false);

	/* Mouse down while hovering, not clicked */
	ui::update_element(ui::Input { .mouse_pos = top_left, .left_mouse_button = ui::KeyState::Pressed }, &element);
	EXPECT_EQ(element.state.is_clicked, false);
	EXPECT_EQ(child.state.is_clicked, false);

	/* Mouse up while hovering child, only is clicked */
	ui::update_element(ui::Input { .mouse_pos = top_left, .left_mouse_button = ui::KeyState::Released }, &element);
	EXPECT_EQ(element.state.is_clicked, false);
	EXPECT_EQ(child.state.is_clicked, true);

	/* Mouse down while hovering only parent, not clicked */
	ui::update_element(ui::Input { .mouse_pos = bottom_right, .left_mouse_button = ui::KeyState::Pressed }, &element);
	EXPECT_EQ(element.state.is_clicked, false);
	EXPECT_EQ(child.state.is_clicked, false);

	/* Mouse up while hovering only parent, only parent is clicked */
	ui::update_element(ui::Input { .mouse_pos = bottom_right, .left_mouse_button = ui::KeyState::Released }, &element);
	EXPECT_EQ(element.state.is_clicked, true);
	EXPECT_EQ(child.state.is_clicked, false);
}

TEST(UIInteractionTests, BoxElementWithChild_ClickOutside_ThenHover_NotActive) {
	ResourceManager resources;
	ui::Element element = box_element_with_child();
	const ui::Element& child = element.box()->children[0];

	/* Click outside box */
	ui::layout_element(resources, SCREEN_SIZE, &element);
	EXPECT_EQ(element.state.is_active, false);
	EXPECT_EQ(child.state.is_active, false);

	/* Drag mouse to top left while button down */
	ui::update_element(ui::Input { .mouse_pos = top_left, .left_mouse_button = ui::KeyState::Down }, &element);
	EXPECT_EQ(element.state.is_active, false);
	EXPECT_EQ(child.state.is_active, false);
}
