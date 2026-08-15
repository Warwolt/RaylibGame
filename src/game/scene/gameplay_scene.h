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

	struct Player {
		Vector2 position = { 0, 0 }; // relative center of player
		Direction direction = Direction::Right;
		bool is_moving = false;
		SpriteSheet sprite_sheet;
	};

	bool m_game_paused = false;
	ui::UserInterface m_ui;
	Player m_player;
	Vector2 m_camera_position = { 0, 0 }; // relative top left of viewport

	void _update_pause_menu(Game* game);
	void _update_gameplay(Game* game);
};
