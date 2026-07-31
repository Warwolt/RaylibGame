#include "game/ui/user_interface.h"

#include "core/debug/assert.h"
#include "core/debug/profiling.h"
#include "game/game.h"

#include <cmath>
#include <format>
#include <numbers>

namespace ui {

	void UserInterface::initialize(Game* game) {
		m_images.focus_indicator = game->resources.load_image("resource/image/pointing_hand.png").value();
		m_sounds.menu_navigate = game->resources.load_sound("resource/sound/menu_navigate.wav").value();
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

	void UserInterface::box_begin(std::optional<Style> style, std::string id) {
		ASSERT(m_is_within_frame, "Missing call to UserInterface::frame_begin?");
		_push_element(
			Element {
				.id = id,
				.style = style.value_or(Style {}),
				.content = Box {},
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
		_push_element(
			Element {
				.id = id,
				.style = style.value_or(Style {}),
				.content = Text { .text = std::string(text) },
			}
		);
	}

	void UserInterface::image(ImageID image, std::optional<Style> style, std::string id) {
		ASSERT(m_is_within_frame, "Missing call to UserInterface::frame_begin?");
		_push_element(
			Element {
				.id = id,
				.style = style.value_or(Style {}),
				.content = Image { .id = image },
			}
		);
	}

	void UserInterface::menu_begin() {
		const Style menu_begin_style = {
			.fit_content = true,
			.cross_alignment = Alignment::Center,
		};
		UserInterface::box_begin(menu_begin_style);
	}

	bool UserInterface::menu_item(const Input& input, const ResourceManager& resources, std::string_view label) {
		const Style menu_item_style = {
			.position = RelativePosition { .x = Pixels(0), .y = Pixels(0) },
			.fit_content = true,
			.direction = Direction::Horizontal,
		};
		bool item_is_clicked = false;
		box_begin(menu_item_style);
		{
			const bool hovered_and_mouse = input.last_input_was_mouse() && element_is_hovered();
			const bool keyboard_or_gamepad = input.last_input_was_keyboard() || input.last_input_was_gamepad();
			const bool focused_and_keyboard_or_gamepad = keyboard_or_gamepad && element_is_focused();

			/* On click */
			item_is_clicked = element_is_clicked();

			/* On hover */
			if (element_is_hovered().has_changed_to(true)) {
				focus_current_element();
			}

			/* On focus */
			if (element_is_focused().has_changed_to(true)) {
				Sound sound = resources.get_sound(m_sounds.menu_navigate);
				Raylib_SetSoundVolume(sound, 0.5f);
				Raylib_PlaySound(sound);
			}

			/* Focus indicator */
			const double time_now = input.time_now.in_seconds();
			const double period = 1.8; // seconds
			const double freq = 1.0 / period;
			const double two_pi = 2.0 * std::numbers::pi;
			const float focus_indicator_offset = -10.0f * std::abs(std::cos(time_now * two_pi * freq));
			const int focus_indicator_size = 48;
			const Style focus_indicator_style = {
				.position =
					AbsolutePosition {
						.x = Pixels(-focus_indicator_size + focus_indicator_offset),
						.y = Pixels(-8),
					},
				.width = Pixels(focus_indicator_size),
				.height = Pixels(focus_indicator_size),
			};
			if (hovered_and_mouse || focused_and_keyboard_or_gamepad) {
				image(m_images.focus_indicator, focus_indicator_style);
			}

			/* Menu text */
			const Style item_label_style = {
			.width = Pixels(130),
			.padding = {
				.bottom = 2,
			},
			.alignment = Alignment::Center,
			.font = {
				.size = 32,
				.color = (hovered_and_mouse || focused_and_keyboard_or_gamepad) ? YELLOW : WHITE,
			},
		};
			text(label, item_label_style);
		}
		box_end();
		return item_is_clicked;
	}

	void UserInterface::menu_end() {
		box_end();
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
			// Set both current and previous value so that initially focused
			// element doesn't trigger any focus changed events.
			const Element& element = m_tree.current_element();
			m_context.focused_element_id = { element.id, element.id };
		}
	}

	void UserInterface::initially_focus_next_element() {
		m_should_initially_focus_next_element = true;
	}

	void UserInterface::focus_current_element() {
		const Element& element = m_tree.current_element();
		ASSERT(!element.id.empty(), "focus_current_element called when current element lacks id!");
		m_context.focused_element_id = element.id;
	}

	void UserInterface::_push_element(Element element) {
		/* Try generate ID if missing */
		const Element& parent = m_tree.current_parent();
		if (element.id.empty() && !parent.id.empty()) {
			const size_t index = parent.box()->children.size();
			element.id = std::format("{}.children[{}]", parent.id, index);
		}

		/* Push element */
		m_tree.push_element(element);

		/* Handle focus */
		if (m_should_initially_focus_next_element) {
			m_should_initially_focus_next_element = false;
			initially_focus_current_element();
		}
	}
}
