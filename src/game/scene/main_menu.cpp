#include "game/scene/main_menu.h"

#include "game/game.h"

#include <raylib.h>

// FIXME: move this to Raymath.h
inline Rectangle operator+(const Rectangle& lhs, const Vector2& rhs) {
	return {
		.x = lhs.x + rhs.x,
		.y = lhs.y + rhs.y,
		.width = lhs.width,
		.height = lhs.height,
	};
}

namespace ui {

	struct Margin {
		float top;
		float bottom;
		float left;
		float right;
	};

	struct Border {
		float top;
		float bottom;
		float left;
		float right;
	};

	struct Padding {
		float top;
		float bottom;
		float left;
		float right;
	};

	struct TextElement {
		Margin margin;
		Border border;
		Padding padding;
		FontID font_id;
		int font_size;
		std::string text;
	};

	// using Element = std::variant<BoxElement, TextElement, ImageElement, ...>

	struct ElementBoxes {
		Rectangle margin_box;
		Rectangle border_box;
		Rectangle padding_box;
		Rectangle content_box;
	};

	inline ElementBoxes operator+(const ElementBoxes& lhs, const Vector2& rhs) {
		return {
			.margin_box = lhs.margin_box + rhs,
			.border_box = lhs.border_box + rhs,
			.padding_box = lhs.padding_box + rhs,
			.content_box = lhs.content_box + rhs,
		};
	}

	Vector2 compute_content_size(const ResourceManager& resources, const TextElement& element) {
		const Font& font = resources.default_font(); // FIXME: get the font with element.font_id
		const float font_spacing = 0.0f;
		return Raylib_MeasureTextEx(font, element.text.c_str(), element.font_size, font_spacing);
	}

	ElementBoxes compute_element_boxes(const ResourceManager& resources, const TextElement& element) {
		/* Box widths */
		// FIXME: can this be const auto& [content_width, content_height] = compute_content_size(resources, element); ?
		const Vector2 content_size = compute_content_size(resources, element);
		const float content_width = content_size.x;
		const float content_height = content_size.y;
		const float padding_width = content_width + element.padding.left + element.padding.right;
		const float padding_height = content_height + element.padding.top + element.padding.bottom;
		const float border_width = padding_width + element.border.left + element.border.right;
		const float border_height = padding_height + element.border.top + element.border.bottom;
		const float margin_width = border_width + element.margin.left + element.margin.right;
		const float margin_height = border_height + element.margin.top + element.margin.bottom;

		/* Box positions */
		ElementBoxes element_boxes = {};
		element_boxes.margin_box = {
			.x = 0,
			.y = 0,
			.width = margin_width,
			.height = margin_height,
		};
		element_boxes.border_box = {
			.x = element_boxes.margin_box.x + element.margin.left,
			.y = element_boxes.margin_box.y + element.margin.top,
			.width = border_width,
			.height = border_height,
		};
		element_boxes.padding_box = {
			.x = element_boxes.border_box.x + element.border.left,
			.y = element_boxes.border_box.y + element.border.top,
			.width = padding_width,
			.height = padding_height,
		};
		element_boxes.content_box = {
			.x = element_boxes.padding_box.x + element.padding.left,
			.y = element_boxes.padding_box.y + element.padding.top,
			.width = content_width,
			.height = content_height,
		};

		return element_boxes;
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
	const ui::TextElement element = {
		.margin = {
			.top = 10,
			.bottom = 10,
			.left = 10,
			.right = 10,
		},
		.border = {
			.top = 10,
			.bottom = 10,
			.left = 10,
			.right = 10,
		},
		.padding = {
			.top = 10,
			.bottom = 10,
			.left = 10,
			.right = 10,
		},
		.font_id = FontID::default_font(),
		.font_size = 32,
		.text = "Sphinx of black quarts, judge my vow!",
	};

	/* Compute layout */
	ui::ElementBoxes element_boxes = ui::compute_element_boxes(game.resources, element);
	Vector2 centered_box_pos = Vector2 {
		.x = (float)(game.window.width() - element_boxes.margin_box.width) / 2.0f,
		.y = (float)(game.window.height() - element_boxes.margin_box.height) / 2.0f,
	};
	element_boxes = element_boxes + centered_box_pos;

	/* Render boxes */
	const Font& font = game.resources.default_font(); // FIXME: get the font with element.font_id
	const Vector2 content_pos = { element_boxes.content_box.x, element_boxes.content_box.y };
	const Color margin_color = { 240, 206, 163, 255 };
	const Color border_color = { 246, 221, 161, 255 };
	const Color padding_color = { 198, 207, 145, 255 };
	const Color content_color = { 152, 181, 192, 255 };
	Raylib_DrawRectangleRec(element_boxes.margin_box, margin_color);
	Raylib_DrawRectangleRec(element_boxes.border_box, border_color);
	Raylib_DrawRectangleRec(element_boxes.padding_box, padding_color);
	Raylib_DrawRectangleRec(element_boxes.content_box, content_color);
	Raylib_DrawTextEx(font, element.text.c_str(), content_pos, element.font_size, 0.0f, WHITE);
}
