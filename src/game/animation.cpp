#include "game/animation.h"

AnimationID<int> AnimationManager::add_animation(std::vector<AnimationFrame<int>> frames) {
	AnimationID id = AnimationID<int>(m_next_animation_id++);
	m_int_animations.insert({ id.value, Animation { frames } });
	return id;
}

AnimationID<std::string> AnimationManager::add_animation(std::vector<AnimationFrame<std::string>> frames) {
	AnimationID id = AnimationID<std::string>(m_next_animation_id++);
	m_text_animations.insert({ id.value, Animation { frames } });
	return id;
}

const Animation<int>& AnimationManager::get_animation(AnimationID<int> id) const {
	return m_int_animations.at(id.value);
}

const Animation<std::string>& AnimationManager::get_animation(AnimationID<std::string> id) const {
	return m_text_animations.at(id.value);
}

int AnimationManager::current_frame(const AnimationPlayback<int>& playback) const {
	const Animation<int>& animation = m_int_animations.at(playback.animation_id.value);
	return get_animation_frame(animation.frames, playback, Time::now());
}

const std::string& AnimationManager::current_frame(const AnimationPlayback<std::string>& playback) const {
	const Animation<std::string>& animation = m_text_animations.at(playback.animation_id.value);
	return get_animation_frame(animation.frames, playback, Time::now());
}
