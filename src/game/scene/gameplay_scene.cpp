#include "game/scene/gameplay_scene.h"

#include "core/debug/profiling.h"
#include "game/game.h"

#include <raylib.h>

void GameplayScene::initialize(Game* game) {
	m_ui.initialize(game);
}

void GameplayScene::deinitialize(Game* /*game*/) {
}

void GameplayScene::update(Game* game) {
	PROFILING_SCOPE();
	if (game->input.key_pressed(KEY_ESCAPE)) {
		m_game_paused = true;
	}

	/* Pause menu*/
	if (m_game_paused) {
		m_ui.frame_begin();
		const ui::Style menu_container_style = {
			.alignment = ui::Alignment::Center,
			.cross_alignment = ui::Alignment::Center,
		};
		const ui::Style menu_style = {
			.fit_content = true,
			.border = {
				.edges = ui::Edges::uniform(4),
				.color = Color { 50, 60, 57, 255},
			},
			.padding = ui::Edges {
				.top = 25,
				.bottom = 25,
				.left = 75,
				.right = 75,
			},
			.background = {
				.color = BLACK,
			},
		};
		m_ui.box_begin(menu_container_style);
		m_ui.menu_begin(menu_style);
		{
			if (m_ui.menu_item(game->input, game->resources, "Continue")) {
				m_game_paused = false;
			}
			if (m_ui.menu_item(game->input, game->resources, "Quit")) {
				game->scenes.pop_scene(game);
				return;
			}
		}
		m_ui.menu_end();
		m_ui.box_end();
		m_ui.frame_end(game->input, game->resources, game->window.size());
	}
}

void GameplayScene::render(const Game& game) const {
	PROFILING_SCOPE();

	/* Render gameplay */
	const Font& font = game.resources.get_font(FontID::default_font());
	const int font_size = 32;
	const char* text = "Gameplay";
	const int text_width = Raylib_MeasureTextEx(font, text, font_size, 0.0f).x;
	const Vector2 pos = {
		.x = (game.window.width() - text_width) / 2.0f,
		.y = (game.window.height() - font_size) / 2.0f,
	};
	Raylib_ClearBackground(Color { 0, 127, 0, 255 });
	Raylib_DrawTextEx(font, text, pos, font_size, 0.0f, WHITE);

	/* Render pause menu */
	if (m_game_paused) {
		m_ui.draw(game.resources);
	}
}
