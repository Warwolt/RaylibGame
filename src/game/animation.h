#pragma once

#include "core/util/time.h"

#include <string>
#include <unordered_map>
#include <vector>

template <typename T>
struct AnimationClipID {
	int value;
	bool operator==(const AnimationClipID& rhs) const = default;
};

template <typename T>
struct AnimationFrame {
	T value;
	Time duration;
};

template <typename T>
struct AnimationClip {
	std::vector<AnimationFrame<T>> frames;
};

template <typename T>
struct Animation {
	AnimationClipID<T> clip_id = AnimationClipID<T> { 0 };
	bool is_started = false;
	Time start_time = Time::zero();

	void set_clip(AnimationClipID<T> new_clip_id) {
		this->clip_id = new_clip_id;
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
const T& get_animation_frame(const std::vector<AnimationFrame<T>>& frames, const Animation<T>& animation, Time time_now) {
	/* Check if animation is playing */
	if (!animation.is_started) {
		return frames[0].value;
	}

	/* Compute duration */
	Time animation_duration = Time::zero();
	for (const AnimationFrame<T>& frame : frames) {
		animation_duration += frame.duration;
	}

	/* Find index of current frame */
	Time frame_start = Time::zero();
	size_t frame_index = 0;
	const Time t = (time_now - animation.start_time) % animation_duration;
	for (const AnimationFrame<T>& frame : frames) {
		if (frame_start <= t && t < frame_start + frame.duration) {
			break;
		}
		frame_start += frame.duration;
		frame_index += 1;
	}

	return frames[frame_index].value;
}

class AnimationManager {
public:
	AnimationClipID<int> add_animation(std::vector<AnimationFrame<int>> frames);
	AnimationClipID<std::string> add_animation(std::vector<AnimationFrame<std::string>> frames);

	const AnimationClip<int>& get_animation(AnimationClipID<int> id) const;
	const AnimationClip<std::string>& get_animation(AnimationClipID<std::string> id) const;

	int current_frame(const Animation<int>& animation) const;
	const std::string& current_frame(const Animation<std::string>& animation) const;

private:
	std::unordered_map<int, AnimationClip<int>> m_int_animations;
	std::unordered_map<int, AnimationClip<std::string>> m_text_animations;
	int m_next_clip_id = 1;
};
