#include "game/ui/user_interface.h"

#include "core/debug/assert.h"
#include "core/debug/profiling.h"

#include <format>

namespace ui {

	static std::string try_generate_automatic_id(const Element& parent) {
		if (parent.id.empty()) {
			return "";
		}
		const size_t index = parent.box()->children.size();
		return std::format("{}.children[{}]", parent.id, index);
	}

	void UserInterface::draw(const ResourceManager& resources) const {
		draw_element(resources, m_context, m_tree.root());
	}

	const Element& UserInterface::root_element() const {
		return m_tree.root();
	}

	void UserInterface::frame_begin() {
		PROFILING_SCOPE();
		ASSERT(!m_is_within_frame, "Missing call to UserInterface::frame_end?");
		m_is_within_frame = true;
		m_tree.reset();
	}

	void UserInterface::frame_end(const Input& input, const ResourceManager& resources, Vector2 window_size) {
		PROFILING_SCOPE();
		ASSERT(m_is_within_frame, "Missing call to UserInterface::frame_begin?");
		ASSERT(!m_tree.has_open_element(), "UserInterface::box_begin and box_end calls don't match. Missing call to UserInterface::box_end?");
		m_is_within_frame = false;
		layout_element(resources, m_context, window_size, &m_tree.root());
		update_element(input, &m_context, &m_tree.root());
		m_context.update();
	}

	void UserInterface::box_begin(Direction direction, std::optional<Style> style, std::string id) {
		ASSERT(m_is_within_frame, "Missing call to UserInterface::frame_begin?");
		m_tree.push_element(
			Element {
				.id = id.empty() ? try_generate_automatic_id(m_tree.current_parent()) : id,
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
				.id = id.empty() ? try_generate_automatic_id(m_tree.current_parent()) : id,
				.style = style.value_or(Style {}),
				.content = Text { .text = std::string(text) },
			}
		);
	}

	void UserInterface::image(ImageID image, std::optional<Style> style, std::string id) {
		ASSERT(m_is_within_frame, "Missing call to UserInterface::frame_begin?");
		m_tree.push_element(
			Element {
				.id = id.empty() ? try_generate_automatic_id(m_tree.current_parent()) : id,
				.style = style.value_or(Style {}),
				.content = Image { .id = image },
			}
		);
	}

	Tracked<bool> UserInterface::element_is_hovered() const {
		const Element& element = m_tree.current_element();
		ASSERT(!element.id.empty(), "element_is_hovered called when current element lacks id!");
		return m_context.is_hovered(element);
	}

	Tracked<bool> UserInterface::element_is_focused() const {
		const Element& element = m_tree.current_element();
		ASSERT(!element.id.empty(), "element_is_focused called when current element lacks id!");
		return m_context.is_focused(element);
	}

	Tracked<bool> UserInterface::element_is_active() const {
		const Element& element = m_tree.current_element();
		ASSERT(!element.id.empty(), "element_is_active called when current element lacks id!");
		return m_context.is_active(element);
	}

	Tracked<bool> UserInterface::element_is_clicked() const {
		const Element& element = m_tree.current_element();
		ASSERT(!element.id.empty(), "element_is_clicked called when current element lacks id!");
		return m_context.is_clicked(element);
	}

	void UserInterface::initially_focus_current_element() {
		if (m_context.focused_element_id->empty()) {
			focus_current_element();
		}
	}

	void UserInterface::focus_current_element() {
		const Element& element = m_tree.current_element();
		ASSERT(!element.id.empty(), "focus_current_element called when current element lacks id!");
		m_context.focus_element(element);
	}
}
