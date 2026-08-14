#pragma once

#include "game/animation.h"

#include <raylib.h>

#include <string>
#include <unordered_map>
#include <vector>

struct SpriteSheetID {
	int value;
	bool operator==(const SpriteSheetID& rhs) const = default;
};

struct SpriteSheet {
	ImageID image;
	std::vector<Rectangle> animation;
	std::unordered_map<std::string, Animation<int>> animations;
};

struct Sprite {
	SpriteSheetID sprite_sheet_id;
	int frame;
};

struct AnimatedSprite {
	SpriteSheetID sprite_sheet_id;
	AnimatedValue<int> frame;
};
