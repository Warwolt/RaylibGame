#include "game/scene/gameplay_scene.h"

#include "core/debug/profiling.h"
#include "game/game.h"

#include <raymath.h>

constexpr int PLAYER_SIZE = 64; // pixels
constexpr int PLAYER_SPEED = 3 * PLAYER_SIZE; // pixels per second

void GameplayScene::initialize(Game* game) {
	m_ui.initialize(game);

	// put player in center of window
	m_player_position = (game->window.size() - Vector2 { PLAYER_SIZE, PLAYER_SIZE }) / 2.0f;
}

void GameplayScene::deinitialize(Game* /*game*/) {
}

void GameplayScene::update(Game* game) {
	PROFILING_SCOPE();

	/* Toggle pause menu */
	if (game->input.action_pressed(ACTION_PAUSE_GAME)) {
		m_game_paused = !m_game_paused;
	}

	if (m_game_paused) {
		_update_pause_menu(game);
	} else {
		_update_gameplay(game);
	}
}

void GameplayScene::_update_pause_menu(Game* game) {
	PROFILING_SCOPE();
	const ui::Style menu_container_style = {
		.alignment = ui::Alignment::Center,
		.cross_alignment = ui::Alignment::Center,
	};
	const ui::Style menu_style = {
			.fit_content = true,
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
	m_ui.frame_begin();
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

void GameplayScene::_update_gameplay(Game* game) {
	PROFILING_SCOPE();

	const float delta_speed = game->input.time_delta.in_seconds() * PLAYER_SPEED;
	m_player_position += delta_speed * game->input.directional_input();
}

void GameplayScene::render(const Game& game) const {
	PROFILING_SCOPE();

	/* Render gameplay */
	Raylib_ClearBackground(Color { 0, 0, 0, 255 });
	Raylib_DrawRectangle(m_player_position.x, m_player_position.y, PLAYER_SIZE, PLAYER_SIZE, GREEN);

	/* Render pause menu */
	if (m_game_paused) {
		m_ui.draw(game.resources);
	}
}
