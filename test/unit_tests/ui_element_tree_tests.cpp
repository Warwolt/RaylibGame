#include <gtest/gtest.h>

#include "game/ui.h"

#include "game/resource.h"

TEST(UIElementTreeTests, ForEach_PreOrder) {
	ui::ElementTree tree;

	tree.push_element(ui::Element { .debug_name = "First" });
	{
		tree.push_element(ui::Element { .debug_name = "Second" });
		tree.push_element(ui::Element { .debug_name = "Third" });
	}
	tree.close_element();
	tree.push_element(ui::Element { .debug_name = "Fourth" });

	std::vector<std::string> debug_names;
	for (const ui::Element& element : tree) {
		debug_names.push_back(element.debug_name);
	}

	const std::vector<std::string> expected = { "root", "First", "Second", "Third", "Fourth" };
	EXPECT_EQ(debug_names, expected);
}
