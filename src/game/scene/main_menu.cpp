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
	const Margin margin = { .top = 10, .bottom = 10, .left = 10, .right = 10 };
	const Border border = { .top = 10, .bottom = 10, .left = 10, .right = 10 };
	const Padding padding = { .top = 10, .bottom = 10, .left = 10, .right = 10 };

	/* Text content */
	const Font& font = game.resources.default_font();
	const int font_size = 32;
	const float font_spacing = 0.0f;
	const char* text = "New Game";
	const float text_width = Raylib_MeasureTextEx(font, text, font_size, font_spacing).x;

	/* Box widths */
	const float content_width = text_width;
	const float content_height = font_size;
	const float padding_width = content_width + padding.left + padding.right;
	const float padding_height = content_height + padding.top + padding.bottom;
	const float border_width = padding_width + border.left + border.right;
	const float border_height = padding_height + border.top + border.bottom;
	const float margin_width = border_width + margin.left + margin.right;
	const float margin_height = border_height + margin.top + margin.bottom;

	/* Box position */
	const Vector2 box_position = {
		.x = (float)(game.window.width() - border_width) / 2.0f,
		.y = (float)(game.window.height() - border_height) / 2.0f,
	};

	/* Box rectangles */
	const Rectangle margin_box = {
		.x = box_position.x - margin.left,
		.y = box_position.y - margin.top,
		.width = margin_width,
		.height = margin_height,
	};
	const Rectangle border_box = {
		.x = box_position.x,
		.y = box_position.y,
		.width = border_width,
		.height = border_height,
	};
	const Rectangle padding_box = {
		.x = border_box.x + border.left,
		.y = border_box.y + border.top,
		.width = padding_width,
		.height = padding_height,
	};
	const Rectangle content_box = {
		.x = padding_box.x + padding.left,
		.y = padding_box.y + padding.right,
		.width = content_width,
		.height = content_height,
	};

	/* Render boxes */
	const Color margin_color = { 240, 206, 163, 255 };
	const Color border_color = { 246, 221, 161, 255 };
	const Color padding_color = { 198, 207, 145, 255 };
	const Color content_color = { 152, 181, 192, 255 };
	Raylib_DrawRectangleRec(margin_box, margin_color);
	Raylib_DrawRectangleRec(border_box, border_color);
	Raylib_DrawRectangleRec(padding_box, padding_color);
	Raylib_DrawRectangleRec(content_box, content_color);
}
