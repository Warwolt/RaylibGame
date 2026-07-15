#pragma once

#include <raylib.h>

template <typename T>
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
