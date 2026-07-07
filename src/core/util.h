#pragma once

#include "core/debug/profiling.h"

#include <iostream>
#include <ranges>
#include <string>
#include <string_view>

namespace util {

	auto split_text_into_words(std::string_view text) {
		PROFILING_SCOPE();
		// clang-format off
		return  text
			| std::views::split(' ')
			| std::views::transform([](auto&& r) {
					return std::string_view(&*r.begin(), std::ranges::distance(r));
				});
		// clang-format on
	}

} // namespace util
