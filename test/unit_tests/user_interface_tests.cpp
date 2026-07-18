#include <gtest/gtest.h>

#include "game/ui.h"

#include "game/resource.h"

#pragma region layouting

TEST(UserInterfaceTests, DefaultConstructed_RootIsEmptyBox) {
	ui::UserInterface ui;
	const ui::Element& root_element = ui.root_element();
	ASSERT_TRUE(root_element.is_box());
	EXPECT_TRUE(root_element.box()->children.empty());
}

TEST(UserInterfaceTests, FrameBegin_FrameBegin_GivesError) {
	ui::UserInterface ui;
	const ui::Input input = {};
	EXPECT_DEATH(
		{
			ui.frame_begin(input);
			ui.frame_begin(input);
		},
		"Missing call to UserInterface::frame_end?"
	);
}

TEST(UserInterfaceTests, FrameBegin_TreeIsCleared) {
	ui::UserInterface ui;
	const ResourceManager resources;
	const Vector2 window_size = {};
	const ui::Input input = {};

	ui.frame_begin(input);
	{
		ui.text("Hello world");
	}
	ui.frame_end(resources, window_size);
	ui.frame_begin(input);

	EXPECT_EQ(ui.root_element(), ui::Element { .debug_name = "root" });
}

TEST(UserInterfaceTests, FrameEnd_WithoutFrameBegin_GivesError) {
	ui::UserInterface ui;
	const ResourceManager resources;
	const Vector2 window_size = {};

	EXPECT_DEATH(
		{
			// ui.frame_begin();
			ui.frame_end(resources, window_size);
		},
		"Missing call to UserInterface::frame_begin?"
	);
}

TEST(UserInterfaceTests, TextElement) {
	ui::UserInterface ui;
	const ResourceManager resources;
	const Vector2 window_size = {};
	const ui::Input input = {};

	ui.frame_begin(input);
	{
		ui.text("Hello world");
	}
	ui.frame_end(resources, window_size);

	const ui::Element& root = ui.root_element();
	ASSERT_TRUE(root.is_box());
	ASSERT_EQ(root.box()->children.size(), 1);
	ASSERT_TRUE(root.box()->children[0].is_text());
	EXPECT_EQ(root.box()->children[0].text()->text, "Hello world");
}

TEST(UserInterfaceTests, BoxElement_BoxBegin_WithoutBoxEnd_GivesError) {
	ui::UserInterface ui;
	const ResourceManager resources;
	const Vector2 window_size = {};
	const ui::Input input = {};

	EXPECT_DEATH(
		{
			ui.frame_begin(input);
			{
				ui.box_begin();
				{
					ui.box_begin();
					// ui.box_end();
				}
				ui.box_end();
			}
			ui.frame_end(resources, window_size);
		},
		"UserInterface::box_begin and box_end calls don't match. Missing call to UserInterface::box_end?"
	);
}

TEST(UserInterfaceTests, BoxElement_BoxEnd_WithoutBoxBegin_GivesError) {
	ui::UserInterface ui;
	const ResourceManager resources;
	const Vector2 window_size = {};
	const ui::Input input = {};

	EXPECT_DEATH(
		{
			ui.frame_begin(input);
			{
				ui.box_begin();
				{
					// ui.box_begin();
					ui.box_end();
				}
				ui.box_end();
			}
			ui.frame_end(resources, window_size);
		},
		"UserInterface::box_begin and box_end calls don't match. Missing call to UserInterface::box_end?"
	);
}

TEST(UserInterfaceTests, BoxElement_TwoBoxesWithText) {
	ui::UserInterface ui;
	const ResourceManager resources;
	const Vector2 window_size = {};
	const ui::Input input = {};

	ui.frame_begin(input);
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
	ui.frame_end(resources, window_size);

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

#pragma endregion
#pragma region interaction

TEST(UserInterfaceTests, BoxElement_Hovered) {
	ui::UserInterface ui;
	const ResourceManager resources;
	const Vector2 window_size = { 1000, 1000 };

	const ui::Input input = {
		.mouse_pos = { 150, 150 }, // middle of the box
	};
	bool is_hovered = false;
	for (int i = 0; i < 2; i++) {
		ui.frame_begin(input);
		{
			ui.box_begin(
				ui::Direction::Horizontal,
				ui::Style {
					.position = ui::AbsolutePosition(ui::Pixels(100), ui::Pixels(100)),
					.width = ui::Pixels(100),
					.height = ui::Pixels(100),
				}
			);
			if (ui.element_is_hovered()) {
				is_hovered = true;
			}
			ui.box_end();
		}
		ui.frame_end(resources, window_size);
	}

	EXPECT_TRUE(is_hovered);
}

TEST(USerInterfaceTests, Bug) {
	FAIL() << "Write a test where re-arranging elements in a list still keeps track of element state";
}
