#pragma once

#include <chrono>

struct Time {
	std::chrono::milliseconds value = std::chrono::milliseconds(0);

	static Time now(); // since epoch
	Time() = default;
	Time(std::chrono::nanoseconds ns);
	Time(std::chrono::milliseconds ms);
	Time(std::chrono::seconds sec);

	float in_seconds() const;

	auto operator<=>(const Time&) const = default;
	Time& operator+=(const Time& rhs);
	Time& operator-=(const Time& rhs);
	friend Time operator+(Time lhs, const Time& rhs);
	friend Time operator-(Time lhs, const Time& rhs);
	friend Time operator*(float lhs, const Time& rhs);
	friend int64_t operator/(Time lhs, const Time& rhs);
	friend Time operator%(Time lhs, const Time& rhs);
};

template <typename T>
class Timestamped {
public:
	Timestamped() = default;

	Timestamped(T value)
		: m_value(value)
		, m_last_changed(Time::now()) {
	}

	Timestamped<T>& operator=(T value) noexcept {
		m_value = value;
		m_last_changed = Time::now();
		return *this;
	}

	const T& value() const {
		return m_value;
	}

	Time last_changed() const {
		return m_last_changed;
	}

private:
	T m_value;
	Time m_last_changed;
};
