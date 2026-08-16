#include "game/resource/sprite_sheet.h"

#include "game/resource/resource.h"

void AnimatedSprite::set_animation(ResourceManager* resources, std::string animation_name) {
	const SpriteSheet& sprite_sheet = resources->get_sprite_sheet(this->sprite_sheet_id);
	this->frame_animation.set_animation(sprite_sheet.animations.at(animation_name));
}
