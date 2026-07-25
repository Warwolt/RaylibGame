#pragma once

#include "game/resource_id.h"

#include <raylib.h>

#include <optional>
#include <variant>

namespace ui {

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

	struct BorderStyleOverride {
		std::optional<Color> color;
		std::optional<ImageID> image;
		std::optional<Edges> image_slices; // for 9-slicing
		std::optional<bool> image_fill_center;
	};

	struct BackgroundStyle {
		Color color;
		ImageID image;
		Fill fill = Fill::Stretch;
	};

	struct BackgroundStyleOverride {
		std::optional<Color> color;
		std::optional<ImageID> image;
		std::optional<Fill> fill;
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
		BorderStyleOverride border;
		BackgroundStyleOverride background;
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

		inline Vector2 spacing() const {
			return { horizontal_spacing(), vertical_spacing() };
		}
	};

} // namespace ui
