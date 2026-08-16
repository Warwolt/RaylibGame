#pragma once

#include "game/resource/resource_id.h"
#include "game/resource/sprite_sheet.h"
#include "game/ui/user_interface.h"

#include <raylib.h>

struct Game;

enum class Direction {
	Up,
	Left,
	Down,
	Right,
};

class GameplayScene {
public:
	void initialize(Game* game);
	void deinitialize(Game* game);

	void update(Game* game);
	void render(const Game& game) const;

private:
	struct Images {
		ImageID level_background;
	} m_images;

	struct Player {
		Vector2 position = { 0, 0 }; // relative center of player
		AnimatedSprite sprite;
		Direction direction;
		std::vector<Direction> direction_stack;
	};

	bool m_game_paused = false;
	ui::UserInterface m_ui;
	Player m_player;
	Vector2 m_camera_position = { 0, 0 }; // relative top left of viewport

	void _update_pause_menu(Game* game);
	void _update_gameplay(Game* game);
};
