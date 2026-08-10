#include "game/animation.h"

AnimationID<std::string> AnimationManager::add_animation(Animation<std::string> animation) {
	AnimationID id = AnimationID<std::string>(m_next_animation_id++);
	m_text_animations.insert({ id.value, animation });
	return id;
}

const Animation<std::string>& AnimationManager::get_animation(AnimationID<std::string> id) const {
	return m_text_animations.at(id.value);
}

const std::string& AnimationManager::current_frame(const AnimationPlayback<std::string>& playback) const {
	const Animation<std::string>& animation = m_text_animations.at(playback.animation_id.value);
	return current_animation_frame(animation.frames, playback);
}
