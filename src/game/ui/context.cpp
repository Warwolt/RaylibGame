#include "game/ui/context.h"

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

	Tracked<bool> Context::is_hovered(const Element& element) const {
		if (const State* state = this->state(element)) {
			return state->is_hovered;
		}
		return false;
	}

	Tracked<bool> Context::is_focused(const Element& element) const {
		if (const State* state = this->state(element)) {
			return state->is_focused;
		}
		return false;
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