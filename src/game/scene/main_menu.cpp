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

	Vector2 compute_content_size(const ResourceManager& resources, const ElementContent& content) {
		if (const BoxContent* box_content = std::get_if<BoxContent>(&content)) {
			// FIXME: Computing the size isn't particularly meaningful for a Box
			// We need to be computing the entire layout, since we're recursing into the children
		}

		if (const TextContent* text_content = std::get_if<TextContent>(&content)) {
			const Font& font = resources.get_font(text_content->font_id);
			const float font_spacing = 0.0f;
			return Raylib_MeasureTextEx(font, text_content->text.c_str(), text_content->font_size, font_spacing);
		}
		return { 0, 0 };
	}

	ElementLayout compute_layout(const ResourceManager& resources, const Element& element) {
		const ElementStyle& style = element.style;

		/* Box widths */
		Vector2 content_size = compute_content_size(resources, element.content);
		const float content_width = content_size.x;
		const float content_height = content_size.y;
		const float padding_width = content_width + style.padding.left + style.padding.right;
		const float padding_height = content_height + style.padding.top + style.padding.bottom;
		const float border_width = padding_width + style.border.left + style.border.right;
		const float border_height = padding_height + style.border.top + style.border.bottom;
		const float margin_width = border_width + style.margin.left + style.margin.right;
		const float margin_height = border_height + style.margin.top + style.margin.bottom;

		/* Box positions */
		ElementLayout layout = {};
		layout.margin_box = {
			.x = 0,
			.y = 0,
			.width = margin_width,
			.height = margin_height,
		};
		layout.border_box = {
			.x = layout.margin_box.x + style.margin.left,
			.y = layout.margin_box.y + style.margin.top,
			.width = border_width,
			.height = border_height,
		};
		layout.padding_box = {
			.x = layout.border_box.x + style.border.left,
			.y = layout.border_box.y + style.border.top,
			.width = padding_width,
			.height = padding_height,
		};
		layout.content_box = {
			.x = layout.padding_box.x + style.padding.left,
			.y = layout.padding_box.y + style.padding.top,
			.width = content_width,
			.height = content_height,
		};

		return layout;
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
	const ui::Element text_element = {
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
	// FIXME: we're centering outside of compute_layout which doesn't make sense
	// Should we pass in the available screen space as argument?
	// ui::compute_layout(game.resources, &root_element);
	ui::ElementLayout element_layout = ui::compute_layout(game.resources, text_element);
	Vector2 centered_box_pos = Vector2 {
		.x = (float)(game.window.width() - element_layout.margin_box.width) / 2.0f,
		.y = (float)(game.window.height() - element_layout.margin_box.height) / 2.0f,
	};
	element_layout = element_layout + centered_box_pos;

	/* Render boxes */
	ui::draw_element(game.resources, text_element);
}
