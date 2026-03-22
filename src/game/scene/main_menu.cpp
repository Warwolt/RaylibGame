#include "game/scene/main_menu.h"

#include "game/game.h"

#include <raylib.h>

struct Margin {
	int top;
	int bottom;
	int left;
	int right;
};

struct Border {
	int top;
	int bottom;
	int left;
	int right;
};

struct Padding {
	int top;
	int bottom;
	int left;
	int right;
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

// ElementBoxes compute_element_boxes(const TextElement& element) {
// }

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
	const TextElement element = {
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
		.text = "New Game",
	};

	/* Text content */
	const Font& font = game.resources.default_font();
	const float font_spacing = 0.0f;
	const float text_width = Raylib_MeasureTextEx(font, element.text.c_str(), element.font_size, font_spacing).x;

	/* Box widths */
	const float content_width = text_width;
	const float content_height = element.font_size;
	const float padding_width = content_width + element.padding.left + element.padding.right;
	const float padding_height = content_height + element.padding.top + element.padding.bottom;
	const float border_width = padding_width + element.border.left + element.border.right;
	const float border_height = padding_height + element.border.top + element.border.bottom;
	const float margin_width = border_width + element.margin.left + element.margin.right;
	const float margin_height = border_height + element.margin.top + element.margin.bottom;

	/* Box position */
	const Vector2 margin_box_position = {
		.x = (float)(game.window.width() - margin_width) / 2.0f,
		.y = (float)(game.window.height() - margin_height) / 2.0f,
	};

	/* Box rectangles */
	const Rectangle margin_box = {
		.x = margin_box_position.x,
		.y = margin_box_position.y,
		.width = margin_width,
		.height = margin_height,
	};
	const Rectangle border_box = {
		.x = margin_box.x + element.margin.left,
		.y = margin_box.y + element.margin.top,
		.width = border_width,
		.height = border_height,
	};
	const Rectangle padding_box = {
		.x = border_box.x + element.border.left,
		.y = border_box.y + element.border.top,
		.width = padding_width,
		.height = padding_height,
	};
	const Rectangle content_box = {
		.x = padding_box.x + element.padding.left,
		.y = padding_box.y + element.padding.right,
		.width = content_width,
		.height = content_height,
	};

	/* Render boxes */
	const Vector2 content_pos = { content_box.x, content_box.y };
	const Color margin_color = { 240, 206, 163, 255 };
	const Color border_color = { 246, 221, 161, 255 };
	const Color padding_color = { 198, 207, 145, 255 };
	const Color content_color = { 152, 181, 192, 255 };
	Raylib_DrawRectangleRec(margin_box, margin_color);
	Raylib_DrawRectangleRec(border_box, border_color);
	Raylib_DrawRectangleRec(padding_box, padding_color);
	Raylib_DrawRectangleRec(content_box, content_color);
	Raylib_DrawTextEx(font, element.text.c_str(), content_pos, element.font_size, 0.0f, WHITE);
}
