#pragma once

#include "core/fixed_capacity_vector.h"
#include "game/resource_id.h"
#include "game/ui/context.h"
#include "game/ui/style.h"
#include "platform/input.h"

#include <raylib.h>

#include <string>
#include <string_view>
#include <variant>
#include <vector>

class ResourceManager;

namespace ui {

	/* Content */
	struct Element;

	// Store elements in fixed capacity vectors to guarantee that pointers to
	// elements won't get invalidated.
	using ElementVector = core::fixed_capacity_vector<Element, 10>;

	struct Text {
		std::string text;
		std::vector<std::string_view> lines; // computed during layout, views into `text` member
	};

	struct Image {
		ImageID id;
	};

	enum class Direction {
		Horizontal,
		Vertical,
	};

	struct Box {
		Direction direction = Direction::Vertical;
		ElementVector children;
	};

	using Content = std::variant<Box, Text, Image>;

	/* Layout */
	struct Layout {
		Rectangle margin_box;
		Rectangle border_box;
		Rectangle padding_box;
		Rectangle content_box;
	};

	/* Element */
	struct Element {
		std::string id;
		Style style;
		Content content;
		Layout layout; // computed with layout_element()
		Element* parent = nullptr;

		inline bool is_box() const {
			return std::holds_alternative<Box>(this->content);
		}

		inline Box* box() {
			return std::get_if<Box>(&this->content);
		}

		inline const Box* box() const {
			return std::get_if<Box>(&this->content);
		}

		inline bool is_text() const {
			return std::holds_alternative<Text>(this->content);
		}

		inline Text* text() {
			return std::get_if<Text>(&this->content);
		}

		inline const Text* text() const {
			return std::get_if<Text>(&this->content);
		}

		inline bool is_image() const {
			return std::holds_alternative<Image>(this->content);
		}

		inline Image* image() {
			return std::get_if<Image>(&this->content);
		}

		inline const Image* image() const {
			return std::get_if<Image>(&this->content);
		}
	};

	/* API */
	void layout_element(const ResourceManager& resources, const Context& context, Vector2 window_size, Element* element);
	bool update_element(const Input& input, Context* context, Element* element);
	void draw_element(const ResourceManager& resources, const Context& context, const Element& element);

} // namespace ui
