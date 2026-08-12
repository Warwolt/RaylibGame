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

struct SpriteSheetID {
	int value;
	bool operator==(const SpriteSheetID& rhs) const = default;
};

struct Sprite {
	SpriteSheetID sprite_sheet_id;
	int frame;
};

struct AnimatedSprite {
	SpriteSheetID sprite_sheet_id;
	Animation<int> sprite_sheet_index;
};

using FrameIndex = int;

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

	struct PlayerAnimations {
		AnimationClipID<FrameIndex> walk_left;
		AnimationClipID<FrameIndex> walk_right;
		AnimationClipID<FrameIndex> walk_down;
		AnimationClipID<FrameIndex> walk_up;
	};

	struct Player {
		Vector2 position = { 0, 0 }; // relative center of player
		bool is_moving = false;
		AnimatedSprite sprite;
		PlayerAnimations animations;
	} m_player;

	bool m_game_paused = false;
	ui::UserInterface m_ui;
	Vector2 m_camera_position = { 0, 0 }; // relative top left of viewport
	SpriteSheet m_player_sprite_sheet;

	void _update_pause_menu(Game* game);
	void _update_gameplay(Game* game);
};
