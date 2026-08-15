#pragma once

#include "game/animation.h"
#include "game/resource_id.h"

#include <raylib.h>

#include <string>
#include <unordered_map>
#include <vector>

class ResourceManager;

struct SpriteSheetID {
	int value;
	bool operator==(const SpriteSheetID& rhs) const = default;
};

struct SpriteSheet {
	ImageID image;
	std::vector<Rectangle> frames;
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

class SpriteSheetManager {
public:
	std::optional<SpriteSheetID> load_aseprite_sprite_sheet(ResourceManager* resources, const std::string& image_path, const std::string& json_path);
	const SpriteSheet& get_sprite_sheet(SpriteSheetID sprite_sheet_id) const;

private:
	int m_next_sprite_sheet_id = 1;
	std::unordered_map<int, SpriteSheet> m_sprite_sheets;
	inline static const SpriteSheet m_empty_sprite_sheet = {};
};
