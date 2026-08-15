#pragma once

#include "core/util/time.h"

#include <vector>

template <typename T>
struct AnimationFrame {
	T value;
	Time duration;
};

template <typename T>
using Animation = std::vector<AnimationFrame<T>>;

template <typename T>
T get_animation_value(const Animation<T>& animation, Time animation_start, Time time_now) {
	/* Compute animation period */
	Time period = {};
	for (const AnimationFrame<T>& frame : animation) {
		period += frame.duration;
	}

	/* Find current frame */
	Time frame_start = {};
	const Time playback_time = (time_now - animation_start) % period;
	for (const AnimationFrame<T>& frame : animation) {
		if (frame_start <= playback_time && playback_time < frame_start + frame.duration) {
			return frame.value;
		}
		frame_start += frame.duration;
	}

	return {};
}

template <typename T>
class AnimationPlayer {
public:
	void set_animation(Animation<T> animation) {
		m_animation = animation;
	}

	void start(Time time_now) {
		if (!m_is_playing) {
			m_is_playing = true;
			m_start_time = time_now;
		}
	}

	void stop() {
		m_is_playing = false;
	}

	T value(Time time_now) const {
		if (m_is_playing) {
			return get_animation_value(m_animation, m_start_time, time_now);
		} else {
			return m_animation[0].value;
		}
	}

private:
	Animation<T> m_animation;
	bool m_is_playing;
	Time m_start_time;
};
