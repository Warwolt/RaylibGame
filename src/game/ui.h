#pragma once

#include "game/resource.h"

#include <raylib.h>

#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace ui {

	/* Content */
	struct Element;

	struct Text {
		std::string text;
		std::vector<std::string_view> lines; // computed during layout, views into `text` member
	};

	struct Image {
		ImageID image;
	};

	enum class Direction {
		Horizontal,
		Vertical,
	};

	struct Box {
		Direction direction = Direction::Vertical;
		std::vector<Element> children;
	};

	using Content = std::variant<Box, Text, Image>;

	/* Style */
	struct AbsoluteSize {
		int pixels = 0;
	};

	struct RelativeSize {
		float percentage; // relative parent size, range [0, 100]
	};

	using Size = std::variant<AbsoluteSize, RelativeSize>;

	// in pixels
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

	enum class Fill {
		Repeat,
		Stretch,
	};

	struct StyleDebug {
		bool show_margin_outline = false;
		bool show_content_outline = false;
	};

	struct StyleOverride {
		std::optional<Color> border_color;
		std::optional<Color> background_color;
		std::optional<Color> font_color;
	};

	struct Style {
		Size width = RelativeSize(100);
		Size height = RelativeSize(100);
		Spacing margin;
		Spacing border;
		Spacing padding;
		Alignment alignment;

		Color border_color;
		Color background_color;

		Color font_color = WHITE;
		FontID font_id = FontID::default_font();
		int font_size = 16;

		ImageID background_image = ImageID(0);
		Fill background_fill = Fill::Repeat;

		ImageID border_image = ImageID(0);
		Spacing border_image_slices;

		StyleOverride hovered;
		StyleOverride active;
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
	enum class ButtonState {
		Up,
		Released,
		Down,
		Pressed,
	};

	struct Input {
		Vector2 mouse_pos;
		ButtonState left_mouse_button;
	};

	struct State {
		bool is_hovered;
		bool is_active;
		bool is_clicked;
	};

	/* Element */
	struct Element {
		Style style;
		Content content;
		Layout layout; // computed with layout_element()
		State state; // computed with update_element()

		inline bool is_box() const {
			return std::holds_alternative<Box>(this->content);
		}

		inline Box* box() {
			return std::get_if<Box>(&this->content);
		}

		inline const Box* box() const {
			return std::get_if<Box>(&this->content);
		}

		inline bool is_text() const {
			return std::holds_alternative<Text>(this->content);
		}

		inline Text* text() {
			return std::get_if<Text>(&this->content);
		}

		inline const Text* text() const {
			return std::get_if<Text>(&this->content);
		}

		inline bool is_image() const {
			return std::holds_alternative<Image>(this->content);
		}

		inline Image* image() {
			return std::get_if<Image>(&this->content);
		}

		inline const Image* image() const {
			return std::get_if<Image>(&this->content);
		}
	};

	void layout_element(const ResourceManager& resources, Vector2 window_size, Element* element);
	bool update_element(const Input& input, Element* element);
	void draw_element(const ResourceManager& resources, const Element& element);

	class UserInterface {
	public:
		void draw(const ResourceManager& resources) const;
		const Element& root_element() const;

		void frame_begin();
		void frame_end(const ResourceManager& resources, Vector2 window_size);

		void box_begin(Direction direction = Direction::Vertical, std::optional<Style> style = {});
		void box_end();

		void text(std::string_view text, std::optional<Style> style = {});
		void image(ImageID image, std::optional<Style> style = {});

	private:
		bool m_within_frame = false;
		Element m_root_element = {};
		std::vector<Element*> m_parent_stack;

		Element* _current_parent();
	};

} // namespace ui
