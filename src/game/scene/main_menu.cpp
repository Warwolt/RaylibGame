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

	struct Margin {
		float top;
		float bottom;
		float left;
		float right;

		static Margin with_size(float size) {
			return { size, size, size, size };
		}
	};

	struct Border {
		float top;
		float bottom;
		float left;
		float right;

		static Border with_size(float size) {
			return { size, size, size, size };
		}
	};

	struct Padding {
		float top;
		float bottom;
		float left;
		float right;

		static Padding with_size(float size) {
			return { size, size, size, size };
		}
	};

	struct Style {
		Margin margin;
		Border border;
		Padding padding;
		Color border_color = { 0, 0, 0, 0 };
		Color background_color = { 0, 0, 0, 0 };
		FontID font_id;
		int font_size;
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

	void compute_element_sizes(const ResourceManager& resources, Element* element) {
		const Style& style = element->style;
		Layout* layout = &element->layout;

		if (const TextContent* text_content = std::get_if<TextContent>(&element->content)) {
			const Font& font = resources.get_font(style.font_id);
			const float font_spacing = 0.0f;
			const Vector2 text_size = Raylib_MeasureTextEx(font, text_content->text.c_str(), style.font_size, font_spacing);
			layout->content_box.width = text_size.x;
			layout->content_box.height = text_size.y;
		}
		if (BoxContent* box_content = std::get_if<BoxContent>(&element->content)) {
			for (Element& child : box_content->children) {
				compute_element_sizes(resources, &child);
			}

			switch (box_content->direction) {
				case Direction::Horizontal: {
					float max_height = 0;
					float total_width = 0;
					for (const Element& child : box_content->children) {
						total_width += child.layout.margin_box.width;
						max_height = std::max(max_height, child.layout.margin_box.height);
					}
					layout->content_box.width = total_width;
					layout->content_box.height = max_height;
				} break;

				case Direction::Vertical:
					float total_height = 0;
					float max_width = 0;
					for (const Element& child : box_content->children) {
						max_width = std::max(max_width, child.layout.margin_box.width);
						total_height += child.layout.margin_box.height;
					}
					layout->content_box.width = max_width;
					layout->content_box.height = total_height;
					break;
			}
		}

		layout->padding_box.width = layout->content_box.width + style.padding.left + style.padding.right;
		layout->padding_box.height = layout->content_box.height + style.padding.top + style.padding.bottom;
		layout->border_box.width = layout->padding_box.width + style.border.left + style.border.right;
		layout->border_box.height = layout->padding_box.height + style.border.top + style.border.bottom;
		layout->margin_box.width = layout->border_box.width + style.margin.left + style.margin.right;
		layout->margin_box.height = layout->border_box.height + style.margin.top + style.margin.bottom;
	}

	float aligned_position(float element_size, float parent_size, Alignment alignment) {
		switch (alignment) {
			case Alignment::Start:
				return 0;

			case Alignment::Center:
				return (parent_size - element_size) / 2.0f;

			case Alignment::End:
				return parent_size - element_size;
		}
		return 0;
	}

	void compute_element_positions(Vector2 /*parent_size*/, Vector2 position, Element* element) {
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
				compute_element_positions(element_size, child_position, &child);

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

	void compute_element_layout(const ResourceManager& resources, Vector2 parent_size, Element* element) {
		compute_element_sizes(resources, element);
		compute_element_positions(parent_size, Vector2 { 0, 0 }, element);
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
		.margin = ui::Margin::with_size(0),
		.border = ui::Border::with_size(4),
		.padding = ui::Padding::with_size(10),
		.border_color = GRAY,
		.background_color = LIGHTGRAY,
		.font_id = FontID::default_font(),
		.font_size = 16,
	};
	ui::Element root_element = {
		.style = {
			.margin = ui::Margin::with_size(10),
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
