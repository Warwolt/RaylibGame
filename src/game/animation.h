#pragma once

#include "core/util/time.h"

#include <optional>
#include <unordered_map>
#include <vector>

template <typename T>
struct AnimationFrame {
	T value;
	Time duration;
};

template <typename T>
struct Animation {
	std::vector<AnimationFrame<T>> frames;
	std::optional<Time> start_time;

	Time duration() const {
		Time duration = Time::zero();
		for (const AnimationFrame<T>& frame : frames) {
			duration += frame.duration;
		}
		return duration;
	}
};

struct AnimationID {
	int value;
	bool operator==(const AnimationID& rhs) const = default;
};

template <typename T>
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

class AnimationManager {
public:
	AnimationID add_text_animation(Animation<std::string> animation);
	const Animation<std::string>& get_text_animation(AnimationID id) const;
	const std::string& current_frame(const AnimationPlayback<std::string>& playback) const;

private:
	std::unordered_map<int, Animation<std::string>> m_text_animations;
	int m_next_animation_id = 1;
};
