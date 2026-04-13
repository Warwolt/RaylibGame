#pragma once

#include "game/resource.h"

#include <raylib.h>

#include <string>
#include <variant>
#include <vector>

namespace ui {

	/* Content */
	struct Element;

	struct Text {
		std::string text;
		std::vector<std::string> lines; // computed during layout
	};

	enum class Direction {
		Horizontal,
		Vertical,
	};

	struct Box {
		Direction direction = Direction::Vertical;
		std::vector<Element> children;
	};

	using Content = std::variant<Box, Text>;

	/* Style */
	struct AbsoluteSize {
		int pixels;
	};

	struct RelativeSize {
		float percentage; // relative parent size, range [0, 100]
	};

	using Size = std::variant<AbsoluteSize, RelativeSize>;

	struct Spacing {
		float top;
		float bottom;
		float left;
		float right;

		inline float horizontal() const {
			return this->left + this->right;
		}

		inline float vertical() const {
			return this->top + this->bottom;
		}

		static Spacing uniform(float size) {
			return { size, size, size, size };
		}
	};

	enum class Alignment {
		Start,
		Center,
		End,
	};

	struct StyleDebug {
		bool show_margin_outline = false;
		bool show_content_outline = false;
	};

	struct Style {
		Size width = RelativeSize(100);
		Size height = RelativeSize(100);
		Spacing margin;
		Spacing border;
		Spacing padding;
		Alignment alignment;
		Color border_color;
		struct {
			Color base;
			Color hovered;
			Color active;
		} background_color;
		Color font_color = WHITE;
		FontID font_id = FontID::default_font();
		int font_size = 16;
		StyleDebug debug;

		inline float horizontal_spacing() const {
			return margin.horizontal() + border.horizontal() + padding.horizontal();
		}

		inline float vertical_spacing() const {
			return margin.vertical() + border.vertical() + padding.vertical();
		}
	};

	/* Layout */
	struct Layout {
		Rectangle margin_box;
		Rectangle border_box;
		Rectangle padding_box;
		Rectangle content_box;
	};

	/* State */
	struct Input {
		Vector2 mouse_pos;
		bool left_mouse_is_down;
	};

	struct State {
		bool is_hovered; // e.g. mouse over
		bool is_active; // e.g. mouse click (button down)
	};

	/* Element */
	struct Element {
		Style style;
		Content content;
		Layout layout;
		State state;
	};

	void layout_element(const ResourceManager& resources, Vector2 window_size, Element* element);
	void update_element(const Input& input, Element* element);
	void draw_element(const ResourceManager& resources, const Element& element);

} // namespace ui
