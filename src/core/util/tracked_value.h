#pragma once

template <typename T>
class TrackedValue {
public:
	TrackedValue() = default;
	TrackedValue(T value)
		: m_current(value) {
	}

	const T& value() const {
		return m_current;
	}

	const T& previous() const {
		return m_previous;
	}

	bool has_changed() const {
		return m_current != m_previous;
	}

	T& operator=(T value) {
		m_previous = m_current;
		m_current = value;
		return m_current;
	}

	operator T() const {
		return m_current;
	}

	const TrackedValue& operator*() const {
		return m_current;
	}

	const TrackedValue* operator->() const {
		return &m_current;
	}

private:
	T m_current = {};
	T m_previous = {};
};
