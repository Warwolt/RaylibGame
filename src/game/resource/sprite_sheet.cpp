#include "game/resource/sprite_sheet.h"

#include "game/resource/resource.h"

void AnimatedSprite::set_animation(ResourceManager* resources, std::string animation_name) {
	const SpriteSheet& sprite_sheet = resources->get_sprite_sheet(this->sprite_sheet_id);
	this->frame_animation.set_animation(sprite_sheet.animations.at(animation_name));
}

void AnimatedSprite::start_animation(Time time_now) {
	this->frame_animation.start(time_now);
}

void AnimatedSprite::stop_animation() {
	this->frame_animation.stop();
}

void AnimatedSprite::update_animation(Time time_now) {
	this->frame_animation.update(time_now);
}

void AnimatedSprite::draw(const ResourceManager& resources, Vector2 position) const {
	const int frame = this->frame_animation.value();
	const SpriteSheet& sprite_sheet = resources.get_sprite_sheet(this->sprite_sheet_id);
	const Rectangle source_rect = sprite_sheet.frames[frame];
	Raylib_DrawTextureRec(resources.get_image(sprite_sheet.image_id), source_rect, position, WHITE);
}
