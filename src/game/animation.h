#pragma once

#include "core/util/time.h"

#include <string>
#include <unordered_map>
#include <vector>

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

struct AnimationPlayback {
	AnimationID animation_id = AnimationID(0);
	bool is_started = false;
	Time start_time = Time::zero();

	void start() {
		this->is_started = true;
		this->start_time = Time::now();
	}

	void stop() {
		this->is_started = false;
	}
};

template <typename T>
const T& current_animation_frame(const std::vector<AnimationFrame<T>>& frames, const AnimationPlayback& playback) {
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
	const Time t = (Time::now() - playback.start_time) % animation_duration;
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
	AnimationID add_text_animation(Animation<std::string> animation);
	const Animation<std::string>& get_text_animation(AnimationID id) const;
	const std::string& current_text_frame(const AnimationPlayback& playback) const;

private:
	std::unordered_map<int, Animation<std::string>> m_text_animations;
	int m_next_animation_id = 1;
};
