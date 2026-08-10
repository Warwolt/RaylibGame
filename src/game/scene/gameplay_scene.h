#pragma once

#include "game/resource_id.h"
#include "game/ui/user_interface.h"
#include "game/animation.h"

#include <raylib.h>

#include <vector>

struct Game;

enum class Direction {
	Up,
	Left,
	Down,
	Right,
};

struct SpriteSheet {
	ImageID image;
	std::vector<Rectangle> sprites;
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

	bool m_game_paused = false;
	ui::UserInterface m_ui;
	Vector2 m_player_position = { 0, 0 }; // relative center of player
	Vector2 m_camera_position = { 0, 0 }; // relative top left of viewport
	Direction m_player_direction = Direction::Right;
	bool m_player_is_moving = false;
	SpriteSheet m_player_sprite_sheet;
	struct Animations {
		AnimationID<int> walk_left;
		AnimationID<int> walk_right;
		AnimationID<int> walk_down;
		AnimationID<int> walk_up;
	} m_animations;

	void _update_pause_menu(Game* game);
	void _update_gameplay(Game* game);
};
