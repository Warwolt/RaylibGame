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
		ImageID image; // FIXME: rename to id (image->id makes more sense than image->image)
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
	struct Pixels {
		float value;
	};
	struct Percentage {
		float value;
		inline float fractional() const {
			return value / 100.0f;
		}
	};
	struct Measure {
		std::variant<Pixels, Percentage> value;

		Measure(Pixels pixels)
			: value(pixels) {
		}

		Measure(Percentage percentage)
			: value(percentage) {
		}

		inline bool is_pixels() const {
			return std::holds_alternative<Pixels>(this->value);
		}

		inline const Pixels* pixels() const {
			return std::get_if<Pixels>(&this->value);
		}

		inline bool is_percentage() const {
			return std::holds_alternative<Percentage>(this->value);
		}

		inline const Percentage* percentage() const {
			return std::get_if<Percentage>(&this->value);
		}
	};

	struct StaticPosition {};
	struct RelativePosition {
		Measure x;
		Measure y;
	};
	struct AbsolutePosition {
		Measure x;
		Measure y;
	};
	struct Position {
		std::variant<StaticPosition, RelativePosition, AbsolutePosition> value;

		Position(StaticPosition static_position)
			: value(static_position) {
		}

		Position(RelativePosition relative_position)
			: value(relative_position) {
		}

		Position(AbsolutePosition absolute_position)
			: value(absolute_position) {
		}

		inline bool is_static_position() const {
			return std::holds_alternative<StaticPosition>(this->value);
		}

		inline bool is_relative_position() const {
			return std::holds_alternative<RelativePosition>(this->value);
		}

		inline const RelativePosition* relative_position() const {
			return std::get_if<RelativePosition>(&this->value);
		}

		inline bool is_absolute_position() const {
			return std::holds_alternative<AbsolutePosition>(this->value);
		}

		inline const AbsolutePosition* absolute_position() const {
			return std::get_if<AbsolutePosition>(&this->value);
		}
	};

	// in pixels
	struct Edges {
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

		static Edges uniform(float size) {
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

	struct BorderStyle {
		Edges edges;
		Color color;
		ImageID image;
		Edges image_slices; // for 9-slicing
		bool image_fill_center;
	};

	struct BackgroundStyle {
		Color color;
		ImageID image;
		Fill fill = Fill::Stretch;
	};

	struct FontStyle {
		FontID id = FontID::default_font();
		int size = 16;
		Color color = WHITE;
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
		Position position = StaticPosition();
		std::optional<Measure> width;
		std::optional<Measure> height;
		bool fit_content = false; // width and height ignored if true
		Edges margin;
		BorderStyle border;
		Edges padding;
		Alignment alignment;
		Alignment cross_alignment;
		BackgroundStyle background;
		FontStyle font;

		StyleOverride hovered;
		StyleOverride active;
		StyleDebug debug;

		inline float horizontal_spacing() const {
			return margin.horizontal() + border.edges.horizontal() + padding.horizontal();
		}

		inline float vertical_spacing() const {
			return margin.vertical() + border.edges.vertical() + padding.vertical();
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
		std::string debug_name;
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
