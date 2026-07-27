#pragma once

#include <stdexcept>
#include <type_traits>
#include <vector>

namespace core {

	// Heap allocated vector with fixed capacity.
	// Used for tree data structures, pointer to elements aren't invalidated.
	//
	// Based on static_vector by Palotás Boldizsár:
	// https://github.com/palotasb/static_vector/blob/master/include/palotasb/static_vector.hpp
	template <typename T, std::size_t Capacity>
	class fixed_capacity_vector {
	public:
		// Value type equal to T
		using value_type = T;
		using size_type = typename std::vector<T>::size_type;
		using difference_type = typename std::vector<T>::difference_type;
		using reference = typename std::vector<T>::reference;
		using const_reference = typename std::vector<T>::const_reference;
		using pointer = typename std::vector<T>::pointer;
		using const_pointer = typename std::vector<T>::const_pointer;
		using iterator = typename std::vector<T>::iterator;
		using const_iterator = typename std::vector<T>::const_iterator;
		using reverse_iterator = typename std::vector<T>::reverse_iterator;
		using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;
		// The static capacity of the fixed_capacity_vector
		static const size_type static_capacity = Capacity;

		fixed_capacity_vector() noexcept {
			m_values.reserve(Capacity);
		}

		fixed_capacity_vector(std::initializer_list<value_type> init_list)
			: m_values(init_list) {
			if (init_list.size() > Capacity) {
				throw std::length_error("initializer list size exceeds fixed_capacity_vector capacity");
			}
			m_values.reserve(Capacity);
		}

		// Constructor from std::vector (move)
		fixed_capacity_vector(std::vector<value_type>&& values)
			: m_values(std::move(values)) {
			if (m_values.size() > Capacity) {
				throw std::length_error("vector size exceeds fixed_capacity_vector capacity");
			}
			m_values.reserve(Capacity);
		}

		// Constructor from std::vector lvalue (moves contents)
		fixed_capacity_vector(std::vector<value_type>& values)
			: m_values(std::move(values)) {
			if (m_values.size() > Capacity) {
				throw std::length_error("vector size exceeds fixed_capacity_vector capacity");
			}
			m_values.reserve(Capacity);
		}

		// Copy constructor (defaulted - delegates to std::vector's copy)
		fixed_capacity_vector(const fixed_capacity_vector& other) = default;

		// Copy assignment
		fixed_capacity_vector& operator=(const fixed_capacity_vector& other) = default;

		// Move constructor
		fixed_capacity_vector(fixed_capacity_vector&& other) noexcept
			: m_values(std::move(other.m_values)) {
		}

		// Move assignment
		fixed_capacity_vector& operator=(fixed_capacity_vector&& other) noexcept {
			if (this != &other) {
				m_values = std::move(other.m_values);
			}
			return *this;
		}

		reference at(size_type index) {
			return m_values.at(index);
		}
		const_reference at(size_type index) const {
			return m_values.at(index);
		}

		reference operator[](size_t index) noexcept {
			return m_values[index];
		}
		const_reference operator[](size_t index) const noexcept {
			return m_values[index];
		}

		reference front() noexcept {
			return m_values.front();
		}
		const_reference front() const noexcept {
			return m_values.front();
		}

		reference back() noexcept {
			return m_values.back();
		}
		const_reference back() const noexcept {
			return m_values.back();
		}

		iterator begin() noexcept {
			return m_values.begin();
		}
		const_iterator begin() const noexcept {
			return m_values.begin();
		}
		const_iterator cbegin() const noexcept {
			return m_values.cbegin();
		}

		iterator end() noexcept {
			return m_values.end();
		}
		const_iterator end() const noexcept {
			return m_values.end();
		}
		const_iterator cend() const noexcept {
			return m_values.cend();
		}

		reverse_iterator rbegin() noexcept {
			return m_values.rbegin();
		}
		const_reverse_iterator rbegin() const noexcept {
			return m_values.rbegin();
		}
		const_reverse_iterator crbegin() const noexcept {
			return m_values.rbegin();
		}

		reverse_iterator rend() noexcept {
			return m_values.rend();
		}
		const_reverse_iterator rend() const noexcept {
			return m_values.rend();
		}
		const_reverse_iterator crend() const noexcept {
			return m_values.rend();
		}

		size_type size() const noexcept {
			return m_values.size();
		}

		bool empty() const noexcept {
			return m_values.empty();
		}

		size_type capacity() const noexcept {
			return static_capacity;
		}

		void clear() {
			m_values.clear();
		}

		void push_back(const value_type& value) {
			if (m_values.size() == Capacity) {
				throw std::out_of_range("push_back called on full fixed_capacity_vector");
			}
			m_values.push_back(value);
		}
		void push_back(value_type&& value) {
			if (m_values.size() == Capacity) {
				throw std::out_of_range("push_back called on full fixed_capacity_vector");
			}
			m_values.push_back(std::move(value));
		}

		bool operator==(const fixed_capacity_vector& rhs) const = default;
		auto operator<=>(const fixed_capacity_vector&) const = default;

	private:
		std::vector<T> m_values;
	};

} // namespace core
