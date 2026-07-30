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

	// For values that are only updated occasionally, e.g. in response to an
	// event, you can call `update` once per frame to make sure current and
	// previous values still update continously.
	//
	// If the value is updated every frame, i.e. always assigned
	// unconditionally, then you don't need to call this function.
	void update() {
		if (!m_write_since_last_update) {
			m_previous = m_current;
		}
		m_write_since_last_update = false;
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
		m_write_since_last_update = true;
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
	bool m_write_since_last_update = false;
};
