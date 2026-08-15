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
