#include <gtest/gtest.h>

#include "game/ui.h"

#include "game/resource.h"

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
	ResourceManager resources;
	Vector2 window_size = {};
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
	ResourceManager resources;
	Vector2 window_size = {};

	ui.frame_begin();
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
			ui.frame_end(resources, window_size);
		},
		"UserInterface::box_begin and box_end calls don't match. Missing call to UserInterface::box_end?"
	);
}

TEST(UserInterfaceTests, BoxElement_BoxEnd_WithoutBoxBegin_GivesError) {
	ui::UserInterface ui;
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
			ui.frame_end(resources, window_size);
		},
		"UserInterface::box_begin and box_end calls don't match. Missing call to UserInterface::box_end?"
	);
}

TEST(UserInterfaceTests, BoxElement_TwoBoxesWithText) {
	ui::UserInterface ui;
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
