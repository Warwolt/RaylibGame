#include "game/scene/gameplay.h"

#include "game/game.h"

#include <raylib.h>

void GameplayScene::initialize(Game* /*game*/) {
}

void GameplayScene::deinitialize(Game* /*game*/) {
}

void GameplayScene::update(Game* game) {
	if (Raylib_IsKeyPressed(KEY_ESCAPE)) {
		game->scenes.pop_scene(game);
	}
}

void GameplayScene::render(const Game& game) const {
	const Font& font = game.resources.get_font(FontID::default_font());
	const int font_size = 32;
	const char* text = "Gameplay";
	const int text_width = Raylib_MeasureTextEx(font, text, font_size, 0.0f).x;
	const Vector2 pos = {
		.x = (game.window.width() - text_width) / 2.0f,
		.y = (game.window.height() - font_size) / 2.0f,
	};
	Raylib_DrawTextEx(font, text, pos, font_size, 0.0f, WHITE);
	Raylib_ClearBackground(Color { 0, 127, 0, 255 });
}
