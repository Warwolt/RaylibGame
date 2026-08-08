#include "core/util/time.h"

#include <windows.h>

Time Time::now() {
	static LARGE_INTEGER frequency = {};
	if (frequency.QuadPart == 0) {
		QueryPerformanceFrequency(&frequency);
	}
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	// Split into whole seconds and remainder to avoid overflow
	LONGLONG whole_seconds = counter.QuadPart / frequency.QuadPart;
	LONGLONG remainder = counter.QuadPart % frequency.QuadPart;
	return Time { std::chrono::nanoseconds(whole_seconds * 1000000000LL + (remainder * 1000000000LL) / frequency.QuadPart) };
}

Time Time::zero() {
	return std::chrono::nanoseconds(0);
}

Time::Time(std::chrono::nanoseconds ns)
	: value(ns) {
}

Time::Time(std::chrono::milliseconds ms)
	: value(ms) {
}

Time::Time(std::chrono::seconds sec)
	: value(sec) {
}

double Time::in_seconds() const {
	return std::chrono::duration<double>(value).count();
}

Time& Time::operator+=(const Time& rhs) {
	this->value += rhs.value;
	return *this;
}

Time& Time::operator-=(const Time& rhs) {
	this->value -= rhs.value;
	return *this;
}

Time operator+(Time lhs, const Time& rhs) {
	return lhs.value + rhs.value;
}

Time operator-(Time lhs, const Time& rhs) {
	return lhs.value - rhs.value;
}

Time operator*(float lhs, const Time& rhs) {
	return Time(std::chrono::duration_cast<std::chrono::milliseconds>(lhs * rhs.value));
}

int64_t operator/(Time lhs, const Time& rhs) {
	return lhs.value / rhs.value;
}

Time operator%(Time lhs, const Time& rhs) {
	return lhs.value % rhs.value;
}
