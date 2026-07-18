#pragma once

template <typename T>
class DoubleBuffer {
public:
	DoubleBuffer() = default;

	DoubleBuffer(T value)
		: m_first(value) {
	}

	T& current() {
		return m_index == 0 ? m_first : m_second;
	}

	const T& current() const {
		return m_index == 0 ? m_first : m_second;
	}

	T& previous() {
		return m_index == 0 ? m_second : m_first;
	}

	const T& previous() const {
		return m_index == 0 ? m_second : m_first;
	}

	T* operator->() {
		return m_index == 0 ? &m_first : &m_second;
	}

	const T* operator->() const {
		return m_index == 0 ? &m_first : &m_second;
	}

	void swap() {
		m_index = (m_index + 1) % 2;
	}

private:
	int m_index = 0;
	T m_first = {};
	T m_second = {};
};
