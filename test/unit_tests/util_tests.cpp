#include <gtest/gtest.h>

#include "core/util.h"

TEST(UtilTests, SplitTextIntoWords_EmptyString) {
	std::vector<std::string> words = util::split_text_into_words("");
	EXPECT_TRUE(words.empty());
}

TEST(UtilTests, SplitTextIntoWords_Hello_World) {
	std::vector<std::string> words = util::split_text_into_words("Hello, World!");
	std::vector<std::string> expected = { "Hello,", "World!" };
	EXPECT_EQ(words, expected);
}
