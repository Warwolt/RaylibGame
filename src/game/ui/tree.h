#pragma once

#include "game/ui/element.h"

#include <vector>

namespace ui {

	class TreeIterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = Element;
		using difference_type = std::ptrdiff_t;
		using pointer = Element*;
		using reference = Element&;

		TreeIterator() = default;
		explicit TreeIterator(Element* root);
		reference operator*() const;
		pointer operator->() const;
		TreeIterator& operator++();
		TreeIterator operator++(int);
		bool operator==(const TreeIterator& rhs) const = default;

	private:
		std::vector<Element*> m_stack;
	};

	class Tree {
	public:
		Tree();

		Element& root();
		const Element& root() const;

		Element& current_element(); // most recent non-closed element
		const Element& current_element() const;

		Element& current_parent();
		const Element& current_parent() const;

		void reset();
		void push_element(Element element);
		void close_element(); // i.e. close current box element
		bool has_open_element() const;

		TreeIterator begin();
		TreeIterator end();

	private:
		Element m_root;
		std::vector<Element*> m_parents;
	};

} // namespace ui
