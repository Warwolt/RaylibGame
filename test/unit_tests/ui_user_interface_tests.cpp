#include <gtest/gtest.h>

#include "game/ui/user_interface.h"

#include "game/resource.h"

#pragma region layout

TEST(UserInterfaceTests, DefaultConstructed_RootIsEmptyBox) {
	ui::UserInterface ui;
	const ui::Element& root_element = ui.root_element();
	ASSERT_TRUE(root_element.is_box());
	EXPECT_TRUE(root_element.box()->children.empty());
}

TEST(UserInterfaceTests, FrameBegin_FrameBegin_GivesError) {
	ui::UserInterface ui;
	EXPECT_DEATH(
		{
			ui.frame_begin();
			ui.frame_begin();
		},
		"Missing call to UserInterface::frame_end?"
	);
}

TEST(UserInterfaceTests, FrameEnd_WithoutFrameBegin_GivesError) {
	ui::UserInterface ui;
	Input input = {};
	ResourceManager resources;
	Vector2 window_size = {};
	EXPECT_DEATH(
		{
			// ui.frame_begin();
			ui.frame_end(input, resources, window_size);
		},
		"Missing call to UserInterface::frame_begin?"
	);
}

TEST(UserInterfaceTests, TextElement) {
	ui::UserInterface ui;
	Input input = {};
	ResourceManager resources;
	Vector2 window_size = {};

	ui.frame_begin();
	{
		ui.text("Hello world");
	}
	ui.frame_end(input, resources, window_size);

	const ui::Element& root = ui.root_element();
	ASSERT_TRUE(root.is_box());
	ASSERT_EQ(root.box()->children.size(), 1);
	ASSERT_TRUE(root.box()->children[0].is_text());
	EXPECT_EQ(root.box()->children[0].text()->text, "Hello world");
}

TEST(UserInterfaceTests, BoxElement_BoxBegin_WithoutBoxEnd_GivesError) {
	ui::UserInterface ui;
	Input input = {};
	ResourceManager resources;
	Vector2 window_size = {};
	EXPECT_DEATH(
		{
			ui.frame_begin();
			{
				ui.box_begin();
				{
					ui.box_begin();
					// ui.box_end();
				}
				ui.box_end();
			}
			ui.frame_end(input, resources, window_size);
		},
		"UserInterface::box_begin and box_end calls don't match. Missing call to UserInterface::box_end?"
	);
}

TEST(UserInterfaceTests, BoxElement_BoxEnd_WithoutBoxBegin_GivesError) {
	ui::UserInterface ui;
	Input input = {};
	ResourceManager resources;
	Vector2 window_size = {};
	EXPECT_DEATH(
		{
			ui.frame_begin();
			{
				ui.box_begin();
				{
					// ui.box_begin();
					ui.box_end();
				}
				ui.box_end();
			}
			ui.frame_end(input, resources, window_size);
		},
		"UserInterface::box_begin and box_end calls don't match. Missing call to UserInterface::box_end?"
	);
}

TEST(UserInterfaceTests, BoxElement_TwoBoxesWithText) {
	ui::UserInterface ui;
	Input input = {};
	ResourceManager resources;
	Vector2 window_size = {};

	ui.frame_begin();
	{
		ui.box_begin();
		{
			ui.text("Hello");
		}
		ui.box_end();

		ui.box_begin();
		{
			ui.text("World");
		}
		ui.box_end();
	}
	ui.frame_end(input, resources, window_size);

	const ui::Element& root = ui.root_element();
	// root
	ASSERT_TRUE(root.is_box());
	ASSERT_EQ(root.box()->children.size(), 2);

	// first box
	ASSERT_TRUE(root.box()->children[0].is_box());
	ASSERT_EQ(root.box()->children[0].box()->children.size(), 1);
	ASSERT_TRUE(root.box()->children[0].box()->children[0].is_text());
	ASSERT_EQ(root.box()->children[0].box()->children[0].text()->text, "Hello");

	// second box
	ASSERT_TRUE(root.box()->children[1].is_box());
	ASSERT_EQ(root.box()->children[1].box()->children.size(), 1);
	ASSERT_TRUE(root.box()->children[1].box()->children[0].is_text());
	ASSERT_EQ(root.box()->children[1].box()->children[0].text()->text, "World");
}

#pragma endregion layouting

#pragma region interaction

TEST(UserInterfaceTests, BoxElement_IsHovered_WithoutId_GivesError) {
	ui::UserInterface ui;
	const ResourceManager resources;
	const Vector2 window_size = {};
	const Input input = {};
	EXPECT_DEATH(
		{
			ui.frame_begin();
			{
				ui.box_begin(); // id argument omitted
				{
					ui.element_is_hovered();
				}
				ui.box_end();
			}
			ui.frame_end(input, resources, window_size);
		},
		"element_is_hovered called when current element lacks id!"
	);
}

TEST(UserInterfaceTests, BoxElement_IsActive_WithoutId_GivesError) {
	ui::UserInterface ui;
	const ResourceManager resources;
	const Vector2 window_size = {};
	const Input input = {};
	EXPECT_DEATH(
		{
			ui.frame_begin();
			{
				ui.box_begin(); // id argument omitted
				{
					ui.element_is_active();
				}
				ui.box_end();
			}
			ui.frame_end(input, resources, window_size);
		},
		"element_is_active called when current element lacks id!"
	);
}

TEST(UserInterfaceTests, BoxElement_IsClicked_WithoutId_GivesError) {
	ui::UserInterface ui;
	const ResourceManager resources;
	const Vector2 window_size = {};
	const Input input = {};
	EXPECT_DEATH(
		{
			ui.frame_begin();
			{
				ui.box_begin(); // id argument omitted
				{
					ui.element_is_clicked();
				}
				ui.box_end();
			}
			ui.frame_end(input, resources, window_size);
		},
		"element_is_clicked called when current element lacks id!"
	);
}
TEST(UserInterfaceTests, BoxElement_IsHovered) {
	ui::UserInterface ui;
	const ResourceManager resources;
	const Vector2 window_size = { 1000, 1000 };

	const Input input = {
		.mouse_position = { 150, 150 }, // middle of the box
	};
	bool is_hovered = false;
	for (int i = 0; i < 2; i++) {
		ui.frame_begin();
		{
			const ui::Style box_style = {
				.position = ui::AbsolutePosition(ui::Pixels(100), ui::Pixels(100)),
				.width = ui::Pixels(100),
				.height = ui::Pixels(100),
			};
			ui.box_begin(ui::Direction::Horizontal, box_style, "box");
			{
				if (ui.element_is_hovered()) {
					is_hovered = true;
				}
			}
			ui.box_end();
		}
		ui.frame_end(input, resources, window_size);
	}

	EXPECT_TRUE(is_hovered);
}

TEST(UserInterfaceTests, BoxElement_IsActive) {
	ui::UserInterface ui;
	const ResourceManager resources;
	const Vector2 window_size = { 1000, 1000 };

	const Input inputs[2] = {
		{
			.mouse_position = { 150, 150 }, // middle of the box
			.mouse_buttons = { { MOUSE_BUTTON_LEFT, ButtonState::Pressed }, },
		},
		{
			.mouse_position = { 150, 150 },
			.mouse_buttons = { { MOUSE_BUTTON_LEFT, ButtonState::Down }, },
		},
	};
	bool is_active = false;
	for (int i = 0; i < 2; i++) {
		const Input input = inputs[i];
		ui.frame_begin();
		{
			const ui::Style box_style = {
				.position = ui::AbsolutePosition(ui::Pixels(100), ui::Pixels(100)),
				.width = ui::Pixels(100),
				.height = ui::Pixels(100),
			};
			ui.box_begin(ui::Direction::Horizontal, box_style, "box");
			{
				if (ui.element_is_active()) {
					is_active = true;
				}
			}
			ui.box_end();
		}
		ui.frame_end(input, resources, window_size);
	}

	EXPECT_TRUE(is_active);
}

TEST(UserInterfaceTests, BoxElement_IsClicked) {
	ui::UserInterface ui;
	const ResourceManager resources;
	const Vector2 window_size = { 1000, 1000 };

	const Input inputs[3] = {
		{
			.mouse_position = { 150, 150 }, // middle of the box
			.mouse_buttons = { { MOUSE_BUTTON_LEFT, ButtonState::Pressed }, },
		},
		{
			.mouse_position = { 150, 150 },
			.mouse_buttons = { { MOUSE_BUTTON_LEFT, ButtonState::Released }, },
		},
		{
			.mouse_position = { 150, 150 },
			.mouse_buttons = { { MOUSE_BUTTON_LEFT, ButtonState::Up }, },
		},
	};
	bool is_clicked = false;
	for (int i = 0; i < 3; i++) {
		const Input input = inputs[i];
		ui.frame_begin();
		{
			const ui::Style box_style = {
				.position = ui::AbsolutePosition(ui::Pixels(100), ui::Pixels(100)),
				.width = ui::Pixels(100),
				.height = ui::Pixels(100),
			};
			ui.box_begin(ui::Direction::Horizontal, box_style, "box");
			{
				if (ui.element_is_clicked()) {
					is_clicked = true;
				}
			}
			ui.box_end();
		}
		ui.frame_end(input, resources, window_size);
	}

	EXPECT_TRUE(is_clicked);
}
#pragma endregion
