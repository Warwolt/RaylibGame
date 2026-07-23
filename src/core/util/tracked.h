#pragma once

template <typename T>
class Tracked {
public:
	Tracked() = default;
	Tracked(T value)
		: m_current(value) {
	}
	Tracked(T current, T previous)
		: m_current(current)
		, m_previous(previous) {
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

	bool has_changed_to(const T& value) const {
		return m_current != m_previous && m_current == value;
	}

	T& operator=(T value) {
		m_previous = m_current;
		m_current = value;
		return m_current;
	}

	operator T() const {
		return m_current;
	}

	const T& operator*() const {
		return m_current;
	}

	const T* operator->() const {
		return &m_current;
	}

private:
	T m_current = {};
	T m_previous = {};
};
