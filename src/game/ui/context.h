#pragma once

#include "core/util/tracked.h"

#include <string>
#include <unordered_map>

namespace ui {

	struct Element;

	using ElementID = std::string;

	struct State {
		Tracked<bool> is_hovered;
		Tracked<bool> is_active;
		Tracked<bool> is_clicked;
	};

	struct Context {
		std::unordered_map<ElementID, State> element_states; // computed with update_element()
		Tracked<ElementID> focused_element_id;

		void update();

		State* state(const Element& element);
		const State* state(const Element& element) const;

		void focus_element(const Element& element);

		Tracked<bool> is_hovered(const Element& element) const;
		Tracked<bool> is_focused(const Element& element) const;
		Tracked<bool> is_active(const Element& element) const;
		Tracked<bool> is_clicked(const Element& element) const;
	};

} // namespace ui
