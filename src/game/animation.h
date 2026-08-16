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

	void set_frame(int frame) {
		m_frame = frame;
		m_frame_remainder = Time::zero();
	}

	void start(Time time_now) {
		if (!m_is_playing) {
			m_is_playing = true;
			m_time_previous = time_now;
		}
	}

	void stop() {
		m_is_playing = false;
		m_frame = 0;
	}

	void update(Time time_now) {
		if (!m_is_playing || m_animation.empty()) {
			return;
		}

		Time delta_time = time_now - m_time_previous;
		m_time_previous = time_now;

		while (delta_time > Time::zero()) {
			if (m_frame_remainder <= delta_time) {
				delta_time -= m_frame_remainder;
				m_frame = (m_frame + 1) % m_animation.size();
				m_frame_remainder = m_animation[m_frame].duration;
			} else {
				m_frame_remainder -= delta_time;
				delta_time = Time::zero();
			}
		}
	}

	T value() const {
		return m_animation[m_frame].value;
	}

private:
	Animation<T> m_animation;
	int m_frame;
	bool m_is_playing;
	Time m_time_previous;
	Time m_frame_remainder;
};
