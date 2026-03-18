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
	Raylib_DrawTextEx(font, text, Vector2 { pos_x, pos_y }, font_size, font_spacing, WHITE);
}
