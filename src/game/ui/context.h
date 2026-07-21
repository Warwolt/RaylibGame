#pragma once

#include "core/util/tracked.h"

#include <string>
#include <unordered_map>

namespace ui {

	struct Element;

	struct State {
		Tracked<bool> is_hovered;
		Tracked<bool> is_active;
		Tracked<bool> is_clicked;
	};

	struct Context {
		std::unordered_map<std::string, State> element_states; // computed with update_element()

		State* state(const Element& element);
		const State* state(const Element& element) const;
		Tracked<bool> is_active(const Element& element) const;
		Tracked<bool> is_hovered(const Element& element) const;
		Tracked<bool> is_clicked(const Element& element) const;
	};

} // namespace ui
