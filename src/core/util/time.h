#pragma once

#include <chrono>

struct Time {
	std::chrono::nanoseconds value { 0 };
	static Time now(); // since epoch
	static Time zero();
	Time() = default;
	Time(std::chrono::nanoseconds ns);
	Time(std::chrono::milliseconds ms);
	Time(std::chrono::seconds sec);

	double in_seconds() const;

	auto operator<=>(const Time&) const = default;
	Time& operator+=(const Time& rhs);
	Time& operator-=(const Time& rhs);
	friend Time operator+(Time lhs, const Time& rhs);
	friend Time operator-(Time lhs, const Time& rhs);
	friend Time operator*(float lhs, const Time& rhs);
	friend int64_t operator/(Time lhs, const Time& rhs);
	friend Time operator%(Time lhs, const Time& rhs);
};
