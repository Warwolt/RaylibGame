#include <gtest/gtest.h>

#include "game/ui.h"

#include "game/resource.h"

ui::ElementTree enumerated_tree() {
	ui::ElementTree tree;

	tree.push_element(ui::Element { .debug_name = "First" });
	{
		tree.push_element(ui::Element { .debug_name = "Second" });
		tree.push_element(ui::Element { .debug_name = "Third" });
	}
	tree.close_element();
	tree.push_element(ui::Element { .debug_name = "Fourth" });

	return tree;
}

TEST(UIElementTreeTests, RangeBasedLoop_IteratesInPreOrder) {
	ui::ElementTree tree = enumerated_tree();

	std::vector<std::string> debug_names;
	for (const ui::Element& element : tree) {
		debug_names.push_back(element.debug_name);
	}

	const std::vector<std::string> expected = { "root", "First", "Second", "Third", "Fourth" };
	EXPECT_EQ(debug_names, expected);
}

TEST(UIElementTreeTests, ForLoop_IteratesInPreOrder) {
	ui::ElementTree tree = enumerated_tree();

	std::vector<std::string> debug_names;
	for (auto it = tree.begin(); it != tree.end(); ++it) {
		const ui::Element& element = *it;
		debug_names.push_back(element.debug_name);
	}

	const std::vector<std::string> expected = { "root", "First", "Second", "Third", "Fourth" };
	EXPECT_EQ(debug_names, expected);
}
