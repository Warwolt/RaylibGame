#include "game/animation.h"

template <typename T>
static const T& get_current_frame(const AnimationPlayback<T>& playback, const Animation<T>& animation) {
	if (!playback.is_started) {
		return animation.frames[0].value;
	}

	Time frame_start = Time::zero();
	size_t frame_index = 0;
	const Time t = (Time::now() - playback.start_time) % animation.duration();
	for (const AnimationFrame<T>& frame : animation.frames) {
		if (frame_start <= t && t < frame_start + frame.duration) {
			break;
		}
		frame_start += frame.duration;
		frame_index += 1;
	}
	return animation.frames[frame_index].value;
}

AnimationID AnimationManager::add_text_animation(Animation<std::string> animation) {
	AnimationID id = AnimationID(m_next_animation_id++);
	m_text_animations.insert({ id.value, animation });
	return id;
}

const Animation<std::string>& AnimationManager::get_text_animation(AnimationID id) const {
	return m_text_animations.at(id.value);
}

const std::string& AnimationManager::current_frame(const AnimationPlayback<std::string>& playback) const {
	const Animation<std::string>& animation = m_text_animations.at(playback.animation_id.value);
	return get_current_frame(playback, animation);
}
