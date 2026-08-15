#pragma once

#include "game/animation.h"
#include "game/resource_id.h"
#include "game/ui/user_interface.h"

#include <raylib.h>

#include <string>
#include <unordered_map>
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
	std::vector<Rectangle> frames;
	std::unordered_map<std::string, Animation<int>> animations;
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

	void _update_pause_menu(Game* game);
	void _update_gameplay(Game* game);
};
