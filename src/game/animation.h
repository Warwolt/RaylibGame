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
class AnimatedValue {
public:
	void start() {
		if (!m_is_started) {
			m_is_started = true;
			m_start_time = Time::now();
		}
	}

	void stop() {
		m_is_started = false;
	}

	void set_animation(Animation<T> animation) {
		m_animation = animation;
	}

	T value_at_time(Time time_now) const {
		/* Check if animation is playing */
		if (!m_is_started) {
			return m_animation[0].value;
		}

		/* Compute duration */
		Time animation_duration = Time::zero();
		for (const AnimationFrame<T>& frame : m_animation) {
			animation_duration += frame.duration;
		}

		/* Find index of current frame */
		Time frame_start = Time::zero();
		size_t frame_index = 0;
		const Time t = (time_now - m_start_time) % animation_duration;
		for (const AnimationFrame<T>& frame : m_animation) {
			if (frame_start <= t && t < frame_start + frame.duration) {
				break;
			}
			frame_start += frame.duration;
			frame_index += 1;
		}

		return m_animation[frame_index].value;
	}

private:
	Animation<T> m_animation;
	bool m_is_started = false;
	Time m_start_time = Time::zero();
};
