#include "game/scene/gameplay_scene.h"

#include "core/debug/logging.h"
#include "core/debug/profiling.h"
#include "core/util.h"
#include "game/game.h"

#include <raymath.h>

constexpr Vector2 PLAYER_SIZE = { 16, 16 }; // pixels
constexpr int PLAYER_SPEED = 4 * PLAYER_SIZE.x; // pixels per second

constexpr Vector2 ROOM_SIZE = { 256, 128 };
constexpr int CAMERA_SPEED = 1 * ROOM_SIZE.x; // pixels per second

void GameplayScene::initialize(Game* game) {
	m_ui.initialize(game);
	m_images.level_background = game->resources.load_image("resource/level/zelda_dungeon.png").value();
	m_player.sprite_sheet.image = game->resources.load_image("resource/image/walk_animation.png").value();
	// clang-format off
	m_player.sprite_sheet.frames = {
		{ 0, 0, 16, 16 },
		{ 16, 0, 16, 16 },
		{ 32, 0, 16, 16 },
		{ 48, 0, 16, 16 },
		{ 64, 0, 16, 16 },
		{ 80, 0, 16, 16 },
		{ 96, 0, 16, 16 },
		{ 112, 0, 16, 16 },
	};
	// clang-format on

	m_player.position = ROOM_SIZE / 2.0;
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

	/* Camera should show the current room player is in */
	const Vector2 player_room_position = {
		.x = ROOM_SIZE.x * std::floor(m_player.position.x / ROOM_SIZE.x),
		.y = ROOM_SIZE.y * std::floor(m_player.position.y / ROOM_SIZE.y),
	};
	const Vector2 camera_target_delta = player_room_position - m_camera_position;
	const bool should_move_camera = m_camera_position != player_room_position;
	if (should_move_camera) {
		/* Move camera */
		const float delta_speed = game->input.time_delta.in_seconds() * CAMERA_SPEED;
		const float distance = Vector2Length(camera_target_delta);
		const float move_amount = std::min(delta_speed, distance);
		m_camera_position += move_amount * Vector2Normalize(camera_target_delta);

		/* Push player along with camera */
		const bool camera_moving_right = camera_target_delta.x > 0;
		const bool camera_moving_left = camera_target_delta.x < 0;
		const bool camera_moving_down = camera_target_delta.y > 0;
		const bool camera_moving_up = camera_target_delta.y < 0;
		if (camera_moving_right) {
			const float camera_left = m_camera_position.x;
			const float player_left = m_player.position.x - PLAYER_SIZE.x / 2;
			if (camera_left >= player_left) {
				m_player.position.x = camera_left + PLAYER_SIZE.x / 2;
			}
		}
		if (camera_moving_left) {
			const float camera_right = m_camera_position.x + ROOM_SIZE.x;
			const float player_right = m_player.position.x + PLAYER_SIZE.x / 2;
			if (camera_right <= player_right) {
				m_player.position.x = camera_right - PLAYER_SIZE.x / 2;
			}
		}
		if (camera_moving_down) {
			const float camera_top = m_camera_position.y;
			const float player_top = m_player.position.y - PLAYER_SIZE.x / 2;
			if (camera_top >= player_top) {
				m_player.position.y = camera_top + PLAYER_SIZE.y / 2;
			}
		}
		if (camera_moving_up) {
			const float camera_bottom = m_camera_position.y + ROOM_SIZE.y;
			const float player_bottom = m_player.position.y + PLAYER_SIZE.x / 2;
			if (camera_bottom <= player_bottom) {
				m_player.position.y = camera_bottom - PLAYER_SIZE.y / 2;
			}
		}
	}

	/* Allow player to move as long as camera isn't moving */
	const bool camera_is_moving = camera_target_delta != Vector2 { 0, 0 };
	if (!camera_is_moving) {
		const Vector2 directional_input = game->input.directional_input();
		const float delta_speed = game->input.time_delta.in_seconds() * PLAYER_SPEED;
		m_player.position += delta_speed * directional_input;
		m_player.is_moving = directional_input != Vector2 { 0, 0 };

		if (directional_input.x > 0) {
			m_player.direction = Direction::Right;
		}
		if (directional_input.x < 0) {
			m_player.direction = Direction::Left;
		}
		if (directional_input.y > 0) {
			m_player.direction = Direction::Down;
		}
		if (directional_input.y < 0) {
			m_player.direction = Direction::Up;
		}
	}
}

void GameplayScene::render(const Game& game) const {
	PROFILING_SCOPE();
	Raylib_ClearBackground(Color { 0, 0, 0, 255 });

	/* Play area viewport */
	const Vector2 camera_offset = { 0, 16 };
	const Camera2D camera = {
		.offset = camera_offset,
		.target = m_camera_position,
		.zoom = 1.0f,
	};
	Raylib_BeginMode2D(camera);
	Raylib_BeginScissorMode(camera_offset.x, camera_offset.y, ROOM_SIZE.x, ROOM_SIZE.y);
	{
		/* Level */
		Raylib_DrawTexture(game.resources.get_image(m_images.level_background), 0, 0, WHITE);

		/* Player */
		const Vector2 player_pixel_position = {
			.x = std::round(m_player.position.x),
			.y = std::round(m_player.position.y),
		};
		const Vector2 player_top_left = player_pixel_position - PLAYER_SIZE / 2.0f;

		const float period = 0.4f; // seconds
		const int animation_index = m_player.is_moving ? (std::fmod(game.input.time_now.in_seconds(), period) < period / 2.0f ? 0 : 1) : 0;
		int frame = 0;
		switch (m_player.direction) {
			case Direction::Left:
				frame = animation_index + 0;
				break;
			case Direction::Right:
				frame = animation_index + 2;
				break;
			case Direction::Down:
				frame = animation_index + 4;
				break;
			case Direction::Up:
				frame = animation_index + 6;
				break;
		}
		const Rectangle frame_source = m_player.sprite_sheet.frames[frame];
		Raylib_DrawTextureRec(game.resources.get_image(m_player.sprite_sheet.image), frame_source, player_top_left, WHITE);
	}
	Raylib_EndScissorMode();
	Raylib_EndMode2D();

	/* HUD */
	Raylib_DrawTextEx(game.resources.get_font(FontID::default_font()), "Life: 8", { 4, -1 }, 16, 0, WHITE);
	Raylib_DrawTextEx(game.resources.get_font(FontID::default_font()), "Mana: 4", { 4 + 56, -1 }, 16, 0, WHITE);
	Raylib_DrawTextEx(game.resources.get_font(FontID::default_font()), "Gold: 255", { 4 + 56 + 4 + 56, -1 }, 16, 0, WHITE);

	/* Render pause menu */
	m_ui.draw(game.resources);
}
