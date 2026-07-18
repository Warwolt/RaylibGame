#pragma once

template <typename T>
class DoubleBuffer {
public:
	DoubleBuffer() = default;

	DoubleBuffer(T value)
		: m_first(value) {
	}

	T& value() {
		return m_index == 0 ? m_first : m_second;
	}

	const T& value() const {
		return m_index == 0 ? m_first : m_second;
	}

	T& other() {
		return m_index == 0 ? m_second : m_first;
	}

	const T& other() const {
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
