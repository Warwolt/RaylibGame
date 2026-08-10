#pragma once

#include "game/animation.h"
#include "game/resource_id.h"
#include "game/ui/user_interface.h"

#include <raylib.h>

#include <vector>

struct Game;

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
	struct Animations {
		AnimationID<int> walk_left;
		AnimationID<int> walk_right;
		AnimationID<int> walk_down;
		AnimationID<int> walk_up;
	} m_animations;

	bool m_game_paused = false;
	ui::UserInterface m_ui;
	Vector2 m_player_position = { 0, 0 }; // relative center of player
	Vector2 m_camera_position = { 0, 0 }; // relative top left of viewport
	bool m_player_is_moving = false;
	SpriteSheet m_player_sprite_sheet;
	AnimationPlayback<int> m_player_sprite_animation;

	void _update_pause_menu(Game* game);
	void _update_gameplay(Game* game);
};
