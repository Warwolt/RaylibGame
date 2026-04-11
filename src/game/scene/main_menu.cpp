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
	const Font& font = game.resources.get_font(FontID::default_font());
	const int font_size = 32;
	const char* text = "Main Menu";
	const int text_width = Raylib_MeasureText(text, font_size);
	const Vector2 pos = {
		.x = (game.window.width() - text_width) / 2.0f,
		.y = (game.window.height() - font_size) / 2.0f,
	};
	Raylib_DrawTextEx(font, text, pos, font_size, 0.0f, WHITE);
	Raylib_ClearBackground(Color { 0, 127, 127, 255 });
}
