#pragma once

#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

TEST(NlohmannJsonTests, ParseObjectLiteral) {
	const char* object_string = R"(
  {
    "pi": 3.141,
    "happy": true
  }
)";

	const nlohmann::json json_object = nlohmann::json::parse(object_string);

	ASSERT_TRUE(json_object["pi"].is_number());
	ASSERT_TRUE(json_object["happy"].is_boolean());
	EXPECT_EQ(json_object["pi"].get<double>(), 3.141);
	EXPECT_EQ(json_object["happy"].get<bool>(), true);
}

TEST(NlohmannJsonTests, SerializeObject) {
	const nlohmann::ordered_json json_object = {
		{ "pi", 3.141 },
		{ "happy", true },
	};

	const std::string object_string = json_object.dump();

	EXPECT_EQ(object_string, R"({"pi":3.141,"happy":true})");
}
