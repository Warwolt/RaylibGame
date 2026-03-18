#include "game/scene/gameplay.h"

#include "game/game.h"

#include <raylib.h>

void GameplayScene::initialize(Game* /*game*/) {
}

void GameplayScene::deinitialize(Game* /*game*/) {
}

void GameplayScene::update(Game* game) {
	if (Raylib_IsKeyPressed(KEY_ESCAPE)) {
		game->scene_manager.pop_scene(game);
	}
}

void GameplayScene::render(const Game& game) const {
	const int font_size = 32;
	const char* text = "Gameplay";
	const int text_width = Raylib_MeasureText(text, font_size);
	const int pos_x = (game.window.width() - text_width) / 2;
	const int pos_y = (game.window.height() - font_size) / 2;
	Raylib_DrawText(text, pos_x, pos_y, font_size, WHITE);
	Raylib_ClearBackground(Color { 0, 127, 0, 255 });
}
