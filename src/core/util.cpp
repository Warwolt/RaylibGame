#include "core/util.h"

#include <sstream>

namespace util {

	std::vector<std::string> split_text_into_words(const std::string& text) {
		std::vector<std::string> words;

		std::istringstream iss(text);
		std::string word;
		while (iss >> word) {
			words.push_back(word);
		}

		return words;
	}

} // namespace util
