#include <gtest/gtest.h>

#include "game/ui/tree.h"

#include "game/resource.h"

ui::Tree enumerated_tree() {
	ui::Tree tree;

	tree.push_element(ui::Element { .debug_name = "First" });
	{
		tree.push_element(ui::Element { .debug_name = "Second" });
		tree.push_element(ui::Element { .debug_name = "Third" });
	}
	tree.close_element();
	tree.push_element(ui::Element { .debug_name = "Fourth" });

	return tree;
}

TEST(UITreeTests, RangeBasedLoop_IteratesInPreOrder) {
	ui::Tree tree = enumerated_tree();

	std::vector<std::string> debug_names;
	for (const ui::Element& element : tree) {
		debug_names.push_back(element.debug_name);
	}

	const std::vector<std::string> expected = { "root", "First", "Second", "Third", "Fourth" };
	EXPECT_EQ(debug_names, expected);
}

TEST(UITreeTests, ForLoop_IteratesInPreOrder) {
	ui::Tree tree = enumerated_tree();

	std::vector<std::string> debug_names;
	for (auto it = tree.begin(); it != tree.end(); ++it) {
		const ui::Element& element = *it;
		debug_names.push_back(element.debug_name);
	}

	const std::vector<std::string> expected = { "root", "First", "Second", "Third", "Fourth" };
	EXPECT_EQ(debug_names, expected);
}
