#pragma once

#include "core/util/time.h"

#include <vector>

template <typename T>
struct AnimationFrame {
	T value;
	Time duration;
};

template <typename T>
T current_animation_frame(const std::vector<AnimationFrame<T>>& frames, Time animation_start, Time time_now) {
	/* Compute animation period */
	Time period = {};
	for (const AnimationFrame<T>& frame : frames) {
		period += frame.duration;
	}

	/* Find current frame */
	Time frame_start = {};
	const Time playback_time = (time_now - animation_start) % period;
	for (const AnimationFrame<T>& frame : frames) {
		if (frame_start <= playback_time && playback_time < frame_start + frame.duration) {
			return frame.value;
		}
		frame_start += frame.duration;
	}

	return {};
}
