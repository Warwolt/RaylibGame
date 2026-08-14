#pragma once

#include "game/animation.h"
#include "game/resource_id.h"
#include "game/ui/user_interface.h"

#include <raylib.h>

#include <string>
#include <unordered_map>
#include <vector>

struct Game;

// FIXME: who should own SpriteSheets?
// It's probably easiest to store them in the ResourceManager
// But, importing a sprite sheet depends on both an image and meta data
//
// So, probably best if we just have `ResourceManager::add_sprite_sheet` and
// have e.g. `load_aseprite_sprite_sheet` as a helper function that takes the
// path to the .png file, the .json file and ResourceManager* as pointer.
//
// Or, we just get a JSON parser and add a `ResoureManager::load_aseprite_sprite_sheet`
//
struct SpriteSheet {
	ImageID image;
	std::vector<Rectangle> animation;
	std::unordered_map<std::string, Animation<int>> animations;
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
	AnimatedValue<int> frame;
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

	struct Player {
		Vector2 position = { 0, 0 }; // relative center of player
		bool is_moving = false;
		AnimatedSprite sprite;
	} m_player;

	bool m_game_paused = false;
	ui::UserInterface m_ui;
	Vector2 m_camera_position = { 0, 0 }; // relative top left of viewport
	SpriteSheet m_player_sprite_sheet;

	void _update_pause_menu(Game* game);
	void _update_gameplay(Game* game);
};
