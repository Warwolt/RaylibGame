#include "game/ui/element.h"

#include "game/resource.h"

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
		.id = "box_parent",
		.style = {
			.width = ui::Pixels(size.x),
			.height = ui::Pixels(size.y),
		},
		.content = ui::Box {
			.children = {
				ui::Element {
				.id = "box_child",
					.style = {
						.width = ui::Pixels(size.x / 2),
						.height = ui::Pixels(size.y / 2),
					},
					.content = ui::Box {},
				},
			},
		},
	};
}

TEST(ElementTests, BoxElementWithChild_IsHovered) {
	ResourceManager resources;
	ui::Context context = {};
	ui::Element element = box_element_with_child();
	const ui::Element& child = element.box()->children[0];
	ui::State* parent_state = context.state(element);
	ui::State* child_state = context.state(child);

	/* Element initially not hovered */
	ui::layout_element(resources, SCREEN_SIZE, &element);
	EXPECT_EQ(parent_state->is_hovered, false);
	EXPECT_EQ(child_state->is_hovered, false);

	/* Hover top left */
	ui::update_element(Input { .mouse_position = top_left }, &context, &element);
	EXPECT_EQ(parent_state->is_hovered, true);
	EXPECT_EQ(child_state->is_hovered, true);

	/* Hover middle */
	ui::update_element(Input { .mouse_position = middle }, &context, &element);
	EXPECT_EQ(parent_state->is_hovered, true);
	EXPECT_EQ(child_state->is_hovered, true);

	/* Hover bottom right */
	ui::update_element(Input { .mouse_position = bottom_right }, &context, &element);
	EXPECT_EQ(parent_state->is_hovered, true);
	EXPECT_EQ(child_state->is_hovered, false);

	/* Hover outside */
	ui::update_element(Input { .mouse_position = outside }, &context, &element);
	EXPECT_EQ(parent_state->is_hovered, false);
	EXPECT_EQ(child_state->is_hovered, false);
}

TEST(ElementTests, BoxElementWithChild_IsActive) {
	ResourceManager resources;
	ui::Context context = {};
	ui::Element element = box_element_with_child();
	const ui::Element& child = element.box()->children[0];
	ui::State* parent_state = context.state(element);
	ui::State* child_state = context.state(child);

	/* Element initially not active */
	ui::layout_element(resources, SCREEN_SIZE, &element);
	EXPECT_EQ(parent_state->is_active, false);
	EXPECT_EQ(child_state->is_active, false);

	/* Click top left */
	ui::update_element(Input { .mouse_position = top_left, .left_mouse_button = ButtonState::Pressed }, &context, &element);
	EXPECT_EQ(parent_state->is_active, true);
	EXPECT_EQ(child_state->is_active, true);

	/* Hold down button top left */
	ui::update_element(Input { .mouse_position = top_left, .left_mouse_button = ButtonState::Down }, &context, &element);
	EXPECT_EQ(parent_state->is_active, true);
	EXPECT_EQ(child_state->is_active, true);

	/* Release top left */
	ui::update_element(Input { .mouse_position = top_left, .left_mouse_button = ButtonState::Released }, &context, &element);
	EXPECT_EQ(parent_state->is_active, false);
	EXPECT_EQ(child_state->is_active, false);

	/* Click outside box */
	ui::update_element(Input { .mouse_position = outside, .left_mouse_button = ButtonState::Pressed }, &context, &element);
	EXPECT_EQ(parent_state->is_active, false);
	EXPECT_EQ(child_state->is_active, false);
}

TEST(ElementTests, BoxElementWithChild_IsClicked) {
	ResourceManager resources;
	ui::Context context = {};
	ui::Element element = box_element_with_child();
	const ui::Element& child = element.box()->children[0];
	ui::State* parent_state = context.state(element);
	ui::State* child_state = context.state(child);

	/* Element initially not clicked */
	ui::layout_element(resources, SCREEN_SIZE, &element);
	EXPECT_EQ(parent_state->is_clicked, false);
	EXPECT_EQ(child_state->is_clicked, false);

	/* Mouse down while hovering, not clicked */
	ui::update_element(Input { .mouse_position = top_left, .left_mouse_button = ButtonState::Pressed }, &context, &element);
	EXPECT_EQ(parent_state->is_clicked, false);
	EXPECT_EQ(child_state->is_clicked, false);

	/* Mouse up while hovering child, both child and parent is clicked */
	ui::update_element(Input { .mouse_position = top_left, .left_mouse_button = ButtonState::Released }, &context, &element);
	EXPECT_EQ(parent_state->is_clicked, true);
	EXPECT_EQ(child_state->is_clicked, true);

	/* Mouse down while hovering only parent, not clicked */
	ui::update_element(Input { .mouse_position = bottom_right, .left_mouse_button = ButtonState::Pressed }, &context, &element);
	EXPECT_EQ(parent_state->is_clicked, false);
	EXPECT_EQ(child_state->is_clicked, false);

	/* Mouse up while hovering only parent, only parent is clicked */
	ui::update_element(Input { .mouse_position = bottom_right, .left_mouse_button = ButtonState::Released }, &context, &element);
	EXPECT_EQ(parent_state->is_clicked, true);
	EXPECT_EQ(child_state->is_clicked, false);
}

TEST(ElementTests, BoxElementWithChild_ClickOutside_ThenHover_NotActive) {
	ResourceManager resources;
	ui::Context context = {};
	ui::Element element = box_element_with_child();
	const ui::Element& child = element.box()->children[0];
	ui::State* parent_state = context.state(element);
	ui::State* child_state = context.state(child);

	/* Click outside box */
	ui::layout_element(resources, SCREEN_SIZE, &element);
	EXPECT_EQ(parent_state->is_active, false);
	EXPECT_EQ(child_state->is_active, false);

	/* Drag mouse to top left while button down */
	ui::update_element(Input { .mouse_position = top_left, .left_mouse_button = ButtonState::Down }, &context, &element);
	EXPECT_EQ(parent_state->is_active, false);
	EXPECT_EQ(child_state->is_active, false);
}
