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
// +---------+---------+
// |         |         |
// |  child  |         |
// |         |         |
// +---------+         |
// |      parent       |
// |                   |
// +-------------------+
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

ui::Element box_element_with_three_children(ui::Direction direction) {
	return ui::Element {
		.id = "box_parent",
		.style = {
			.width = ui::Pixels(size.x),
			.height = ui::Pixels(size.y),
		},
		.content = ui::Box {
			.direction = direction,
			.children = {
				ui::Element {
				.id = "box_child_1",
					.content = ui::Box {},
				},
				ui::Element {
				.id = "box_child_2",
					.content = ui::Box {},
				},
				ui::Element {
				.id = "box_child_3",
					.content = ui::Box {},
				},
			},
		},
	};
}

#pragma region click

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
	const Input input1 = {
		.mouse_position = top_left,
	};
	ui::update_element(input1, &context, &element);
	EXPECT_EQ(parent_state->is_hovered, true);
	EXPECT_EQ(child_state->is_hovered, true);

	/* Hover middle */
	const Input input2 = {
		.mouse_position = middle,
	};
	ui::update_element(input2, &context, &element);
	EXPECT_EQ(parent_state->is_hovered, true);
	EXPECT_EQ(child_state->is_hovered, true);

	/* Hover bottom right */
	const Input input3 = {
		.mouse_position = bottom_right,
	};
	ui::update_element(input3, &context, &element);
	EXPECT_EQ(parent_state->is_hovered, true);
	EXPECT_EQ(child_state->is_hovered, false);

	/* Hover outside */
	const Input input4 = {
		.mouse_position = outside,
	};
	ui::update_element(input4, &context, &element);
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
	const Input input1 = {
		.mouse_position = top_left,
		.mouse_buttons = { { MOUSE_BUTTON_LEFT, ButtonState::Pressed } },
	};
	ui::update_element(input1, &context, &element);
	EXPECT_EQ(parent_state->is_active, true);
	EXPECT_EQ(child_state->is_active, true);

	/* Hold down button top left */
	const Input input2 = {
		.mouse_position = top_left,
		.mouse_buttons = { { MOUSE_BUTTON_LEFT, ButtonState::Down } },
	};
	ui::update_element(input2, &context, &element);
	EXPECT_EQ(parent_state->is_active, true);
	EXPECT_EQ(child_state->is_active, true);

	/* Release top left */
	const Input input3 = {
		.mouse_position = top_left,
		.mouse_buttons = { { MOUSE_BUTTON_LEFT, ButtonState::Released } },
	};
	ui::update_element(input3, &context, &element);
	EXPECT_EQ(parent_state->is_active, false);
	EXPECT_EQ(child_state->is_active, false);

	/* Click outside box */
	const Input input4 = {
		.mouse_position = outside,
		.mouse_buttons = { { MOUSE_BUTTON_LEFT, ButtonState::Pressed } },
	};
	ui::update_element(input4, &context, &element);
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
	const Input input1 = {
		.mouse_position = top_left,
		.mouse_buttons = { { MOUSE_BUTTON_LEFT, ButtonState::Pressed } },
	};
	ui::update_element(input1, &context, &element);
	EXPECT_EQ(parent_state->is_clicked, false);
	EXPECT_EQ(child_state->is_clicked, false);

	/* Mouse up while hovering child, both child and parent is clicked */
	const Input input2 = {
		.mouse_position = top_left,
		.mouse_buttons = { { MOUSE_BUTTON_LEFT, ButtonState::Released } },
	};
	ui::update_element(input2, &context, &element);
	EXPECT_EQ(parent_state->is_clicked, true);
	EXPECT_EQ(child_state->is_clicked, true);

	/* Mouse down while hovering only parent, not clicked */
	const Input input3 = {
		.mouse_position = bottom_right,
		.mouse_buttons = { { MOUSE_BUTTON_LEFT, ButtonState::Pressed } },
	};
	ui::update_element(input3, &context, &element);
	EXPECT_EQ(parent_state->is_clicked, false);
	EXPECT_EQ(child_state->is_clicked, false);

	/* Mouse up while hovering only parent, only parent is clicked */
	const Input input4 = {
		.mouse_position = bottom_right,
		.mouse_buttons = { { MOUSE_BUTTON_LEFT, ButtonState::Released } },
	};
	ui::update_element(input4, &context, &element);
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
	const Input input = {
		.mouse_position = top_left,
		.mouse_buttons = { { MOUSE_BUTTON_LEFT, ButtonState::Down } },
	};
	ui::update_element(input, &context, &element);
	EXPECT_EQ(parent_state->is_active, false);
	EXPECT_EQ(child_state->is_active, false);
}

#pragma endregion

#pragma region focus

TEST(ElementTests, BoxWithThreeChildren_ChangeFocusWithKeyboard_Horizontal) {
	ResourceManager resources;
	ui::Context context = {
		.focused_element_id = "box_child_1",
	};
	ui::Element parent = box_element_with_three_children(ui::Direction::Horizontal);
	const ui::Element& child1 = parent.box()->children[0];
	const ui::Element& child2 = parent.box()->children[1];
	const ui::Element& child3 = parent.box()->children[2];

	/* Initially, child 1 focused */
	EXPECT_EQ(context.is_focused(parent), false);
	EXPECT_EQ(context.is_focused(child1), true);
	EXPECT_EQ(context.is_focused(child2), false);
	EXPECT_EQ(context.is_focused(child3), false);

	/* Can't focus previously beyond child 1 */
	const Input input1 = {
		.keyboard_keys = { { KEY_LEFT, ButtonState::Pressed } },
	};
	ui::update_element(input1, &context, &parent);
	EXPECT_EQ(context.is_focused(parent), false);
	EXPECT_EQ(context.is_focused(child1), true);
	EXPECT_EQ(context.is_focused(child2), false);
	EXPECT_EQ(context.is_focused(child3), false);

	/* Focus child 2 */
	const Input input2 = {
		.keyboard_keys = { { KEY_RIGHT, ButtonState::Pressed } },
	};
	ui::update_element(input2, &context, &parent);
	EXPECT_EQ(context.is_focused(parent), false);
	EXPECT_EQ(context.is_focused(child1), false);
	EXPECT_EQ(context.is_focused(child2), true);
	EXPECT_EQ(context.is_focused(child3), false);

	/* Focus child 3 */
	const Input input3 = {
		.keyboard_keys = { { KEY_RIGHT, ButtonState::Pressed } },
	};
	ui::update_element(input3, &context, &parent);
	EXPECT_EQ(context.is_focused(parent), false);
	EXPECT_EQ(context.is_focused(child1), false);
	EXPECT_EQ(context.is_focused(child2), false);
	EXPECT_EQ(context.is_focused(child3), true);

	/* Can't focus beyond child 3 */
	const Input input4 = {
		.keyboard_keys = { { KEY_RIGHT, ButtonState::Pressed } },
	};
	ui::update_element(input4, &context, &parent);
	EXPECT_EQ(context.is_focused(parent), false);
	EXPECT_EQ(context.is_focused(child1), false);
	EXPECT_EQ(context.is_focused(child2), false);
	EXPECT_EQ(context.is_focused(child3), true);
}

TEST(ElementTests, BoxWithThreeChildren_ChangeFocusWithKeyboard_Vertical) {
	ResourceManager resources;
	ui::Context context = {
		.focused_element_id = "box_child_1",
	};
	ui::Element parent = box_element_with_three_children(ui::Direction::Vertical);
	const ui::Element& child1 = parent.box()->children[0];
	const ui::Element& child2 = parent.box()->children[1];
	const ui::Element& child3 = parent.box()->children[2];

	/* Initially, child 1 focused */
	EXPECT_EQ(context.is_focused(parent), false);
	EXPECT_EQ(context.is_focused(child1), true);
	EXPECT_EQ(context.is_focused(child2), false);
	EXPECT_EQ(context.is_focused(child3), false);

	/* Can't focus previously beyond child 1 */
	const Input input1 = {
		.keyboard_keys = { { KEY_UP, ButtonState::Pressed } },
	};
	ui::update_element(input1, &context, &parent);
	EXPECT_EQ(context.is_focused(parent), false);
	EXPECT_EQ(context.is_focused(child1), true);
	EXPECT_EQ(context.is_focused(child2), false);
	EXPECT_EQ(context.is_focused(child3), false);

	/* Focus child 2 */
	const Input input2 = {
		.keyboard_keys = { { KEY_DOWN, ButtonState::Pressed } },
	};
	ui::update_element(input2, &context, &parent);
	EXPECT_EQ(context.is_focused(parent), false);
	EXPECT_EQ(context.is_focused(child1), false);
	EXPECT_EQ(context.is_focused(child2), true);
	EXPECT_EQ(context.is_focused(child3), false);

	/* Focus child 3 */
	const Input input3 = {
		.keyboard_keys = { { KEY_DOWN, ButtonState::Pressed } },
	};
	ui::update_element(input3, &context, &parent);
	EXPECT_EQ(context.is_focused(parent), false);
	EXPECT_EQ(context.is_focused(child1), false);
	EXPECT_EQ(context.is_focused(child2), false);
	EXPECT_EQ(context.is_focused(child3), true);

	/* Can't focus beyond child 3 */
	const Input input4 = {
		.keyboard_keys = { { KEY_DOWN, ButtonState::Pressed } },
	};
	ui::update_element(input4, &context, &parent);
	EXPECT_EQ(context.is_focused(parent), false);
	EXPECT_EQ(context.is_focused(child1), false);
	EXPECT_EQ(context.is_focused(child2), false);
	EXPECT_EQ(context.is_focused(child3), true);
}

#pragma endregion
