#include "core/util/time.h"

Time Time::now() {
	return Time { std::chrono::high_resolution_clock::now().time_since_epoch() };
}

Time::Time(std::chrono::nanoseconds ns)
	: value(std::chrono::duration_cast<std::chrono::milliseconds>(ns)) {
}

Time::Time(std::chrono::milliseconds ms)
	: value(ms) {
}

Time::Time(std::chrono::seconds sec)
	: value(std::chrono::duration_cast<std::chrono::milliseconds>(sec)) {
}

float Time::in_seconds() const {
	return std::chrono::duration<float>(value).count();
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
