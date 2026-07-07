#pragma once

#include <string_view>
#include <vector>

namespace util {

	std::vector<std::string_view> get_string_view_per_word(std::string_view text);

} // namespace util
