#include "game/ui/context.h"

#include "core/debug/assert.h"
#include "game/ui/element.h"

namespace ui {

	State* Context::state(const Element& element) {
		if (element.id.empty()) {
			return nullptr;
		}
		return &this->element_states[element.id];
	}

	const State* Context::state(const Element& element) const {
		auto it = this->element_states.find(element.id);
		if (it == this->element_states.end()) {
			return nullptr;
		}
		return &it->second;
	}

	void Context::focus_element(const Element& element) {
		ASSERT(!element.id.empty(), "Cannot focus an element without id!");
		this->focused_element_id = element.id;
	}

	Tracked<bool> Context::is_hovered(const Element& element) const {
		if (const State* state = this->state(element)) {
			return state->is_hovered;
		}
		return false;
	}

	Tracked<bool> Context::is_focused(const Element& element) const {
		/* Try inherit focused state from parent */
		if (element.parent != nullptr) {
			Tracked<bool> parent_is_focused = is_focused(*element.parent);
			if (parent_is_focused) {
				return parent_is_focused;
			}
		}

		/* Check if given element is focused */
		if (element.id.empty()) {
			return false;
		}
		return { focused_element_id.value() == element.id, focused_element_id.previous() == element.id };
	}

	Tracked<bool> Context::is_active(const Element& element) const {
		if (const State* state = this->state(element)) {
			return state->is_active;
		}
		return false;
	}

	Tracked<bool> Context::is_clicked(const Element& element) const {
		if (const State* state = this->state(element)) {
			return state->is_clicked;
		}
		return false;
	}

} // namespace ui
