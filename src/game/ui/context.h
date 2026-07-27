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

		State* state(const Element& element);
		const State* state(const Element& element) const;

		void focus_element(const Element& element);

		// FIXME: these should apply to children as well!
		//
		// It'll likely be more composable if these states apply to not only a
		// given element but all its descendants, so that we e.g. can style a
		// text element sitting inside a focused box element.
		//
		// To do that we need access to the entire chain of parents for the
		// given element. How should we pass that to these functions?
		// `std::vector<Element*> ancestors` ?
		//
		Tracked<bool> is_hovered(const Element& element) const;
		Tracked<bool> is_focused(const Element& element) const;
		Tracked<bool> is_active(const Element& element) const;
		Tracked<bool> is_clicked(const Element& element) const;
	};

} // namespace ui
