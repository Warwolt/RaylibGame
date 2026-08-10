#include "game/animation.h"

AnimationClipID<int> AnimationManager::add_animation(std::vector<AnimationFrame<int>> frames) {
	AnimationClipID id = AnimationClipID<int>(m_next_clip_id++);
	m_int_animations.insert({ id.value, AnimationClip { frames } });
	return id;
}

AnimationClipID<std::string> AnimationManager::add_animation(std::vector<AnimationFrame<std::string>> frames) {
	AnimationClipID id = AnimationClipID<std::string>(m_next_clip_id++);
	m_text_animations.insert({ id.value, AnimationClip { frames } });
	return id;
}

const AnimationClip<int>& AnimationManager::get_animation(AnimationClipID<int> id) const {
	return m_int_animations.at(id.value);
}

const AnimationClip<std::string>& AnimationManager::get_animation(AnimationClipID<std::string> id) const {
	return m_text_animations.at(id.value);
}

int AnimationManager::current_frame(const Animation<int>& animation) const {
	const AnimationClip<int>& clip = m_int_animations.at(animation.clip_id.value);
	return get_animation_frame(clip.frames, animation, Time::now());
}

const std::string& AnimationManager::current_frame(const Animation<std::string>& animation) const {
	const AnimationClip<std::string>& clip = m_text_animations.at(animation.clip_id.value);
	return get_animation_frame(clip.frames, animation, Time::now());
}
