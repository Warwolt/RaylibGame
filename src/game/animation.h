#pragma once

#include "core/util/time.h"

#include <vector>

template <typename T>
struct AnimationFrame {
	T value;
	Time duration;
};

// FIXME: should this be Animation<T>? And the animated thing be e.g. Animated<Sprite>?
template <typename T>
using AnimationClip = std::vector<AnimationFrame<T>>;

template <typename T>
struct Animation {
	AnimationClip<T> frames;
	bool is_started = false;
	Time start_time = Time::zero();

	void set_clip(AnimationClip<T> new_frames) {
		this->frames = new_frames;
	}

	void start() {
		if (!this->is_started) {
			this->is_started = true;
			this->start_time = Time::now();
		}
	}

	void stop() {
		this->is_started = false;
	}
};

template <typename T>
const T& get_animation_frame(const Animation<T>& animation, Time time_now) {
	/* Check if animation is playing */
	if (!animation.is_started) {
		return animation.frames[0].value;
	}

	/* Compute duration */
	Time animation_duration = Time::zero();
	for (const AnimationFrame<T>& frame : animation.frames) {
		animation_duration += frame.duration;
	}

	/* Find index of current frame */
	Time frame_start = Time::zero();
	size_t frame_index = 0;
	const Time t = (time_now - animation.start_time) % animation_duration;
	for (const AnimationFrame<T>& frame : animation.frames) {
		if (frame_start <= t && t < frame_start + frame.duration) {
			break;
		}
		frame_start += frame.duration;
		frame_index += 1;
	}

	return animation.frames[frame_index].value;
}
