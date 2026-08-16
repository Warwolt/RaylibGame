#pragma once

#include "game/resource/resource_id.h"

#include <raylib.h>

#include <string>
#include <unordered_map>
#include <vector>

struct SpriteSheet {
	ImageID image;
	std::vector<Rectangle> frames;
	std::unordered_map<std::string, Animation<int>> animations;
};
