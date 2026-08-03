#include "game/scene/gameplay_scene.h"

#include "core/debug/logging.h"
#include "core/debug/profiling.h"
#include "core/util.h"
#include "game/game.h"

#include <raymath.h>

constexpr Vector2 PLAYER_SIZE = { 24, 30 }; // pixels
constexpr int PLAYER_SPEED = 4 * PLAYER_SIZE.x; // pixels per second
constexpr Vector2 LEVEL_SIZE = { 384, 160 };

void GameplayScene::initialize(Game* game) {
	m_ui.initialize(game);

	m_level_background = game->resources.load_image("resource/level/zelda_dungeon.png").value();

	// put player in center of level
	m_player_position = LEVEL_SIZE / 2.0;
}

void GameplayScene::deinitialize(Game* /*game*/) {
}

void GameplayScene::update(Game* game) {
	PROFILING_SCOPE();
	m_ui.frame_begin();

	/* Toggle pause menu */
	if (game->input.action_pressed(ACTION_PAUSE_GAME)) {
		m_game_paused = !m_game_paused;
	}

	/* Update state */
	if (m_game_paused) {
		_update_pause_menu(game);
	} else {
		_update_gameplay(game);
	}

	m_ui.frame_end(game->input, game->resources, game->window.size());
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
				.top = 15,
				.bottom = 15,
				.left = 50,
				.right = 50,
			},
			.background = {
				.color = BLACK,
			},
		};
	m_ui.box_begin(menu_container_style);
	{
		m_ui.menu_begin(menu_style);
		{
			if (m_ui.menu_item(game->input, game->resources, "Continue")) {
				m_game_paused = false;
			}

			if (m_ui.menu_item(game->input, game->resources, "Quit")) {
				game->scenes.queue_pop_scene();
			}
		}
		m_ui.menu_end();
	}
	m_ui.box_end();
}

void GameplayScene::_update_gameplay(Game* game) {
	PROFILING_SCOPE();

	const float delta_speed = game->input.time_delta.in_seconds() * PLAYER_SPEED;
	m_player_position += delta_speed * game->input.directional_input();
}

void GameplayScene::render(const Game& game) const {
	PROFILING_SCOPE();
	Raylib_ClearBackground(Color { 0, 0, 0, 255 });

	const Texture2D level_background = game.resources.get_image(m_level_background);
	const Vector2 room_size = { 288, 192 };

	Vector2 player_pixel_position = {
		.x = std::round(m_player_position.x),
		.y = std::round(m_player_position.y),
	};
	Vector2 player_room_position = {
		.x = std::floor(player_pixel_position.x / room_size.x),
		.y = std::floor(player_pixel_position.y / room_size.y),
	};

	const Vector2 camera_offset = { 72, 12 };
	const Vector2 viewport_position = room_size * player_room_position;
	const Camera2D camera = {
		.offset = camera_offset,
		.target = viewport_position,
		.zoom = 1.0f,
	};
	Raylib_BeginMode2D(camera);
	Raylib_BeginScissorMode(camera_offset.x, camera_offset.y, room_size.x, room_size.y);
	{
		/* Level */
		Raylib_DrawTexture(level_background, 0, 0, WHITE);

		/* Player */
		const Vector2 player_rect_pos = player_pixel_position - PLAYER_SIZE / 2.0f;
		Raylib_DrawRectangle(player_rect_pos.x, player_rect_pos.y, PLAYER_SIZE.x, PLAYER_SIZE.y, GREEN);
	}
	Raylib_EndScissorMode();
	Raylib_EndMode2D();

	/* HUD */
	Raylib_DrawTextEx(game.resources.get_font(FontID::default_font()), "Life: 8", { 8, 12 }, 16, 0, WHITE);
	Raylib_DrawTextEx(game.resources.get_font(FontID::default_font()), "Mana: 4", { 8, 12 + 16 }, 16, 0, WHITE);

	/* Render pause menu */
	m_ui.draw(game.resources);
}
