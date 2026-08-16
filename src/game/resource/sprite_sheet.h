#pragma once

#include "game/animation.h"
#include "game/resource/resource_id.h"

#include <raylib.h>

#include <string>
#include <unordered_map>
#include <vector>

class ResourceManager;

struct SpriteSheet {
	ImageID image_id;
	std::vector<Rectangle> frames;
	std::unordered_map<std::string, Animation<int>> animations;
};

struct AnimatedSprite {
	SpriteSheetID sprite_sheet_id;
	AnimationPlayer<int> frame_animation;

	void set_animation(ResourceManager* resources, std::string animation_name);
	void set_animation_frame(int frame);
	void start_animation(Time time_now);
	void stop_animation();
	void update_animation(Time time_now);
	bool animation_is_playing() const;
	void draw(const ResourceManager& resources, Vector2 position) const;
};
