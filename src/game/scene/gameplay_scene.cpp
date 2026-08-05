#include "game/scene/gameplay_scene.h"

#include "core/debug/logging.h"
#include "core/debug/profiling.h"
#include "core/util.h"
#include "game/game.h"

#include <raymath.h>

constexpr Vector2 PLAYER_SIZE = { 16, 18 }; // pixels
constexpr int PLAYER_SPEED = 4 * PLAYER_SIZE.x; // pixels per second

constexpr Vector2 ROOM_SIZE = { 256, 128 };
constexpr int CAMERA_SPEED = 1 * ROOM_SIZE.x; // pixels per second

void GameplayScene::initialize(Game* game) {
	m_ui.initialize(game);
	m_images.level_background = game->resources.load_image("resource/level/zelda_dungeon.png").value();
	m_images.knight_sprite_sheet = game->resources.load_image("resource/image/walk_animation.png").value();

	m_player_position = ROOM_SIZE / 2.0;
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
		.x = ROOM_SIZE.x * std::floor(m_player_position.x / ROOM_SIZE.x),
		.y = ROOM_SIZE.y * std::floor(m_player_position.y / ROOM_SIZE.y),
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
			const float player_left = m_player_position.x - PLAYER_SIZE.x / 2;
			if (camera_left >= player_left) {
				m_player_position.x = camera_left + PLAYER_SIZE.x / 2;
			}
		}
		if (camera_moving_left) {
			const float camera_right = m_camera_position.x + ROOM_SIZE.x;
			const float player_right = m_player_position.x + PLAYER_SIZE.x / 2;
			if (camera_right <= player_right) {
				m_player_position.x = camera_right - PLAYER_SIZE.x / 2;
			}
		}
		if (camera_moving_down) {
			const float camera_top = m_camera_position.y;
			const float player_top = m_player_position.y - PLAYER_SIZE.x / 2;
			if (camera_top >= player_top) {
				m_player_position.y = camera_top + PLAYER_SIZE.y / 2;
			}
		}
		if (camera_moving_up) {
			const float camera_bottom = m_camera_position.y + ROOM_SIZE.y;
			const float player_bottom = m_player_position.y + PLAYER_SIZE.x / 2;
			if (camera_bottom <= player_bottom) {
				m_player_position.y = camera_bottom - PLAYER_SIZE.y / 2;
			}
		}
	}

	/* Allow player to move as long as camera isn't moving */
	const bool camera_is_moving = camera_target_delta != Vector2 { 0, 0 };
	if (!camera_is_moving) {
		const Vector2 directional_input = game->input.directional_input();
		const float delta_speed = game->input.time_delta.in_seconds() * PLAYER_SPEED;
		m_player_position += delta_speed * directional_input;
		m_player_is_moving = directional_input != Vector2 { 0, 0 };

		if (directional_input.x > 0) {
			m_player_direction = Direction::Right;
		}
		if (directional_input.x < 0) {
			m_player_direction = Direction::Left;
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
			.x = std::round(m_player_position.x),
			.y = std::round(m_player_position.y),
		};
		const Vector2 player_top_left = player_pixel_position - PLAYER_SIZE / 2.0f;

		// FIXME: we need some lightweight spritesheet animation system
		//
		// Spec:
		// When walking we should animate the player character
		// Walk animation should start from frame with legs apart
		// When idle frame with legs standing still should be used
		//
		// API:
		// Load sprite sheet.
		// Define animation (frames in sheet + duration of frames).
		// Start animation.
		// Stop animation.
		// Set current frame.
		// Flip horizontally.
		//
		const bool flip_horizontal = m_player_direction == Direction::Left;
		const float period = 0.4f; // seconds
		const int frame = m_player_is_moving ? (std::fmod(game.input.time_now.in_seconds(), period) < period / 2.0f ? 0 : 1) : 0;
		const Rectangle sprite_sheet_index = { frame * 16, 0, (flip_horizontal ? -1.0f : 1.0) * 16, 18 };
		Raylib_DrawTextureRec(game.resources.get_image(m_images.knight_sprite_sheet), sprite_sheet_index, player_top_left, WHITE);
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
