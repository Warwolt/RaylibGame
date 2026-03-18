#include "game/scene/main_menu.h"

#include "game/game.h"

#include <raylib.h>

void MainMenuScene::update(Game* game) {
	if (Raylib_IsKeyPressed(KEY_ESCAPE)) {
		game->should_quit = true;
	}
}

void MainMenuScene::render(const Game& game) const {
	const int font_size = 32;
	const char* text = "Main Menu";
	const int text_width = Raylib_MeasureText(text, font_size);
	const int pos_x = (game.window.width() - text_width) / 2;
	const int pos_y = (game.window.height() - font_size) / 2;
	Raylib_DrawText(text, pos_x, pos_y, font_size, WHITE);
	Raylib_ClearBackground(Color { 0, 127, 127, 255 });
}
