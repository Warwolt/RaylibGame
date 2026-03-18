#include "game/scene/main_menu.h"

#include "game/game.h"

#include <raylib.h>

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
	const Font& font = game.resources.default_font();
	const int font_size = 32;
	const float font_spacing = 0.0f;
	const char* text = "New Game";
	const int text_width = Raylib_MeasureTextEx(font, text, font_size, font_spacing).x;
	const float pos_x = (game.window.width() - text_width) / 2;
	const float pos_y = (game.window.height() - font_size) / 2;
	const Vector2 padding = { 4, 4 };
	const Vector2 font_adjust = { -1, -2 };
	Raylib_DrawRectangle(
		pos_x - padding.x + font_adjust.x,
		pos_y - padding.y + font_adjust.y,
		text_width + 2 * padding.x,
		font_size + 2 * padding.y,
		Color { 196, 196, 196, 255 }
	);
	Raylib_DrawRectangleLinesEx(
		{
			pos_x - padding.x + font_adjust.x,
			pos_y - padding.y + font_adjust.y,
			text_width + 2 * padding.x,
			font_size + 2 * padding.y,
		},
		2,
		Color { 130, 130, 130, 255 }
	);
	Raylib_DrawTextEx(font, text, Vector2 { pos_x, pos_y }, font_size, font_spacing, BLACK);
}
