#include <gtest/gtest.h>

#include "core/util.h"

TEST(UtilTests, SplitTextIntoWords_EmptyString) {
	std::vector<std::string> words = util::split_text_into_words("");
	EXPECT_TRUE(words.empty());
}
