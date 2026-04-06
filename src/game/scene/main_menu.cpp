#include "game/scene/main_menu.h"

#include "game/game.h"

#include <raylib.h>
#include <raymath.h>

#include <algorithm>
#include <memory>
#include <variant>
#include <vector>

namespace ui {

	/* Content */
	struct Element;

	enum class Direction {
		Horizontal,
		Vertical,
	};

	struct TextContent {
		std::string text;
	};

	struct BoxContent {
		Direction direction = Direction::Vertical;
		std::vector<Element> children;
	};

	using Content = std::variant<BoxContent, TextContent>;

	/* Style */
	enum class Alignment {
		Start,
		Center,
		End,
	};

	struct Absolute {
		int pixels;
	};

	struct Relative {
		int percentage;
	};

	struct Size {
		std::variant<Absolute, Relative> value;

		Size(Absolute value)
			: value(value) {
		}

		Size(Relative value)
			: value(value) {
		}

		float fit_to_parent(float parent_size) {
			float pixels = 0.0f;
			if (const Absolute* absolute_width = std::get_if<Absolute>(&this->value)) {
				pixels = std::min<float>(absolute_width->pixels, parent_size);
			}
			if (const Relative* relative_width = std::get_if<Relative>(&this->value)) {
				pixels = (relative_width->percentage / 100.0f) * parent_size;
			}
			return pixels;
		}
	};

	struct Spacing {
		float top;
		float bottom;
		float left;
		float right;

		float horizontal() const {
			return this->left + this->right;
		}

		float vertical() const {
			return this->top + this->bottom;
		}

		static Spacing with_size(float size) {
			return { size, size, size, size };
		}
	};

	struct Style {
		Size width = Relative(100);
		Size height = Relative(100);
		Spacing margin;
		Spacing border;
		Spacing padding;
		Color border_color = { 0, 0, 0, 0 };
		Color background_color = { 0, 0, 0, 0 };
		FontID font_id;
		int font_size;

		float horizontal_spacing() const {
			return margin.horizontal() + border.horizontal() + padding.horizontal();
		}

		float vertical_spacing() const {
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

	/* Element */
	struct Element {
		Style style;
		Content content;
		Layout layout;
	};

	void compute_element_sizes(const ResourceManager& resources, Vector2 available_size, Element* element) {
		const Style& style = element->style;
		Layout* layout = &element->layout;

		if (const TextContent* text_content = std::get_if<TextContent>(&element->content)) {
			const Font& font = resources.get_font(style.font_id);
			float text_width = 0;
			const float text_height = element->style.font_size;
			const float font_scaling = element->style.font_size / (float)font.baseSize;
			for (char character : text_content->text) {
				const GlyphInfo glyph = Raylib_GetGlyphInfo(font, character);
				const float advance_x = font_scaling * glyph.advanceX;
				text_width += advance_x;
			}
			layout->content_box.width = text_width;
			layout->content_box.height = text_height;
		}

		if (BoxContent* box_content = std::get_if<BoxContent>(&element->content)) {
			/* Size content box */
			const float element_width = element->style.width.fit_to_parent(available_size.x);
			const float element_height = element->style.width.fit_to_parent(available_size.y);
			layout->content_box.width = element_width - style.horizontal_spacing();
			layout->content_box.height = element_height - style.vertical_spacing();

			/* Size all children */
			for (Element& child : box_content->children) {
				// FIXME: Check what the _desired_ size of the child is, and set
				// the available size to the minimum of desired size and (parent
				// size)/N where N is num children
				Vector2 available_child_size = available_size / box_content->children.size();
				compute_element_sizes(resources, available_child_size, &child);
			}
		}

		layout->padding_box.width = layout->content_box.width + style.padding.left + style.padding.right;
		layout->padding_box.height = layout->content_box.height + style.padding.top + style.padding.bottom;
		layout->border_box.width = layout->padding_box.width + style.border.left + style.border.right;
		layout->border_box.height = layout->padding_box.height + style.border.top + style.border.bottom;
		layout->margin_box.width = layout->border_box.width + style.margin.left + style.margin.right;
		layout->margin_box.height = layout->border_box.height + style.margin.top + style.margin.bottom;
	}

	void compute_element_positions(Vector2 position, Element* element) {
		const Style style = element->style;
		Layout* layout = &element->layout;

		/* Position all boxes relative to each other */
		layout->margin_box.x = position.x;
		layout->margin_box.y = position.y;
		layout->border_box.x = layout->margin_box.x + style.margin.left;
		layout->border_box.y = layout->margin_box.y + style.margin.top;
		layout->padding_box.x = layout->border_box.x + style.border.left;
		layout->padding_box.y = layout->border_box.y + style.border.top;
		layout->content_box.x = layout->padding_box.x + style.padding.left;
		layout->content_box.y = layout->padding_box.y + style.padding.top;

		/* Recurse into children */
		if (BoxContent* box_content = std::get_if<BoxContent>(&element->content)) {
			Vector2 cursor = { layout->content_box.x, layout->content_box.y };
			for (Element& child : box_content->children) {
				/* Compute child position */
				Vector2 element_size = { layout->margin_box.width, layout->margin_box.height };
				Vector2 child_position = cursor;
				compute_element_positions(child_position, &child);

				/* Move cursor */
				switch (box_content->direction) {
					case Direction::Horizontal:
						cursor.x += child.layout.margin_box.width;
						break;
					case Direction::Vertical:
						cursor.y += child.layout.margin_box.height;
						break;
				}
			}
		}
	}

	void compute_element_layout(const ResourceManager& resources, Vector2 window_size, Element* element) {
		const Vector2 top_left = { 0, 0 };
		compute_element_sizes(resources, window_size, element);
		compute_element_positions(top_left, element);
	}

	void draw_element(const ResourceManager& resources, const Element& element) {
		const Style& style = element.style;
		Raylib_DrawRectangleRec(element.layout.border_box, element.style.border_color);
		Raylib_DrawRectangleRec(element.layout.padding_box, element.style.background_color);
		Raylib_DrawRectangleLinesEx(element.layout.margin_box, 1, GREEN); // debug
		if (const ui::TextContent* text_content = std::get_if<ui::TextContent>(&element.content)) {
			const Font& font = resources.get_font(style.font_id);
			const Vector2 content_pos = { element.layout.content_box.x, element.layout.content_box.y };
			Raylib_DrawTextEx(font, text_content->text.c_str(), content_pos, style.font_size, 0.0f, BLACK);
		}
		if (const ui::BoxContent* box_content = std::get_if<ui::BoxContent>(&element.content)) {
			for (const Element& child : box_content->children) {
				draw_element(resources, child);
			}
		}
	}

	void debug_draw_element_boxes(const Layout& element_boxes) {
		const Color margin_color = { 240, 206, 163, 255 };
		const Color border_color = { 246, 221, 161, 255 };
		const Color padding_color = { 198, 207, 145, 255 };
		const Color content_color = { 152, 181, 192, 255 };
		Raylib_DrawRectangleRec(element_boxes.margin_box, margin_color);
		Raylib_DrawRectangleRec(element_boxes.border_box, border_color);
		Raylib_DrawRectangleRec(element_boxes.padding_box, padding_color);
		Raylib_DrawRectangleRec(element_boxes.content_box, content_color);
	}
}

void MainMenuScene::initialize(Game* /*game*/) {
}

void MainMenuScene::deinitialize(Game* /*game*/) {
}

void MainMenuScene::update(Game* game) {
	if (Raylib_IsKeyPressed(KEY_ESCAPE)) {
		game->scenes.pop_scene(game);
	}
	if (Raylib_IsKeyPressed(KEY_ENTER)) {
		game->scenes.push_scene(game, SceneID::Gameplay);
	}
}

void MainMenuScene::render(const Game& game) const {
	/* Input */
	ui::Style text_style = {
		.margin = ui::Spacing::with_size(0),
		.border = ui::Spacing::with_size(4),
		.padding = ui::Spacing::with_size(10),
		.border_color = GRAY,
		.background_color = LIGHTGRAY,
		.font_id = FontID::default_font(),
		.font_size = 16,
	};
	ui::Element root_element = {
		.style = {
			.width = ui::Relative(50),
			.height = ui::Relative(50),
		},
		.content =
			ui::BoxContent {
				.direction = ui::Direction::Horizontal,
				.children = {
					ui::Element {
						.style = text_style,
						.content =
							ui::TextContent {
								.text = "Sphinx of black quarts, judge my vow!",
							},
					},
					ui::Element {
						.style = text_style,
						.content =
							ui::TextContent {
								.text = "The quick brown fox jumps over the lazy dog",
							},
					},
				},
			},
	};

	/* Compute layout */
	ui::compute_element_layout(game.resources, game.window.size(), &root_element);

	/* Render elements */
	ui::draw_element(game.resources, root_element);
}
