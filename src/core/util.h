#pragma once

#include "core/debug/profiling.h"

#include <string>
#include <string_view>
#include <vector>

namespace util {

	inline std::vector<std::string_view> get_string_view_per_word(std::string_view text) {
		std::vector<std::string_view> words;
		size_t start = 0;
		while (start < text.size()) {
			// skip spaces
			while (start < text.size() && text[start] == ' ') {
				start++;
			}
			if (start >= text.size())
				break;
			// find end of word
			size_t end = text.find(' ', start);
			if (end == std::string_view::npos) {
				end = text.size();
			}
			words.push_back(text.substr(start, end - start));
			start = end;
		}
		return words;
	}

} // namespace util
