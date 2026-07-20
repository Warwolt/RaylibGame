#include "game/ui/user_interface.h"

#include "core/debug/assert.h"
#include "core/debug/profiling.h"
#include "core/util.h"

#include <algorithm>
#include <array>
#include <numeric>

namespace ui {

	void UserInterface::draw(const ResourceManager& resources) const {
		draw_element(resources, m_root_element);
	}

	const Element& UserInterface::root_element() const {
		return m_root_element;
	}

	void UserInterface::frame_begin() {
		ASSERT(!m_within_frame, "Missing call to UserInterface::frame_end?");
		m_within_frame = true;
		m_root_element = {};
		m_parent_stack = { &m_root_element };
	}

	void UserInterface::frame_end(const ResourceManager& resources, Vector2 window_size) {
		PROFILING_SCOPE();
		ASSERT(m_within_frame, "Missing call to UserInterface::frame_begin?");
		ASSERT(m_parent_stack.size() == 1, "UserInterface::box_begin and box_end calls don't match. Missing call to UserInterface::box_end?");
		m_within_frame = false;
		layout_element(resources, window_size, &m_root_element);
	}

	void UserInterface::box_begin(Direction direction, std::optional<Style> style, std::string debug_name) {
		Element* parent = _current_parent();
		ASSERT(m_within_frame, "Missing call to UserInterface::frame_begin?");
		parent->box()->children.push_back(
			Element {
				.debug_name = debug_name,
				.style = style.value_or(Style {}),
				.content = Box { .direction = direction },
			}
		);
		m_parent_stack.push_back(&parent->box()->children.back());
	}

	void UserInterface::box_end() {
		ASSERT(m_within_frame, "Missing call to UserInterface::frame_begin?");
		ASSERT(m_parent_stack.size() > 1, "UserInterface::box_begin and box_end calls don't match. Missing call to UserInterface::box_end?");
		m_parent_stack.pop_back();
	}

	void UserInterface::text(std::string_view text, std::optional<Style> style, std::string debug_name) {
		Element* parent = _current_parent();
		ASSERT(m_within_frame, "Missing call to UserInterface::frame_begin?");
		parent->box()->children.push_back(
			Element {
				.debug_name = debug_name,
				.style = style.value_or(Style {}),
				.content = Text { .text = std::string(text) },
			}
		);
	}

	void UserInterface::image(ImageID image, std::optional<Style> style, std::string debug_name) {
		Element* parent = _current_parent();
		ASSERT(m_within_frame, "Missing call to UserInterface::frame_begin?");
		parent->box()->children.push_back(
			Element {
				.debug_name = debug_name,
				.style = style.value_or(Style {}),
				.content = Image { .id = image },
			}
		);
	}

	Element* UserInterface::_current_parent() {
		ASSERT(!m_parent_stack.empty(), "Forgot to add root element to parent stack?");
		return m_parent_stack.back();
	}
}
