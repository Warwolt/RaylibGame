#include "game/ui/user_interface.h"

#include "core/debug/assert.h"
#include "core/debug/profiling.h"

namespace ui {

	void UserInterface::draw(const ResourceManager& resources) const {
		draw_element(resources, m_context, m_tree.root());
	}

	const Element& UserInterface::root_element() const {
		return m_tree.root();
	}

	void UserInterface::frame_begin() {
		ASSERT(!m_is_within_frame, "Missing call to UserInterface::frame_end?");
		m_is_within_frame = true;
		m_tree.reset();
	}

	void UserInterface::frame_end(const Input& input, const ResourceManager& resources, Vector2 window_size) {
		PROFILING_SCOPE();
		ASSERT(m_is_within_frame, "Missing call to UserInterface::frame_begin?");
		ASSERT(!m_tree.has_open_element(), "UserInterface::box_begin and box_end calls don't match. Missing call to UserInterface::box_end?");
		m_is_within_frame = false;
		layout_element(resources, window_size, &m_tree.root());
		update_element(input, &m_context, &m_tree.root());
	}

	void UserInterface::box_begin(Direction direction, std::optional<Style> style, std::string id) {
		ASSERT(m_is_within_frame, "Missing call to UserInterface::frame_begin?");
		m_tree.push_element(
			Element {
				.id = id,
				.style = style.value_or(Style {}),
				.content = Box { .direction = direction },
			}
		);
	}

	void UserInterface::box_end() {
		ASSERT(m_is_within_frame, "Missing call to UserInterface::frame_begin?");
		ASSERT(m_tree.has_open_element(), "UserInterface::box_begin and box_end calls don't match. Missing call to UserInterface::box_end?");
		m_tree.close_element();
	}

	void UserInterface::text(std::string_view text, std::optional<Style> style, std::string id) {
		ASSERT(m_is_within_frame, "Missing call to UserInterface::frame_begin?");
		m_tree.push_element(
			Element {
				.id = id,
				.style = style.value_or(Style {}),
				.content = Text { .text = std::string(text) },
			}
		);
	}

	void UserInterface::image(ImageID image, std::optional<Style> style, std::string id) {
		ASSERT(m_is_within_frame, "Missing call to UserInterface::frame_begin?");
		m_tree.push_element(
			Element {
				.id = id,
				.style = style.value_or(Style {}),
				.content = Image { .id = image },
			}
		);
	}

	bool UserInterface::element_is_hovered() const {
		const Element& element = m_tree.current_element();
		ASSERT(!element.id.empty(), "element_is_hovered called when current element lacks id!");
		return m_context.is_hovered(element);
	}

	bool UserInterface::element_is_active() const {
		const Element& element = m_tree.current_element();
		ASSERT(!element.id.empty(), "element_is_active called when current element lacks id!");
		return m_context.is_active(element);
	}
}
