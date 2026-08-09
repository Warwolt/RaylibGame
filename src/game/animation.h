#pragma once

#include "core/util/time.h"

#include <optional>
#include <vector>

template <typename T>
struct AnimationFrame {
	T value;
	Time duration;
};

template <typename T>
struct FrameAnimation {
	std::vector<AnimationFrame<T>> frames;
	std::optional<Time> start_time;

	void start() {
		start_time = Time::now;
	}

	void stop() {
		start_time = std::nullopt;
	}

	Time total_duration() const {
		Time duration = Time::zero();
		for (const AnimationFrame<T>& frame : frames) {
			duration += frame.duration;
		}
		return duration;
	}

	T current_frame(Time time_now) const {
		if (!start_time.has_value()) {
			return this->frames[0].value;
		}

		Time frame_start = Time::zero();
		const Time t = (time_now - this->start_time.value()) % total_duration();
		size_t frame_index = 0;
		for (const AnimationFrame<T>& frame : this->frames) {
			if (frame_start <= t && t < frame_start + frame.duration) {
				break;
			}
			frame_start += frame.duration;
			frame_index += 1;
		}
		return this->frames[frame_index].value;
	}
};
