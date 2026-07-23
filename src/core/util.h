#pragma once

#include <functional>
#include <string_view>
#include <vector>

namespace util {

	std::vector<std::string_view> get_string_view_per_word(std::string_view text);

	template <typename Container, typename Pred>
	bool any_of(const Container& items, Pred predicate) {
		return std::find_if(items.begin(), items.end(), predicate) != items.end();
	}

} // namespace util
