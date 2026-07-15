#pragma once

#include <raylib.h>

struct Game;

enum class HotReloadState {
	Idle,
	Rebuilding,
	ReadyToReload,
	Failed,
};

template<typename T>
class Timestamped {
public:
	Timestamped() = default;

	Timestamped(T value)
		: m_value(value)
		, m_last_changed(Raylib_GetTime()) {
	}

	Timestamped<T>& operator=(T value) noexcept {
		m_value = value;
		m_last_changed = Raylib_GetTime();
		return *this;
	}

	const T& value() const {
		return m_value;
	}

	double last_changed() const {
		return m_last_changed;
	}

private:
	T m_value;
	double m_last_changed;
};

struct Debug {
	Timestamped<HotReloadState> reload_state;
};

void render_debug_overlay(const Game& game);
