#pragma once

#include <cmath>
#include <vector>

template <typename T>
struct AnimationFrame {
	T value;
	double duration; // seconds
};

template <typename T>
T current_animation_frame(const std::vector<AnimationFrame<T>>& frames, double animation_start, double time_now) {
	/* Compute animation period */
	double period = 0;
	for (const AnimationFrame<T>& frame : frames) {
		period += frame.duration;
	}

	/* Find current frame */
	double frame_start = 0;
	const double playback_time = fmod(time_now - animation_start, period);
	for (const AnimationFrame<T>& frame : frames) {
		if (frame_start <= playback_time && playback_time < frame_start + frame.duration) {
			return frame.value;
		}
		frame_start += frame.duration;
	}

	return {};
}
