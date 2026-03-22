#include "game/scene/main_menu.h"

#include "game/game.h"

#include <memory>
#include <raylib.h>
#include <raymath.h>
#include <variant>
#include <vector>

namespace ui {

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

	struct Element;

	struct TextContent {
		FontID font_id;
		int font_size;
		std::string text;
	};

	struct BoxContent {
		std::vector<Element> children;
	};

	using ElementContent = std::variant<BoxContent, TextContent>;

	struct ElementStyle {
		Margin margin;
		Border border;
		Padding padding;
	};

	struct ElementLayout {
		Rectangle margin_box;
		Rectangle border_box;
		Rectangle padding_box;
		Rectangle content_box;
	};

	struct Element {
		ElementContent content;
		ElementStyle style;
		ElementLayout layout;
	};

	inline ElementLayout operator+(const ElementLayout& lhs, const Vector2& rhs) {
		return {
			.margin_box = lhs.margin_box + rhs,
			.border_box = lhs.border_box + rhs,
			.padding_box = lhs.padding_box + rhs,
			.content_box = lhs.content_box + rhs,
		};
	}

	void compute_element_sizes(const ResourceManager& resources, Element* element) {
		const ElementStyle& style = element->style;
		ElementLayout* layout = &element->layout;

		if (const TextContent* text_content = std::get_if<TextContent>(&element->content)) {
			const Font& font = resources.get_font(text_content->font_id);
			const float font_spacing = 0.0f;
			const Vector2 text_size = Raylib_MeasureTextEx(font, text_content->text.c_str(), text_content->font_size, font_spacing);
			layout->content_box.width = text_size.x;
			layout->content_box.height = text_size.y;
		}

		layout->padding_box.width = layout->content_box.width + style.padding.left + style.padding.right;
		layout->padding_box.height = layout->content_box.height + style.padding.top + style.padding.bottom;
		layout->border_box.width = layout->padding_box.width + style.border.left + style.border.right;
		layout->border_box.height = layout->padding_box.height + style.border.top + style.border.bottom;
		layout->margin_box.width = layout->border_box.width + style.margin.left + style.margin.right;
		layout->margin_box.height = layout->border_box.height + style.margin.top + style.margin.bottom;
	}

	void compute_element_positions(Vector2 available_space, Element* element) {
		const ElementStyle style = element->style;
		ElementLayout* layout = &element->layout;

		// FIXME: position things based on the layout rules in `style`
		layout->margin_box.x = (available_space.x - layout->margin_box.width) / 2.0f;
		layout->margin_box.y = (available_space.y - layout->margin_box.height) / 2.0f;
		layout->border_box.x = layout->margin_box.x + style.margin.left;
		layout->border_box.y = layout->margin_box.y + style.margin.top;
		layout->padding_box.x = layout->border_box.x + style.border.left;
		layout->padding_box.y = layout->border_box.y + style.border.top;
		layout->content_box.x = layout->padding_box.x + style.padding.left;
		layout->content_box.y = layout->padding_box.y + style.padding.top;
	}

	void compute_element_layout(const ResourceManager& resources, Vector2 available_space, Element* element) {
		compute_element_sizes(resources, element);
		compute_element_positions(available_space, element);
	}

	void draw_element(const ResourceManager& resources, const Element& element) {
		Raylib_DrawRectangleRec(element.layout.border_box, GRAY);
		Raylib_DrawRectangleRec(element.layout.padding_box, LIGHTGRAY);
		Raylib_DrawRectangleLinesEx(element.layout.margin_box, 1, GREEN); // debug
		if (const ui::TextContent* text_content = std::get_if<ui::TextContent>(&element.content)) {
			const Font& font = resources.get_font(text_content->font_id);
			const Vector2 content_pos = { element.layout.content_box.x, element.layout.content_box.y };
			Raylib_DrawTextEx(font, text_content->text.c_str(), content_pos, text_content->font_size, 0.0f, BLACK);
		}
	}

	void debug_draw_element_boxes(const ElementLayout& element_boxes) {
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
	ui::Element text_element = {
		.content =
			ui::TextContent {
				.font_id = FontID::default_font(),
				.font_size = 16,
				.text = "Sphinx of black quarts, judge my vow!",
			},
		.style =
			ui::ElementStyle {
				.margin = ui::Margin::with_size(4),
				.border = ui::Border::with_size(4),
				.padding = ui::Padding::with_size(10),
			},
	};

	/* Compute layout */
	ui::compute_element_layout(game.resources, game.window.size(), &text_element);

	/* Render elements */
	ui::draw_element(game.resources, text_element);
}
