#pragma once

#include "core/util/time.h"

#include <string>
#include <unordered_map>
#include <vector>

template <typename T>
struct AnimationID {
	int value;
	bool operator==(const AnimationID& rhs) const = default;
};

template <typename T>
struct AnimationFrame {
	T value;
	Time duration;
};

template <typename T>
struct Animation {
	std::vector<AnimationFrame<T>> frames;
};

template <typename T>
struct AnimationPlayback {
	AnimationID<T> animation_id = AnimationID<T> { 0 };
	bool is_started = false;
	Time start_time = Time::zero();

	void set_animation(AnimationID<T> animation_id) {
		this->animation_id = animation_id;
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
const T& get_animation_frame(const std::vector<AnimationFrame<T>>& frames, const AnimationPlayback<T>& playback, Time time_now) {
	/* Check if animation is playing */
	if (!playback.is_started) {
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
	const Time t = (time_now - playback.start_time) % animation_duration;
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
	AnimationID<int> add_animation(std::vector<AnimationFrame<int>> frames);
	AnimationID<std::string> add_animation(std::vector<AnimationFrame<std::string>> frames);

	const Animation<int>& get_animation(AnimationID<int> id) const;
	const Animation<std::string>& get_animation(AnimationID<std::string> id) const;

	int current_frame(const AnimationPlayback<int>& playback) const;
	const std::string& current_frame(const AnimationPlayback<std::string>& playback) const;

private:
	std::unordered_map<int, Animation<int>> m_int_animations;
	std::unordered_map<int, Animation<std::string>> m_text_animations;
	int m_next_animation_id = 1;
};
