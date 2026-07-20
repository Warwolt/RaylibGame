#include "game/ui/tree.h"

namespace ui {

	TreeIterator::TreeIterator(Element* root) {
		if (root) {
			m_stack.push_back(root);
		}
	}

	TreeIterator::reference TreeIterator::operator*() const {
		return *m_stack.back();
	}
	TreeIterator::pointer TreeIterator::operator->() const {
		return m_stack.back();
	}

	TreeIterator& TreeIterator::operator++() {
		Element* current = m_stack.back();
		m_stack.pop_back();
		if (Box* box = current->box()) {
			for (auto it = box->children.rbegin(); it != box->children.rend(); ++it) {
				m_stack.push_back(&(*it));
			}
		}
		return *this;
	}

	TreeIterator TreeIterator::operator++(int) {
		TreeIterator tmp = *this;
		++(*this);
		return tmp;
	}

	Tree::Tree() {
		reset();
	}

	Element& Tree::root() {
		return m_root;
	}

	const Element& Tree::root() const {
		return m_root;
	}

	Element& Tree::current_element() {
		Element* parent = m_parents.back();
		std::vector<Element>& children = m_parents.back()->box()->children;
		return children.empty() ? *parent : children.back();
	}

	const Element& Tree::current_element() const {
		const Element* parent = m_parents.back();
		const std::vector<Element>& children = m_parents.back()->box()->children;
		return children.empty() ? *parent : children.back();
	}

	std::vector<Element*>& Tree::parents() {
		return m_parents;
	}

	void Tree::reset() {
		m_root = { .debug_name = "root", .content = ui::Box {} };
		m_parents = { &m_root };
	}

	void Tree::push_element(Element element) {
		Element* parent = m_parents.back();
		parent->box()->children.push_back(element);
		if (element.is_box()) {
			m_parents.push_back(&parent->box()->children.back());
		}
	}

	void Tree::close_element() {
		if (m_parents.size() > 1) {
			m_parents.pop_back();
		}
	}

	TreeIterator Tree::begin() {
		return TreeIterator(&m_root);
	}
	TreeIterator Tree::end() {
		return TreeIterator();
	}

} // namespace ui
